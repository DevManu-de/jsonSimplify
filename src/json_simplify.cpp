#include "json_simplify.hpp"

std::map<std::string, std::string> json_simplify_object(const std::string &input, u_int64_t &offset);
std::map<std::string, std::string> json_simplify_array(const std::string &input, u_int64_t &offset);

std::string match_braces_pair(const char &brace_open, const char &brace_close, const std::string &input);
std::string substring_curly_braces(const std::string &input);
std::string substring_squre_braces(const std::string &input);

std::string match_braces_pair(const char &brace_open, const char &brace_close, const std::string &input) {
    int level {0};
    bool is_escaped = false;
    bool collect = false;
    int length = 0;
    for (const char &c : input) {
        if (c == '\\') {
            is_escaped = !is_escaped;
        } else if (c == brace_open && !is_escaped) {
            collect = true;
            ++level;
        } else if (c == brace_close && !is_escaped) {
            --level;
        }
        if (collect) {
            ++length;
            if (level == 0) {
                break;
            }
        }

    }

    return input.substr(input.find(brace_open), length);
}

std::string substring_curly_braces(const std::string &input) {

    return match_braces_pair('{', '}', input);
}

std::string substring_squre_braces(const std::string &input) {

    return match_braces_pair('[', ']', input);
}

std::map<std::string, std::string> json_simplify_array(const std::string &input, u_int64_t &offset) {
    std::string json = substring_squre_braces(input).substr(1);

    std::map<std::string, std::string> map {};
    int64_t index = 0;
    bool is_escaped = false;
    bool in_string = false;
    std::string value_buffer {};

    for (u_int64_t i = 0; i < json.length(); ++i) {
        const char c = json[i];
        if (isspace(c) && !in_string) {
            continue;
        } else if (c == '"' && !in_string && !is_escaped) {
            in_string = true;
        } else if (c == '"' && in_string && !is_escaped) {
            in_string = false;
        } else if (c == '\\') {
            is_escaped = true;
        } else if (c == ',' && !is_escaped && !in_string && !value_buffer.empty()) {
            map.insert({std::to_string(index), value_buffer});
            value_buffer.clear();
            ++index;
        } else if (in_string) {
            value_buffer.append(1, c);
        } else if (c == '{') {
            for (const auto &[k, v] : json_simplify_object(input.substr(i), i)) {
                map.insert({std::to_string(index) + "." + k, v});
            }
            ++index;
            //i += substring_curly_braces(input.substr(i)).length();
        } else if (c == '[') {
            for (const auto &[k, v] : json_simplify_array(input.substr(i), i)) {
                map.insert({std::to_string(index) + "." + k, v});
            }
            ++index;
            //i += substring_squre_braces(input.substr(i)).length();
        } else if (c != '}' && c != ']') {
            value_buffer.append(1, c);
        } else {
            throw std::invalid_argument(std::string("Cannot parse: ") + c);
        }

    }

    if (!value_buffer.empty()) {
        map.insert({std::to_string(index), value_buffer});
        value_buffer.clear();
    }

    return map;

}

std::map<std::string, std::string> json_simplify_object(const std::string &input, u_int64_t &offset) {

    std::string json = substring_curly_braces(input).substr(1);
    offset += json.length();

    std::map<std::string, std::string> map {};
    bool is_escaped = false;
    bool in_string = false;
    bool is_value = false;
    std::string key_buffer {};
    std::string value_buffer {};

    for (u_int64_t i = 0; i < json.length(); ++i) {
        const char c = json[i];
        if (isspace(c) && !in_string) {
            continue;
        } else if (c == '"' && !in_string && !is_escaped) {
            in_string = true;
        } else if (c == '"' && in_string && !is_escaped) {
            in_string = false;
        } else if (c == ':' && !in_string) {
            is_value = true;
        } else if (c == '\\') {
            is_escaped = true;
        } else if (c == ',' && !is_escaped && !in_string) {
            map.insert({key_buffer, value_buffer});
            key_buffer.clear();
            value_buffer.clear();
            is_value = false;
        } else if (is_value) {
            if (c == '{') {
                for (const auto &[k, v] : json_simplify_object(input.substr(i), i)) {
                    map.insert({key_buffer + "." + k, v});
                }
                key_buffer.clear();
                //i += substring_curly_braces(input.substr(i)).length();
                is_value = false;
            } else if (c == '[') {
                for (const auto &[k, v] : json_simplify_array(input.substr(i), i)) {
                    map.insert({key_buffer + "." + k, v});
                }
                key_buffer.clear();
                //i += substring_squre_braces(input.substr(i)).length();
                is_value = false;
            } else if (c != '}' && c != ']') {
                value_buffer.append(1, c);
            }
        } else if (in_string) {
            key_buffer.append(1, c);
        } else {
            throw std::invalid_argument(std::string("Cannot parse: ") + c);
        }

    }

    if (!key_buffer.empty() && !value_buffer.empty()) {
        map.insert({key_buffer, value_buffer});
        key_buffer.clear();
        value_buffer.clear();
    }

    return map;
}

std::map<std::string, std::string> json_simplify::json_simplify(const std::string &input) {

    bool is_escaped = false;
    std::vector<char> stack {};
    for (const char &c : input) {
        if (c == '\\') {
            is_escaped = !is_escaped;
        } else if ((c == '{' || c == '[') && !is_escaped) {
            stack.push_back(c);
        } else if ((c == '}' || c == ']') && !is_escaped) {
            if (stack.size() <= 0) {
                throw std::invalid_argument("Cannot parse");
            } else if (c == '}' && stack.back() == '{') {
                stack.pop_back();
            } else if (c == '}' && stack.back() == '[') {
                throw std::invalid_argument("Cannot parse");
            } else if (c == ']' && stack.back() == '[') {
                stack.pop_back();
            } else if (c == ']' && stack.back() == '{') {
                throw std::invalid_argument("Cannot parse");
            }
        }
    }
    if (!stack.empty()) {
        throw std::invalid_argument("Cannot parse");
    }

    for (const char &c : input) {
        if (isspace(c)) {
            continue;
        } else if (c == '{') {
            u_int64_t tmp = 0;
            return json_simplify_object(input, tmp);
        } else if (c == '[') {
            u_int64_t tmp = 0;
            return json_simplify_array(input, tmp);
        }
    }
    throw std::invalid_argument("Cannot parse");
}
