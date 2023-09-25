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

class json_view;

class json_value;
class json_object;
class json_array;

enum class json_element_type;

class json_invalid;
class json_unsupported_function;

class json {
private:
    const json_element_type element_type;
    const json *jsn = nullptr;

    static json_simplify::json_object *json_simplify_object(const std::string &input, u_int64_t &offset);
    static json_simplify::json_array *json_simplify_array(const std::string &input, u_int64_t &offset);

    static std::string match_braces_pair(const char &brace_open, const char &brace_close, const std::string &input);
    static std::string substring_curly_braces(const std::string &input);
    static std::string substring_squre_braces(const std::string &input);

    static char next_nonspace_char(std::string input);
    static bool is_next_valid_value(const std::string &json, const bool &in_string, std::string &output, u_int64_t &skip);
    static char resolve_escape_characters(const char &c);

    static json_invalid generate_error_message(const std::string &message, const std::string &json, const u_int64_t &err_pos, const u_int64_t &spacing);

    static json *json_simplify(const std::string &input);

protected:
    json(const enum json_element_type json_element_type);
    static json_unsupported_function generate_unsupported_function(std::string fn_name, std::string class_name);

public:

    json(const std::string json);

    virtual const std::string at() const;
    virtual const json_view at(std::string) const;
    virtual const json_view at(u_int64_t) const;
    virtual std::map<std::string, std::string> to_map() const;
    virtual bool is_key_truth() const;

    enum json_element_type get_type() const noexcept;

    virtual ~json() noexcept;
};

class json_view {

private:
    const json *jsn;

public:
    json_view(json *jsn);

    virtual const std::string at() const;
    virtual const json_view at(std::string) const;
    virtual const json_view at(u_int64_t) const;
    virtual std::map<std::string, std::string> to_map() const;
    virtual bool is_key_truth() const;

    enum json_element_type get_type() const noexcept;

    virtual ~json_view() = default;

};

class json_value : public json {
private:
    const std::string value;

protected:
    bool is_key_truth() const noexcept;

public:
    json_value(std::string value) noexcept;

    const std::string at() const;
    const json_view at(std::string) const;
    const json_view at(u_int64_t) const;
    std::map<std::string, std::string> to_map() const noexcept;

    const std::string &get_value() const noexcept;

    ~json_value() noexcept = default;

};

class json_object : public json {
private:
    std::map<std::string, json*> map {};

    std::map<std::string, json*> &get_map() noexcept;

protected:
    bool is_key_truth() const noexcept;

public:
    json_object() noexcept;
    void insert(std::pair<std::string, json*>) noexcept;
    const std::string at() const;
    const json_view at(std::string) const;
    const json_view at(u_int64_t) const;
    std::map<std::string, std::string> to_map() const noexcept;

    const std::map<std::string, json*> &get_map() const noexcept;

    ~json_object() noexcept;
};

class json_array : public json {
private:
    std::vector<json*> list {};

    std::vector<json*> &get_list() noexcept;

protected:
    bool is_key_truth() const noexcept;

public:
    json_array() noexcept;
    void insert(json*) noexcept;
    const std::string at() const;
    const json_view at(std::string) const;
    const json_view at(u_int64_t) const;
    std::map<std::string, std::string> to_map() const noexcept;

    const std::vector<json*> &get_list() const noexcept;

    ~json_array() noexcept;
};

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
