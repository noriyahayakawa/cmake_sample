#pragma once

/**
 * @file message_register.hpp
 * @brief TCP メッセージの登録クラスを宣言する。
 */

 #include "comm/messages/message_factory.hpp"

namespace comm::messages {

/**
 * @brief TCP メッセージ型を message_factory に自動登録するヘルパー構造体。
 *
 * @tparam T 登録するメッセージ型。`message` の派生クラスであり、
 *           引数なしで呼び出し可能な静的メンバ関数 `type()` が
 *           `std::string` に変換可能な値を返す必要がある。
 *
 * @note 使用側でこの構造体をスタティック変数として定義することで、
 *       プログラム起動時に自動的にファクトリーへ登録される。
 */
    template<class T, class = std::enable_if_t<
        std::is_base_of_v<message, T> &&
        std::is_invocable_v<decltype(&T::type)> &&
        std::is_convertible_v<std::invoke_result_t<decltype(&T::type)>, std::string>
    >>
    struct message_register {
        /**
         * @brief T 型のインスタンスを生成して返すクリエーター関数。
         * @return 新しく生成した T 型オブジェクトの shared_ptr。
         */
        static std::shared_ptr<message> creator() {
            return std::make_shared<T>();
        }

        /**
         * @brief コンストラクタ。T 型のクリエーターを message_factory に登録する。
         */
        explicit message_register() {
            message_factory::instance().register_message(T::type(), &message_register::creator);
        }

        /** @brief デストラクタ。 */
        virtual ~message_register() = default;
    };
}