// A small JSON value, for saves and settings. Objects keep their keys in the
// order they were set, so what is written reads the same each time.
#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace w2 {

class Json {
 public:
  enum Type { Null, Bool, Number, String, Array, Object };

  Json() = default;
  Json(std::nullptr_t) {}
  Json(bool b) : type_(Bool), b_(b) {}
  Json(int n) : type_(Number), n_(n) {}
  Json(long n) : type_(Number), n_((double)n) {}
  Json(double n) : type_(Number), n_(n) {}
  Json(const char* s) : type_(String), s_(s) {}
  Json(std::string s) : type_(String), s_(std::move(s)) {}

  static Json array() { Json j; j.type_ = Array; return j; }
  static Json object() { Json j; j.type_ = Object; return j; }

  Type type() const { return type_; }
  bool isNull() const { return type_ == Null; }
  bool isNumber() const { return type_ == Number; }
  bool isString() const { return type_ == String; }
  bool isArray() const { return type_ == Array; }
  bool isObject() const { return type_ == Object; }

  bool boolean(bool dflt = false) const { return type_ == Bool ? b_ : (type_ == Number ? n_ != 0 : dflt); }
  double number(double dflt = 0) const { return type_ == Number ? n_ : dflt; }
  long integer(long dflt = 0) const { return type_ == Number ? (long)n_ : dflt; }
  const std::string& str() const { return s_; }
  std::string str(const std::string& dflt) const { return type_ == String ? s_ : dflt; }

  // arrays
  size_t size() const { return type_ == Array ? a_.size() : type_ == Object ? o_.size() : 0; }
  const Json& operator[](size_t i) const { return i < a_.size() ? a_[i] : none(); }
  const Json& operator[](int i) const { return i >= 0 ? (*this)[(size_t)i] : none(); }
  Json& push(Json v) { type_ = Array; a_.push_back(std::move(v)); return a_.back(); }
  const std::vector<Json>& items() const { return a_; }

  // objects
  bool has(const std::string& k) const;
  const Json& operator[](const std::string& k) const;
  const Json& operator[](const char* k) const { return (*this)[std::string(k)]; }
  /** Set a key (the value is dropped if null, as JSON.stringify drops undefined). */
  Json& set(const std::string& k, Json v);
  const std::vector<std::pair<std::string, Json>>& entries() const { return o_; }

  std::string dump() const;
  /** Parse, or throw std::runtime_error. */
  static Json parse(const std::string& text);

 private:
  static const Json& none();
  void write(std::string& out) const;

  Type type_ = Null;
  bool b_ = false;
  double n_ = 0;
  std::string s_;
  std::vector<Json> a_;
  std::vector<std::pair<std::string, Json>> o_;
};

}  // namespace w2
