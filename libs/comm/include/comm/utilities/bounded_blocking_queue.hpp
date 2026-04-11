/**
 * @file bounded_blocking_queue.hpp
 * @brief 容量上限付きスレッドセーフブロッキングキュー
 */

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <cstddef>
#include <deque>
#include <utility>

namespace comm::utilities {

/**
 * @brief 容量上限付きのスレッドセーフなブロッキングキュー。
 *
 * プロデューサー／コンシューマーパターン向けに設計されており、キューが満杯の場合は
 * push 操作が、空の場合は pop
 * 操作がそれぞれ空きまたは要素が生じるまでブロックする。
 * @c close() を呼び出すことで待機中のスレッドを例外なしに安全に終了させられる。
 *
 * @tparam T キューに格納する要素の型。
 */
template <class T> class bounded_blocking_queue {
public:
  /**
   * @brief キューを構築する。
   * @param capacity キューに格納できる要素の最大数。
   */
  explicit bounded_blocking_queue(std::size_t capacity) : capacity_(capacity) {}

  /**
   * @brief キューをクローズし、待機中のすべてのスレッドを起床させる。
   *
   * クローズ後は push / pop ともに @c false を返す。
   * スレッドを例外なしに終了させたい場合に使用する。
   */
  void close() {
    boost::unique_lock<boost::mutex> lk(m_);
    closed_ = true;
    lk.unlock();
    cv_not_empty_.notify_all();
    cv_not_full_.notify_all();
  }

  /**
   * @brief キューがクローズ済みかどうかを返す。
   * @return クローズ済みであれば @c true。
   */
  bool closed() const {
    boost::lock_guard<boost::mutex> lk(m_);
    return closed_;
  }

  /**
   * @brief 現在キューに格納されている要素数を返す。
   * @return 要素数。
   */
  std::size_t size() const {
    boost::lock_guard<boost::mutex> lk(m_);
    return q_.size();
  }

  /**
   * @brief 要素をキューに追加する（無期限ブロック）。
   *
   * キューが満杯の場合は空きができるまでブロックする。
   * @param item 追加する要素（ムーブされる）。
   * @return 追加に成功した場合は @c true、キューがクローズ済みの場合は @c
   * false。
   */
  bool push(T item) {
    boost::unique_lock<boost::mutex> lk(m_);
    while (!closed_ && q_.size() >= capacity_) {
      cv_not_full_.wait(lk);
    }
    if (closed_)
      return false;

    q_.push_back(std::move(item));
    lk.unlock();
    cv_not_empty_.notify_one();
    return true;
  }

  /**
   * @brief 要素をキューに追加する（タイムアウト付き）。
   *
   * キューが満杯の場合は空きができるか @p timeout が経過するまでブロックする。
   * @param item 追加する要素（ムーブされる）。
   * @param timeout 待機する最大時間（例: @c
   * boost::posix_time::milliseconds(100)）。
   * @return 追加に成功した場合は @c
   * true、タイムアウトまたはクローズ済みの場合は @c false。
   */
  bool push_for(T item, boost::posix_time::time_duration timeout) {
    boost::unique_lock<boost::mutex> lk(m_);
    const auto deadline = boost::get_system_time() + timeout;

    while (!closed_ && q_.size() >= capacity_) {
      if (!cv_not_full_.timed_wait(lk, deadline)) {
        return false;
      }
    }
    if (closed_)
      return false;

    q_.push_back(std::move(item));
    lk.unlock();
    cv_not_empty_.notify_one();
    return true;
  }

  /**
   * @brief キューから要素を取り出す（無期限ブロック）。
   *
   * キューが空の場合は要素が追加されるまでブロックする。
   * @param[out] out 取り出した要素の格納先。
   * @return 取り出しに成功した場合は @c
   * true、クローズ済みかつキューが空の場合は @c false。
   */
  bool pop(T &out) {
    boost::unique_lock<boost::mutex> lk(m_);
    while (!closed_ && q_.empty()) {
      cv_not_empty_.wait(lk);
    }
    if (q_.empty()) {
      return false;
    }

    out = std::move(q_.front());
    q_.pop_front();
    lk.unlock();
    cv_not_full_.notify_one();
    return true;
  }

  /**
   * @brief キューから要素を取り出す（タイムアウト付き）。
   *
   * キューが空の場合は要素が追加されるか @p timeout
   * が経過するまでブロックする。
   * @param[out] out 取り出した要素の格納先。
   * @param timeout 待機する最大時間（例: @c
   * boost::posix_time::milliseconds(100)）。
   * @return 取り出しに成功した場合は @c
   * true、タイムアウトまたはキューが空の場合は @c false。
   */
  bool pop_for(T &out, boost::posix_time::time_duration timeout) {
    boost::unique_lock<boost::mutex> lk(m_);
    const auto deadline = boost::get_system_time() + timeout;

    while (!closed_ && q_.empty()) {
      if (!cv_not_empty_.timed_wait(lk, deadline)) {
        return false;
      }
    }
    if (q_.empty())
      return false;

    out = std::move(q_.front());
    q_.pop_front();
    lk.unlock();
    cv_not_full_.notify_one();
    return true;
  }

private:
  const std::size_t capacity_; ///< キューの最大容量。
  mutable boost::mutex m_;     ///< キューへのアクセスを保護するミューテックス。
  boost::condition_variable
      cv_not_full_; ///< キューに空きができたことを通知する条件変数。
  boost::condition_variable
      cv_not_empty_;    ///< キューに要素が追加されたことを通知する条件変数。
  std::deque<T> q_;     ///< 要素を格納するデック。
  bool closed_ = false; ///< クローズ状態フラグ。
};

} // namespace comm::utilities
