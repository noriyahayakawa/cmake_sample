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
  Queue queue(4);
  EXPECT_EQ(queue.size(), 0U);
  EXPECT_FALSE(queue.closed());
}

/**
 * @brief push した値が pop で取り出せることを確認する。
 */
TEST(bounded_blocking_queue_test, push_and_pop_preserve_value) {
  std::stop_source stop_source;
  std::stop_token token = stop_source.get_token();
  Queue queue(4);
  constexpr int kTestValue = 42;
  [[maybe_unused]] auto push_result = queue.push(kTestValue, token);
  ASSERT_TRUE(push_result.has_value());
  [[maybe_unused]] auto pop_result = queue.pop(token);
  ASSERT_TRUE(pop_result.has_value());
  EXPECT_EQ(*pop_result, kTestValue);
}

/**
 * @brief 複数の要素が FIFO 順で取り出されることを確認する。
 */
TEST(bounded_blocking_queue_test, fifo_order_is_maintained) {
  Queue queue(4);
  [[maybe_unused]] auto push_result1 = queue.push(1, std::stop_token{});
  [[maybe_unused]] auto push_result2 = queue.push(2, std::stop_token{});
  [[maybe_unused]] auto push_result3 = queue.push(3, std::stop_token{});
  [[maybe_unused]] auto pop_result1 = queue.pop(std::stop_token{});
  [[maybe_unused]] auto pop_result2 = queue.pop(std::stop_token{});
  [[maybe_unused]] auto pop_result3 = queue.pop(std::stop_token{});
  EXPECT_EQ(*pop_result1, 1);
  EXPECT_EQ(*pop_result2, 2);
  EXPECT_EQ(*pop_result3, 3);
}

/**
 * @brief size() が push/pop に追従することを確認する。
 */
TEST(bounded_blocking_queue_test, size_reflects_push_and_pop) {
  Queue queue(4);
  EXPECT_EQ(queue.size(), 0U);
  [[maybe_unused]] auto push_result2 = queue.push(1, std::stop_token{});
  EXPECT_EQ(queue.size(), 1U);
  [[maybe_unused]] auto push_result3 = queue.push(2, std::stop_token{});
  EXPECT_EQ(queue.size(), 2U);
  [[maybe_unused]] auto pop_result = queue.pop(std::stop_token{});
  EXPECT_EQ(queue.size(), 1U);
}

/**
 * @brief close 後は closed() が true を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, closed_returns_true_after_close) {
  Queue queue(4);
  EXPECT_FALSE(queue.closed());
  queue.close();
  EXPECT_TRUE(queue.closed());
}

/**
 * @brief close 後の push が false を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, push_returns_false_after_close) {
  Queue queue(4);
  queue.close();
  auto push_result = queue.push(1, std::stop_token{});
  EXPECT_FALSE(push_result.has_value());
}

/**
 * @brief close 後に空のキューへ pop すると false を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, pop_returns_false_when_closed_and_empty) {
  Queue queue(4);
  queue.close();
  auto pop_result = queue.pop(std::stop_token{});
  EXPECT_FALSE(pop_result.has_value());
}

/**
 * @brief close 後もキューに残っている要素はすべて pop できることを確認する。
 */
TEST(bounded_blocking_queue_test,
     pop_drains_items_then_returns_false_after_close) {
  Queue queue(3);
  constexpr int kPushValue1 = 10;
  constexpr int kPushValue2 = 20;
  [[maybe_unused]] auto push_result1 =
      queue.push(kPushValue1, std::stop_token{});
  [[maybe_unused]] auto push_result2 =
      queue.push(kPushValue2, std::stop_token{});
  queue.close();
  [[maybe_unused]] auto pop_result1 = queue.pop(std::stop_token{});
  EXPECT_TRUE(pop_result1.has_value());
  EXPECT_EQ(*pop_result1, 10);
  [[maybe_unused]] auto pop_result2 = queue.pop(std::stop_token{});
  EXPECT_TRUE(pop_result2.has_value());
  EXPECT_EQ(*pop_result2, 20);
  [[maybe_unused]] auto pop_result3 = queue.pop(std::stop_token{});
  EXPECT_FALSE(pop_result3.has_value());
}

/**
 * @brief push_for は要素数が capacity 未満であれば true を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, push_for_succeeds_when_space_available) {
  Queue queue(4);
  constexpr int kPushValue = 7;
  constexpr int kPushTimeoutMs = 100;
  [[maybe_unused]] auto push_result = queue.push_for(
      kPushValue, std::chrono::milliseconds(kPushTimeoutMs), std::stop_token{});
  EXPECT_TRUE(push_result.has_value());
  EXPECT_EQ(queue.size(), 1U);
}

/**
 * @brief push_for は満杯のキューへのタイムアウト時に false
 * を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, push_for_returns_false_on_timeout_when_full) {
  Queue queue(1);
  [[maybe_unused]] auto push_result = queue.push(0, std::stop_token{});
  constexpr int kPushValue = 99;
  constexpr int kTimeoutMs = 50;
  int value = kPushValue;
  [[maybe_unused]] auto push_for_result = queue.push_for(
      value, std::chrono::milliseconds(kTimeoutMs), std::stop_token{});
  EXPECT_FALSE(push_for_result.has_value());
}

/**
 * @brief pop_for は要素がある場合に true を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, pop_for_succeeds_when_item_available) {
  Queue queue(4);
  constexpr int kPushValue = 7;
  constexpr int kTimeoutMs = 100;
  [[maybe_unused]] auto push_result = queue.push(kPushValue, std::stop_token{});
  [[maybe_unused]] auto pop_for_result =
      queue.pop_for(std::chrono::milliseconds(kTimeoutMs), std::stop_token{});
  EXPECT_TRUE(pop_for_result.has_value());
  EXPECT_EQ(*pop_for_result, kPushValue);
}

/**
 * @brief pop_for は空のキューへのタイムアウト時に false を返すことを確認する。
 */
TEST(bounded_blocking_queue_test, pop_for_returns_false_on_timeout_when_empty) {
  Queue queue(4);
  constexpr int kTimeoutMs = 50;
  [[maybe_unused]] auto pop_for_result =
      queue.pop_for(std::chrono::milliseconds(kTimeoutMs), std::stop_token{});
  EXPECT_FALSE(pop_for_result.has_value());
}

/**
 * @brief close により、pop でブロック中のスレッドが起床することを確認する。
 */
TEST(bounded_blocking_queue_test, close_unblocks_waiting_pop) {
  Queue queue(4);
  std::atomic<bool> pop_success{true};
  boost::thread pop_thread([&] -> void {
    [[maybe_unused]] auto pop_result = queue.pop(std::stop_token{});
    pop_success.store(pop_result.has_value());
  });
  constexpr int kSleepMs = 50;
  boost::this_thread::sleep(boost::posix_time::milliseconds(kSleepMs));
  queue.close();
  pop_thread.join();
  EXPECT_FALSE(pop_success.load());
}

/**
 * @brief close により、満杯キューへの push
 * でブロック中のスレッドが起床することを確認する。
 */
TEST(bounded_blocking_queue_test, close_unblocks_waiting_push) {
  Queue queue(1);
  [[maybe_unused]] auto push_result = queue.push(0, std::stop_token{});
  std::atomic<bool> push_success{true};
  constexpr int kPushValue = 99;
  boost::thread push_thread([&] -> void {
    [[maybe_unused]] auto push_thread_result =
        queue.push(kPushValue, std::stop_token{});
    push_success.store(push_thread_result.has_value());
  });
  constexpr int kSleepMs = 50;
  boost::this_thread::sleep(boost::posix_time::milliseconds(kSleepMs));
  queue.close();
  push_thread.join();
  EXPECT_FALSE(push_success.load());
}

/**
 * @brief
 * マルチスレッドのプロデューサー／コンシューマーで全要素が届くことを確認する。
 */
TEST(bounded_blocking_queue_test, producer_consumer_multithreaded) {
  constexpr int kNumItems = 100;
  constexpr int kQueueCapacity = 8;
  Queue queue(kQueueCapacity);
  std::vector<int> received_items;
  received_items.reserve(kNumItems);

  boost::thread producer_thread([&] -> void {
    for (int value = 0; value < kNumItems; ++value) {
      [[maybe_unused]] auto push_result = queue.push(value, std::stop_token{});
    }
    queue.close();
  });

  boost::thread consumer_thread([&] -> void {
    while (true) {
      [[maybe_unused]] auto pop_result = queue.pop(std::stop_token{});
      if (!pop_result.has_value()) {
        break;
      }
      received_items.push_back(*pop_result);
    }
  });

  producer_thread.join();
  consumer_thread.join();

  ASSERT_EQ(static_cast<int>(received_items.size()), kNumItems);
  for (int value = 0; value < kNumItems; ++value) {
    EXPECT_EQ(received_items[value], value);
  }
}
