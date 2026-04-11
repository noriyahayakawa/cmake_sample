#pragma once

/**
 * @file message_header.hpp
 * @brief TCP メッセージのヘッダーファイルを宣言する。
 */

#include <array>
#include <cstdint>
#include <string>

namespace comm::messages {

/**
 * @brief TCP メッセージのヘッダー情報を保持するクラス。
 *
 * メッセージ型を表す文字列、ペイロード長、および固定サイズのペイロードバッファを管理する。
 * ペイロードの最大サイズは `max_payload_size_` で定義される。
 */
class message_header {
  static constexpr std::size_t max_data_size_ =
      32; ///< データの最大サイズ（バイト単位）。
  static constexpr std::size_t length_offset_ = 0; ///< data_ 内の message_length_ の開始オフセット。
  static constexpr std::size_t version_offset_ =
      length_offset_ + sizeof(uint32_t); ///< data_ 内の message_version_ の開始オフセット。
  static constexpr std::size_t type_offset_ =
      version_offset_ + sizeof(uint16_t); ///< data_ 内の message_type_ の開始オフセット。
  static constexpr std::size_t type_size_ =
      max_data_size_ - type_offset_; ///< data_ 内の message_type_ に割り当てるバイト数。
  std::string message_type_; ///< メッセージの型を表す文字列（最大 `type_size_ - 1` バイト）。
  uint32_t message_length_ =
      0; ///< メッセージのペイロードの長さ（バイト単位）。
  uint16_t message_version_ = 1; ///< メッセージのバージョン番号。
  std::array<std::byte, max_data_size_>
      data_; ///< エンコード済みヘッダーデータ。
public:
  /** @brief デフォルトコンストラクタ。 */
  message_header() = default;

  /** @brief 仮想デストラクタ。 */
  virtual ~message_header() = default;

  /**
   * @brief メッセージ型文字列を取得する。
   * @return メッセージ型を表す文字列。
   */
  std::string message_type() const { return message_type_; }

  /**
   * @brief メッセージ型文字列を設定する。
   * @param t メッセージ型を表す文字列（`type_size_ - 1` バイト以内）。
   * @throws std::runtime_error 文字列幅が `type_size_ - 1` バイトを超過する場合。
   */
  void message_type(const std::string &t);

  /**
   * @brief ペイロード長を取得する。
   * @return ペイロードの長さ（バイト単位）。
   */
  uint32_t message_length() const { return message_length_; }

  /**
   * @brief ペイロード長を設定する。
   * @param l ペイロードの長さ（バイト単位）。
   */
  void message_length(uint32_t l) { message_length_ = l; }

  /**
   * @brief メッセージのバージョン番号を取得する。
   * @return バージョン番号。
   */
  uint16_t message_version() const { return message_version_; }

  /**
   * @brief メッセージのバージョン番号を設定する。
   * @param v バージョン番号。
   */
  void message_version(uint16_t v) { message_version_ = v; }

  /**
   * @brief エンコード済みヘッダーデータへの const 参照を返す。
   * @return ヘッダーデータの const 参照。
   */
  const std::array<std::byte, max_data_size_> &data() const {
    return data_;
  }

  /**
   * @brief エンコード済みヘッダーデータへの参照を返す。
   * @return ヘッダーデータの参照。
   */
  std::array<std::byte, max_data_size_> &data() { return data_; }

  /**
   * @brief ヘッダーのバイト長（データバッファサイズ）を返す。
   * @return データバッファのサイズ（バイト単位）。
   */
  uint32_t header_length() const { return data_.size(); }

  /**
   * @brief ヘッダー情報をバイト列にエンコードして data_ に書き込む。
   */
  void encode_header();

  /**
   * @brief data_ からヘッダー情報をデコードして各フィールドに展開する。
   */
  void decode_header();
};

} // namespace comm::messages