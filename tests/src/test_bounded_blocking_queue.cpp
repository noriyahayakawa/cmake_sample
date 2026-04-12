/**
 * @file test_bounded_blocking_queue.cpp
 * @brief bounded_blocking_queue の単体テスト。
 */

#include "comm/utilities/bounded_blocking_queue.hpp"

#include <atomic>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <gtest/gtest.h>
#include <stop_token>
#include <vector>

using Queue = comm::utilities::bounded_blocking_queue<int>;

/**
 * @brief 構築直後のキューはサイズが 0 でクローズされていないことを確認する。
 */
TEST(bounded_blocking_queue_test, initial_state_is_empty_and_not_closed) {
  Queue q(4);
  EXPECT_EQ(q.size(), 0u);
  EXPECT_FALSE(q.closed());
}

/**
 * @brief push した値が pop で取り出せることを確認する。
 */
TEST(bounded_blocking_queue_test, push_and_pop_preserve_value) {
  std::stop_source stop_source;
  std::stop_token token = stop_source.get_token();
  Queue q(4);
  [[maybe_unused]] auto r1 = q.push(42, token);
  ASSERT_TRUE(r1.has_value());
  [[maybe_unused]] auto r2 = q.pop(token);
  ASSERT_TRUE(r2.has_value());
  EXPECT_EQ(*r2, 42);
}

/**
 * @brief 複数の要素が FIFO 順で取り出されることを確認する。
 */
TEST(bounded_blocking_queue_test, fifo_order_is_maintained) {
  Queue q(4);
  [[maybe_unused]] auto _r1 = q.push(1, std::stop_token{});
  [[maybe_unused]] auto _r2 = q.push(2, std::stop_token{});
  [[maybe_unused]] auto _r3 = q.push(3, std::stop_token{});
  [[maybe_unused]] auto _a = q.pop(std::stop_token{});
  [[maybe_unused]] auto _b = q.pop(std::stop_token{});
  [[maybe_unused]] auto _c = q.pop(std::stop_token{});
  EXPECT_EQ(*_a, 1);
  EXPECT_EQ(*_b, 2);
  EXPECT_EQ(*_c, 3);
}

/**
 * @brief size() が push/pop に追従することを確認する。
 */
TEST(bounded_blocking_queue_test, size_reflects_push_and_pop) {
  Queue q(4);
  EXPECT_EQ(q.size(), 0u);
  [[maybe_unused]] auto _r2 = q.push(1, std::stop_token{});
  EXPECT_EQ(q.size(), 1u);
  [[maybe_unused]] auto _r3 = q.push(2, std::stop_token{});
  EXPECT_EQ(q.size(), 2u);
  [[maybe_unused]] auto _v = q.pop(std::stop_token{});
  EXPECT_EQ(q.size(), 1u);
}

/**
 * @brief close 後は closed() が true を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, closed_returns_true_after_close) {
  Queue q(4);
  EXPECT_FALSE(q.closed());
  q.close();
  EXPECT_TRUE(q.closed());
}

/**
 * @brief close 後の push が false を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, push_returns_false_after_close) {
  Queue q(4);
  q.close();
  auto r = q.push(1, std::stop_token{});
  EXPECT_FALSE(r.has_value());
}

/**
 * @brief close 後に空のキューへ pop すると false を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, pop_returns_false_when_closed_and_empty) {
  Queue q(4);
  q.close();
  auto r = q.pop(std::stop_token{});
  EXPECT_FALSE(r.has_value());
}

/**
 * @brief close 後もキューに残っている要素はすべて pop できることを確認する。
 */
TEST(bounded_blocking_queue_test,
     pop_drains_items_then_returns_false_after_close) {
  Queue q(3);
  [[maybe_unused]] auto _r1 = q.push(10, std::stop_token{});
  [[maybe_unused]] auto _r2 = q.push(20, std::stop_token{});
  q.close();
  [[maybe_unused]] auto r1 = q.pop(std::stop_token{});
  EXPECT_TRUE(r1.has_value());
  EXPECT_EQ(*r1, 10);
  [[maybe_unused]] auto r2 = q.pop(std::stop_token{});
  EXPECT_TRUE(r2.has_value());
  EXPECT_EQ(*r2, 20);
  [[maybe_unused]] auto r3 = q.pop(std::stop_token{});
  EXPECT_FALSE(r3.has_value());
}

/**
 * @brief push_for は要素数が capacity 未満であれば true を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, push_for_succeeds_when_space_available) {
  Queue q(4);
  [[maybe_unused]] auto result =
      q.push_for(7, std::chrono::milliseconds(100), std::stop_token{});
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(q.size(), 1u);
}

/**
 * @brief push_for は満杯のキューへのタイムアウト時に false
 * を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, push_for_returns_false_on_timeout_when_full) {
  Queue q(1);
  [[maybe_unused]] auto _result = q.push(0, std::stop_token{});
  int dummy = 99;
  [[maybe_unused]] auto result =
      q.push_for(dummy, std::chrono::milliseconds(50), std::stop_token{});
  EXPECT_FALSE(result.has_value());
}

/**
 * @brief pop_for は要素がある場合に true を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, pop_for_succeeds_when_item_available) {
  Queue q(4);
  [[maybe_unused]] auto _result = q.push(7, std::stop_token{});
  [[maybe_unused]] auto result =
      q.pop_for(std::chrono::milliseconds(100), std::stop_token{});
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(*result, 7);
}

/**
 * @brief pop_for は空のキューへのタイムアウト時に false を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, pop_for_returns_false_on_timeout_when_empty) {
  Queue q(4);
  [[maybe_unused]] auto result =
      q.pop_for(std::chrono::milliseconds(50), std::stop_token{});
  EXPECT_FALSE(result.has_value());
}

/**
 * @brief close により、pop でブロック中のスレッドが起床することを確認する。
 */
TEST(bounded_blocking_queue_test, close_unblocks_waiting_pop) {
  Queue q(4);
  std::atomic<bool> result{true};
  boost::thread t([&] {
    [[maybe_unused]] auto r = q.pop(std::stop_token{});
    result.store(r.has_value());
  });
  boost::this_thread::sleep(boost::posix_time::milliseconds(50));
  q.close();
  t.join();
  EXPECT_FALSE(result.load());
}

/**
 * @brief close により、満杯キューへの push
 * でブロック中のスレッドが起床することを確認する。
 */
TEST(bounded_blocking_queue_test, close_unblocks_waiting_push) {
  Queue q(1);
  [[maybe_unused]] auto _r = q.push(0, std::stop_token{});
  std::atomic<bool> result{true};
  boost::thread t([&] {
    [[maybe_unused]] auto r = q.push(99, std::stop_token{});
    result.store(r.has_value());
  });
  boost::this_thread::sleep(boost::posix_time::milliseconds(50));
  q.close();
  t.join();
  EXPECT_FALSE(result.load());
}

/**
 * @brief
 * マルチスレッドのプロデューサー／コンシューマーで全要素が届くことを確認する。
 */
TEST(bounded_blocking_queue_test, producer_consumer_multithreaded) {
  constexpr int N = 100;
  Queue q(8);
  std::vector<int> received;
  received.reserve(N);

  boost::thread producer([&] {
    for (int i = 0; i < N; ++i) {
      [[maybe_unused]] auto r = q.push(i, std::stop_token{});
    }
    q.close();
  });

  boost::thread consumer([&] {
    while (true) {
      [[maybe_unused]] auto r = q.pop(std::stop_token{});
      if (!r.has_value())
        break;
      received.push_back(*r);
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(static_cast<int>(received.size()), N);
  for (int i = 0; i < N; ++i) {
    EXPECT_EQ(received[i], i);
  }
}
