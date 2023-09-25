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

#include "json.hpp"
#include "json_array.hpp"
#include "json_element.hpp"
#include "json_object.hpp"
#include "json_value.hpp"

namespace json_simplify {

enum class json_element_type;

class json_invalid;
class json_unsupported_function;

json_object *json_simplify_object(const std::string &input, u_int64_t &offset);
json_array *json_simplify_array(const std::string &input, u_int64_t &offset);

std::string match_braces_pair(const char &brace_open, const char &brace_close, const std::string &input);
std::string substring_curly_braces(const std::string &input);
std::string substring_squre_braces(const std::string &input);

char next_nonspace_char(std::string input);
bool is_next_valid_value(const std::string &json, const bool &in_string, std::string &output, u_int64_t &skip);
char resolve_escape_characters(const char &c);

json_invalid generate_error_message(const std::string &message, const std::string &json, const u_int64_t &err_pos, const u_int64_t &spacing);

json_element *json_simplify(const std::string &input);

json_unsupported_function generate_unsupported_function(std::string fn_name, std::string class_name);


enum class json_element_type {
    JSON_NONE,
    JSON_VALUE,
    JSON_OBJECT,
    JSON_ARRAY
};

class json_invalid : public std::runtime_error {
public:
    const std::string msg;
    const std::string snippet;
    const u_int64_t pos;

    json_invalid(std::string msg, std::string snippet, u_int64_t pos, std::string what);
    std::string format() const;
    virtual ~json_invalid() = default;
};

class json_unsupported_function : public std::runtime_error {
public:
    json_unsupported_function(std::string msg);
    virtual ~json_unsupported_function() = default;
};

} // namespace json_simplify

#endif // JSON_SIMPLIFY_HPP
