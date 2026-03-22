#pragma once

#include <boost/core/noncopyable.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/singleton.hpp>

namespace core::settings {

namespace po = boost::program_options;

/**
 * @brief アプリケーション全体の設定を管理するシングルトンクラス。
 *
 * boost::serialization::singleton を利用してシングルトンパターンを実現する。
 * コピー・代入は禁止されている。
 */
class options : private boost::noncopyable {
private:
  friend class boost::serialization::singleton<options>;
  friend class boost::serialization::detail::singleton_wrapper<options>;
  po::options_description desc_;
  po::variables_map vm_;

  /// @brief 外部からのインスタンス生成を禁止するデフォルトコンストラクタ。
  options();

public:
  //! @brief デストラクタ。
  virtual ~options() = default;

  /**
   * @brief options の唯一のインスタンスへの参照を返す。
   * @return options& シングルトンインスタンスへの参照。
   */
  static options &instance() {
    return boost::serialization::singleton<options>::get_mutable_instance();
  }

  void store(int argc, char *argv[]);
};

} // namespace core::settings
