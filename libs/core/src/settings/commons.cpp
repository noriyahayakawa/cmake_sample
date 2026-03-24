#include "settings/commons.hpp"

namespace core::settings {} // namespace core::settings

namespace boost::json {

::core::settings::commons
tag_invoke(boost::json::value_to_tag<::core::settings::commons>,
           const value &jv) {
  const object &obj = jv.as_object();
  ::core::settings::commons commons;
  commons.appName = obj.if_contains("appName")
                        ? value_to<std::string>(obj.at("appName"))
                        : "";
  commons.version = obj.if_contains("version")
                        ? value_to<std::string>(obj.at("version"))
                        : "";
  return commons;
}

void tag_invoke(value_from_tag, value &jv,
                const ::core::settings::commons &commons) {
  object obj;
  if (!commons.appName.empty()) {
    obj.emplace("appName", commons.appName);
  }
  if (!commons.version.empty()) {
    obj.emplace("version", commons.version);
  }
  jv = std::move(obj);
}

} // namespace boost::json
