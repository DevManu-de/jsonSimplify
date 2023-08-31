#ifndef JSON_SIMPLIFY_HPP
#define JSON_SIMPLIFY_HPP

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace json_simplify {

std::map<std::string, std::string> json_simplify(const std::string &input);

class invalid_json : public std::runtime_error {
public:
    const std::string msg;
    const std::string snippet;
    const u_int64_t pos;

    invalid_json(std::string msg, std::string snippet, u_int64_t pos, std::string what);
    std::string format() const;
    ~invalid_json() = default;
};

} // namespace json_simplify

#endif // JSON_SIMPLIFY_HPP
