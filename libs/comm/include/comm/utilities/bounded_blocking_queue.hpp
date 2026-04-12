#pragma once

#include "comm_export.hpp"
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <expected>
#include <mutex>
#include <stop_token>
#include <utility>

namespace comm::utilities {

/**
 * @brief キュー操作が失敗した理由。
 */
enum class COMM_EXPORT queue_errc {
  /** @brief キューが close 済み（push 不可、pop は空なら終了）。 */
  closed,
  /** @brief タイムアウト。 */
  timeout,
  /** @brief stop_token によるキャンセル要求。 */
  cancelled
};

/**
 * @brief 容量上限付きスレッドセーフブロッキングキュー。
 *
 * - キューが満杯の場合、push 系操作は空きができるまでブロックする。
 * - キューが空の場合、pop 系操作は要素が追加されるまでブロックする。
 * - close() を呼ぶと待機中のスレッドを起床し、以後 push は失敗する。
 *   pop は「残っている要素を取り尽くした後」、closed を返して終了できる。
 *
 * 追加機能:
 * - std::stop_token による待機中キャンセル
 * - std::expected による失敗理由の返却
 *
 * @tparam T 格納する要素型
 */
template <class T>
class COMM_EXPORT bounded_blocking_queue {
public:
  /** @brief 要素型。 */
  using value_type = T;
  /** @brief サイズ型。 */
  using size_type  = std::size_t;

  /**
   * @brief コンストラクタ。
   * @param capacity キューの最大容量（0 は非推奨）
   */
  explicit bounded_blocking_queue(size_type capacity)
      : capacity_(capacity) {}

  /** @brief コピー禁止。 */
  bounded_blocking_queue(const bounded_blocking_queue&) = delete;
  /** @brief コピー代入禁止。 */
  bounded_blocking_queue& operator=(const bounded_blocking_queue&) = delete;

  /**
   * @brief キューをクローズし、待機中のスレッドをすべて起床させる。
   *
   * close 後は push 系は失敗し、pop 系は「キューが空になった時点」で
   * queue_errc::closed を返す。
   */
  void close() noexcept {
    {
      std::lock_guard lk(m_);
      closed_ = true;
    }
    cv_not_empty_.notify_all();
    cv_not_full_.notify_all();
  }

  /**
   * @brief クローズ済みかどうか。
   * @return クローズ済みなら true
   */
  [[nodiscard]] bool closed() const noexcept {
    std::lock_guard lk(m_);
    return closed_;
  }

  /**
   * @brief 現在の要素数（スナップショット）。
   *
   * @note 並行実行中に値は変化し得るため、制御ロジックには使わないことを推奨。
   * @return 要素数
   */
  [[nodiscard]] size_type size() const noexcept {
    std::lock_guard lk(m_);
    return q_.size();
  }

  // ---------------------------------------------------------------------------
  // 互換API（bool 返却）: 既存コード互換向け
  // ---------------------------------------------------------------------------

  /**
   * @brief 要素を追加（無期限ブロック）。
   * @param item 追加する要素
   * @return 追加成功なら true、close 済みなら false
   */
  [[nodiscard]] bool push(T item) {
    return push(std::move(item), std::stop_token{}).has_value();
  }

  /**
   * @brief 要素を追加（タイムアウト付き）。
   * @tparam Rep 期間の表現
   * @tparam Period 期間の単位
   * @param item 追加する要素
   * @param timeout 待機上限
   * @return 追加成功なら true、timeout または close 済みなら false
   */
  template <class Rep, class Period>
  [[nodiscard]] bool push_for(T item, std::chrono::duration<Rep, Period> timeout) {
    return push_for(std::move(item), timeout, std::stop_token{}).has_value();
  }

  /**
   * @brief 要素を取り出し（無期限ブロック）。
   * @param[out] out 取り出した要素
   * @return 取り出し成功なら true、close 済みかつ空なら false
   */
  [[nodiscard]] bool pop(T& out) {
    auto r = pop(std::stop_token{});
    if (!r) return false;
    out = std::move(*r);
    return true;
  }

  /**
   * @brief 要素を取り出し（タイムアウト付き）。
   * @tparam Rep 期間の表現
   * @tparam Period 期間の単位
   * @param[out] out 取り出した要素
   * @param timeout 待機上限
   * @return 取り出し成功なら true、timeout または空なら false
   */
  template <class Rep, class Period>
  [[nodiscard]] bool pop_for(T& out, std::chrono::duration<Rep, Period> timeout) {
    auto r = pop_for(timeout, std::stop_token{});
    if (!r) return false;
    out = std::move(*r);
    return true;
  }

  // ---------------------------------------------------------------------------
  // 推奨API（expected 返却）: 失敗理由が欲しい場合はこちら
  // ---------------------------------------------------------------------------

  /**
   * @brief 要素を追加（無期限ブロック、stop_token 対応）。
   * @param item 追加する要素
   * @param st キャンセル用 stop_token
   * @return 成功: expected<void>、失敗: queue_errc（closed/cancelled）
   */
  [[nodiscard]] std::expected<void, queue_errc> push(T item, std::stop_token st) {
    std::unique_lock lk(m_);

    /**
     * @note stop_token 付き待機を行うため、condition_variable_any を使用。
     *       pred が true で解除、stop 要求で解除（false）となる。
     */
    const bool ready = cv_not_full_.wait(lk, st, [&] {
      return closed_ || q_.size() < capacity_;
    });

    if (!ready) return std::unexpected(queue_errc::cancelled);
    if (closed_) return std::unexpected(queue_errc::closed);

    q_.push_back(std::move(item));
    lk.unlock();
    cv_not_empty_.notify_one();
    return {};
  }

  /**
   * @brief 要素を追加（タイムアウト付き、stop_token 対応）。
   * @tparam Rep 期間の表現
   * @tparam Period 期間の単位
   * @param item 追加する要素
   * @param timeout 待機上限
   * @param st キャンセル用 stop_token
   * @return 成功: expected<void>、失敗: queue_errc（closed/timeout/cancelled）
   */
  template <class Rep, class Period>
  [[nodiscard]] std::expected<void, queue_errc>
  push_for(T item, std::chrono::duration<Rep, Period> timeout, std::stop_token st) {
    using clock = std::chrono::steady_clock;
    const auto deadline = clock::now() + timeout;

    std::unique_lock lk(m_);

    while (!closed_ && q_.size() >= capacity_) {
      if (st.stop_requested())
        return std::unexpected(queue_errc::cancelled);

      if (cv_not_full_.wait_until(lk, deadline) == std::cv_status::timeout) {
        if (!closed_ && q_.size() >= capacity_)
          return std::unexpected(queue_errc::timeout);
      }
    }

    if (closed_) return std::unexpected(queue_errc::closed);

    q_.push_back(std::move(item));
    lk.unlock();
    cv_not_empty_.notify_one();
    return {};
  }

  /**
   * @brief 要素を取り出し（無期限ブロック、stop_token 対応）。
   * @param st キャンセル用 stop_token
   * @return 成功: expected<T>、失敗: queue_errc（closed/cancelled）
   */
  [[nodiscard]] std::expected<T, queue_errc> pop(std::stop_token st) {
    std::unique_lock lk(m_);

    const bool ready = cv_not_empty_.wait(lk, st, [&] {
      return closed_ || !q_.empty();
    });

    if (!ready) return std::unexpected(queue_errc::cancelled);

    if (q_.empty()) {
      return std::unexpected(queue_errc::closed);
    }

    T out = std::move(q_.front());
    q_.pop_front();
    lk.unlock();
    cv_not_full_.notify_one();
    return out;
  }

  /**
   * @brief 要素を取り出し（タイムアウト付き、stop_token 対応）。
   * @tparam Rep 期間の表現
   * @tparam Period 期間の単位
   * @param timeout 待機上限
   * @param st キャンセル用 stop_token
   * @return 成功: expected<T>、失敗: queue_errc（closed/timeout/cancelled）
   */
  template <class Rep, class Period>
  [[nodiscard]] std::expected<T, queue_errc>
  pop_for(std::chrono::duration<Rep, Period> timeout, std::stop_token st) {
    using clock = std::chrono::steady_clock;
    const auto deadline = clock::now() + timeout;

    std::unique_lock lk(m_);

    while (!closed_ && q_.empty()) {
      if (st.stop_requested())
        return std::unexpected(queue_errc::cancelled);

      if (cv_not_empty_.wait_until(lk, deadline) == std::cv_status::timeout) {
        if (!closed_ && q_.empty())
          return std::unexpected(queue_errc::timeout);
      }
    }

    if (q_.empty()) {
      return std::unexpected(queue_errc::closed);
    }

    T out = std::move(q_.front());
    q_.pop_front();
    lk.unlock();
    cv_not_full_.notify_one();
    return out;
  }

private:
  /** @brief キュー最大容量。 */
  const size_type capacity_;
  /** @brief 排他用ミューテックス。 */
  mutable std::mutex m_;
  /** @brief 満杯ではないことを待つ条件変数。 */
  std::condition_variable_any cv_not_full_;
  /** @brief 空ではないことを待つ条件変数。 */
  std::condition_variable_any cv_not_empty_;
  /** @brief 要素格納先。 */
  std::deque<T> q_;
  /** @brief close 済みフラグ。 */
  bool closed_ = false;
};

} // namespace comm::utilities
