#pragma once

#include <string>
#include <fstream>
#include <optional>
#include <unordered_map>

namespace util::env {

class Env {
 public:
  Env(const std::string filename = ".env", bool overwrite = true);

  std::string operator[](const std::string key);
  std::string operator[](const char *key);
  void _read_from_file(const std::string filename);
  void _set_overwrite(bool overwrite);

 private:
  bool overwrite;
  std::unordered_map<std::string, std::string> cache, file;

  std::string query(const std::string key);

  std::optional<std::string> query_cache(const std::string &key);

  std::optional<std::string> query_file(const std::string &key);

  std::optional<std::string> query_map(const std::string &key, const std::unordered_map<std::string, std::string> map);

  std::optional<std::string> query_system(const std::string &key);
};

} // namespace util::env
