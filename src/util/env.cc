
#include "util/env.h"

#include <fstream>

namespace util::env {

Env::Env(const std::string filename, bool overwrite) {
  _read_from_file(filename);
  _set_overwrite(overwrite);
}

std::string Env::operator[](const char *key) {
  return this->operator[](std::string(key));
}

std::string Env::operator[](const std::string key) {
  return query(key);
}

void Env::_read_from_file(const std::string filename) {
  std::ifstream ifs(filename);
  std::string line;
  while (getline(ifs, line)) {
    size_t pos = line.find("=");

    if (pos != std::string::npos) {
      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + 1, line.size() - 1 - pos);

      file[key] = value;
    }
  }
}

void Env::_set_overwrite(bool overwrite) {
  this->overwrite = overwrite;
}

std::string Env::query(const std::string key) {
  auto value = query_cache(key);
  if (value) return value.value();

  value = query_file(key);
  if (overwrite) {
    if (!value) value = query_file(key);
    if (!value) value = query_system(key);
  } else {
    if (!value) value = query_system(key);
    if (!value) value = query_file(key);
  }
  if (!value) value = "";

  cache[key] = value.value();
  return value.value();
}

std::optional<std::string> Env::query_cache(const std::string &key) {
  return query_map(key, cache);
}

std::optional<std::string> Env::query_file(const std::string &key) {
  return query_map(key, file);
}

std::optional<std::string> Env::query_map(const std::string &key, const std::unordered_map<std::string, std::string> map) {
  if (auto it = map.find(key); it == map.end()) return std::nullopt;
  else return it->second;
}

std::optional<std::string> Env::query_system(const std::string &key) {
  char *result = getenv(key.c_str());
  if (result == nullptr) return std::nullopt;
  return result;
}

} // namespace util::env
