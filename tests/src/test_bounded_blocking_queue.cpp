/**
 * @file test_bounded_blocking_queue.cpp
 * @brief bounded_blocking_queue の単体テスト。
 */

#include "utility/bounded_blocking_queue.hpp"

#include <atomic>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <gtest/gtest.h>
#include <vector>

using Queue = core::utility::bounded_blocking_queue<int>;

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
  Queue q(4);
  ASSERT_TRUE(q.push(42));
  int out = 0;
  ASSERT_TRUE(q.pop(out));
  EXPECT_EQ(out, 42);
}

/**
 * @brief 複数の要素が FIFO 順で取り出されることを確認する。
 */
TEST(bounded_blocking_queue_test, fifo_order_is_maintained) {
  Queue q(4);
  q.push(1);
  q.push(2);
  q.push(3);
  int a = 0, b = 0, c = 0;
  q.pop(a);
  q.pop(b);
  q.pop(c);
  EXPECT_EQ(a, 1);
  EXPECT_EQ(b, 2);
  EXPECT_EQ(c, 3);
}

/**
 * @brief size() が push/pop に追従することを確認する。
 */
TEST(bounded_blocking_queue_test, size_reflects_push_and_pop) {
  Queue q(4);
  EXPECT_EQ(q.size(), 0u);
  q.push(1);
  EXPECT_EQ(q.size(), 1u);
  q.push(2);
  EXPECT_EQ(q.size(), 2u);
  int v;
  q.pop(v);
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
  EXPECT_FALSE(q.push(1));
}

/**
 * @brief close 後に空のキューへ pop すると false を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, pop_returns_false_when_closed_and_empty) {
  Queue q(4);
  q.close();
  int v = 0;
  EXPECT_FALSE(q.pop(v));
}

/**
 * @brief close 後もキューに残っている要素はすべて pop できることを確認する。
 */
TEST(bounded_blocking_queue_test,
     pop_drains_items_then_returns_false_after_close) {
  Queue q(3);
  q.push(10);
  q.push(20);
  q.close();
  int v = 0;
  EXPECT_TRUE(q.pop(v));
  EXPECT_EQ(v, 10);
  EXPECT_TRUE(q.pop(v));
  EXPECT_EQ(v, 20);
  EXPECT_FALSE(q.pop(v));
}

/**
 * @brief push_for は要素数が capacity 未満であれば true を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, push_for_succeeds_when_space_available) {
  Queue q(4);
  const bool result = q.push_for(7, boost::posix_time::milliseconds(100));
  EXPECT_TRUE(result);
  EXPECT_EQ(q.size(), 1u);
}

/**
 * @brief push_for は満杯のキューへのタイムアウト時に false
 * を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, push_for_returns_false_on_timeout_when_full) {
  Queue q(1);
  q.push(0);
  int dummy = 99;
  const bool result = q.push_for(dummy, boost::posix_time::milliseconds(50));
  EXPECT_FALSE(result);
}

/**
 * @brief pop_for は要素がある場合に true を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, pop_for_succeeds_when_item_available) {
  Queue q(4);
  q.push(7);
  int out = 0;
  const bool result = q.pop_for(out, boost::posix_time::milliseconds(100));
  EXPECT_TRUE(result);
  EXPECT_EQ(out, 7);
}

/**
 * @brief pop_for は空のキューへのタイムアウト時に false を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, pop_for_returns_false_on_timeout_when_empty) {
  Queue q(4);
  int out = 0;
  const bool result = q.pop_for(out, boost::posix_time::milliseconds(50));
  EXPECT_FALSE(result);
}

/**
 * @brief close により、pop でブロック中のスレッドが起床することを確認する。
 */
TEST(bounded_blocking_queue_test, close_unblocks_waiting_pop) {
  Queue q(4);
  std::atomic<bool> result{true};
  boost::thread t([&] {
    int v;
    result.store(q.pop(v));
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
  q.push(0);
  std::atomic<bool> result{true};
  boost::thread t([&] { result.store(q.push(99)); });
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
      q.push(i);
    }
    q.close();
  });

  boost::thread consumer([&] {
    int v;
    while (q.pop(v)) {
      received.push_back(v);
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(static_cast<int>(received.size()), N);
  for (int i = 0; i < N; ++i) {
    EXPECT_EQ(received[i], i);
  }
}
