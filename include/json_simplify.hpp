#ifndef JSON_SIMPLIFY_HPP
#define JSON_SIMPLIFY_HPP

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace json_simplify {

std::map<std::string, std::string> json_simplify(const std::string &input);

} // namespace json_simplify

#endif // JSON_SIMPLIFY_HPP
