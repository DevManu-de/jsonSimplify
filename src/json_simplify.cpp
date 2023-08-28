#include "json_simplify.hpp"

std::map<std::string, std::string> json_simplify_object(const std::string &input);
std::map<std::string, std::string> json_simplify_array(const std::string &input);

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

std::map<std::string, std::string> json_simplify_array(const std::string &input) {
    std::string json = substring_squre_braces(input).substr(1);

    std::map<std::string, std::string> map {};
    int64_t index = 0;
    int64_t length = 0;
    bool is_escaped = false;
    bool in_string = false;
    bool is_value = false;
    std::string value_buffer {};

    for (u_int64_t i = 0; i < json.length(); ++i) {
        const char c = json[i];
        ++length;
        if (isspace(c) && !in_string) {
            continue;
        }
        if (c == '"' && !in_string && !is_escaped) {
            in_string = true;
            continue;
        }
        if (c == '"' && in_string && !is_escaped) {
            in_string = false;
            continue;
        }
        if (c == '\\') {
            is_escaped = true;
            continue;
        }
        if (c == ',' && !is_escaped && !in_string && !value_buffer.empty()) {
            map.insert({std::to_string(index), value_buffer});
            value_buffer.clear();
            ++index;
            continue;
        }
        if (in_string) {
            value_buffer.append(1, c);
            continue;
        }
        if (c == '{') {
            for (const auto &[k, v] : json_simplify_object(input.substr(length-1))) {
                map.insert({std::to_string(index) + "." + k, v});
            }
            ++index;
            int tmp = substring_curly_braces(input.substr(length-1)).length();
            i += tmp;
            length += tmp;
        } else if (c == '[') {
            for (const auto &[k, v] : json_simplify_array(input.substr(length-1))) {
                map.insert({std::to_string(index) + "." + k, v});
            }
            ++index;
            int tmp = substring_squre_braces(input.substr(length-1)).length();
            i += tmp;
            length += tmp;
        } else if (c != '}' && c != ']') {
            value_buffer.append(1, c);
        }

    }

    if (!value_buffer.empty()) {
        map.insert({std::to_string(index), value_buffer});
        value_buffer.clear();
    }

    return map;

}

std::map<std::string, std::string> json_simplify_object(const std::string &input) {

    std::string json = substring_curly_braces(input);

    std::map<std::string, std::string> map {};
    int64_t length = 0;
    bool is_escaped = false;
    bool in_string = false;
    bool is_value = false;
    std::string key_buffer {};
    std::string value_buffer {};

    for (u_int64_t i = 0; i < json.length(); ++i) {
        const char c = json[i];
        ++length;
        if (isspace(c) && !in_string) {
            continue;
        }
        if (c == '"' && !in_string && !is_escaped) {
            in_string = true;
            continue;
        }
        if (c == '"' && in_string && !is_escaped) {
            in_string = false;
            continue;
        }
        if (c == ':' && !in_string) {
            is_value = true;
            continue;
        }
        if (c == '\\') {
            is_escaped = true;
            continue;
        }
        if (c == ',' && !is_escaped && !in_string) {
            map.insert({key_buffer, value_buffer});
            key_buffer.clear();
            value_buffer.clear();
            is_value = false;
            continue;
        }

        if (is_value) {
            if (c == '{') {
                for (const auto &[k, v] : json_simplify_object(input.substr(length-1))) {
                    map.insert({key_buffer + "." + k, v});
                }
                key_buffer.clear();
                int tmp = substring_curly_braces(input.substr(length-1)).length();
                is_value = false;
                i += tmp;
                length += tmp;
            } else if (c == '[') {
                for (const auto &[k, v] : json_simplify_array(input.substr(length-1))) {
                    map.insert({key_buffer + "." + k, v});
                }
                key_buffer.clear();
                int tmp = substring_squre_braces(input.substr(length-1)).length();
                is_value = false;
                i += tmp;
                length += tmp;
            } else if (c != '}' && c != ']') {
                value_buffer.append(1, c);
            }
        } else if (in_string) {
            key_buffer.append(1, c);
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

    for (const char &c : input) {
        if (isspace(c)) {
            continue;
        } else if (c == '{') {
            return json_simplify_object(input);
        } else if (c == '[') {
            return json_simplify_array(input);
        }
    }
    return std::map<std::string, std::string>();
}
