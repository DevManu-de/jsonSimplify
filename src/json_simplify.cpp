#include "json_simplify.hpp"

std::map<std::string, std::string> json_simplify_object(const std::string &input, u_int64_t &offset);
std::map<std::string, std::string> json_simplify_array(const std::string &input, u_int64_t &offset);

std::string match_braces_pair(const char &brace_open, const char &brace_close, const std::string &input);
std::string substring_curly_braces(const std::string &input);
std::string substring_squre_braces(const std::string &input);

char next_nonspace_char(std::string input);
bool is_next_valid_value(const std::string &json, const bool &in_string, std::string &output, u_int64_t &skip);
char resolve_escape_characters(const char &c);

std::string match_braces_pair(const char &brace_open, const char &brace_close, const std::string &input) {

    int level {0};
    bool collect = false;
    int length = 0;

    bool is_escaped = false;
    bool in_string = false;
    for (const char &c : input) {
        if (c == '\\' && !is_escaped) {
            is_escaped = true;
            continue;
        } else if (c == '"' && !in_string && !is_escaped) {
            in_string = true;
        } else if (c == '"' && in_string && !is_escaped) {
            in_string = false;
        } else if (c == brace_open && !in_string) {
            collect = true;
            ++level;
        } else if (c == brace_close && !in_string) {
            --level;
        }
        if (collect) {
            ++length;
            is_escaped = false;
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

char next_nonspace_char(std::string input) {
    for (const char &c : input) {
        if (!isspace(c)) {
            return c;
        }
    }
    throw std::invalid_argument("Cannot parse: ");
}

bool is_next_valid_value(const std::string &json, const bool &in_string, std::string &output, u_int64_t &skip) {

    std::string buffer {};

    static const std::string illegal_characters {",}]"};

    bool is_num = true;
    bool is_escaped = false;
    for (const char &c : json) {
        if (c == '\\' && !is_escaped) {
            is_escaped = true;
        } else if ((c == '"' && !is_escaped) || illegal_characters.find(c) != illegal_characters.npos && !in_string) {
            break;
        } else {
            is_num = is_num && (isdigit(c) || c == '.');
            if (is_escaped) {
                buffer.append(1, resolve_escape_characters(c));
                skip += 2;
            } else {
                buffer.append(1, c);
                ++skip;
            }
            //buffer.append(1, is_escaped ? resolve_escape_characters(c) : c);
            is_escaped = false;
        }
    }

    if (is_num || in_string) {
        output = buffer;
        return true;
    }

    static const std::string allowed_keywords[] = {"null", "true", "false"};

    if (std::find(std::begin(allowed_keywords), std::end(allowed_keywords), buffer) != std::end(allowed_keywords)) {
        output = buffer;
        return true;
    }

    output = "";
    return false;
}

char resolve_escape_characters(const char &c) {

    switch (c) {
    case '\\':
        return '\\';
    case 'b':
        return '\b';
    case 'f':
        return '\f';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    case 'u':
    default:
        throw std::domain_error(std::string("Currently \\") + c + " is not supported");
    }
}

std::map<std::string, std::string> json_simplify_array(const std::string &input, u_int64_t &offset) {
    std::string json = substring_squre_braces(input);
    offset += json.length() - 1;

    std::map<std::string, std::string> map {};
    int64_t index = 0;
    bool is_escaped = false;
    bool in_string = false;
    std::string value_buffer {};

    for (u_int64_t i = 1; i < json.length() - 1; ++i) {
        const char c = json[i];
        if (isspace(c) && !in_string) {
            continue;
        } else if (c == '"' && !in_string && !is_escaped) {
            in_string = true;
        } else if (c == '"' && in_string && !is_escaped) {
            in_string = false;
        } else if (c == '\\' && !is_escaped) {
            is_escaped = true;
        } else if (c == ',' && !is_escaped && !in_string) {
            if (next_nonspace_char(json.substr(i + 1)) == ']' || next_nonspace_char(json.substr(i + 1)) == ',') {
                throw std::invalid_argument(std::string("Cannot parse: ") + c + "; " + json.substr(i-10, 20));
            }
            if (!value_buffer.empty()) {
                map.insert({std::to_string(index), value_buffer});
                value_buffer.clear();
                ++index;
            }
        } else if (in_string) {
            //value_buffer.append(1, c);
            value_buffer.append(1, (is_escaped ? resolve_escape_characters(c) : c));
            is_escaped = false;
        } else if (c == '{' && !in_string) {
            for (const auto &[k, v] : json_simplify_object(input.substr(i), i)) {
                map.insert({std::to_string(index) + "." + k, v});
            }
            ++index;
            is_escaped = false;
        } else if (c == '[' && !in_string) {
            for (const auto &[k, v] : json_simplify_array(input.substr(i), i)) {
                map.insert({std::to_string(index) + "." + k, v});
            }
            ++index;
            is_escaped = false;
        } else {
            std::string tmp {};
            u_int64_t skip {0};
            if (is_next_valid_value(json.substr(i), in_string, tmp, skip)) {
                value_buffer.append(tmp);
                i += skip - 1;
            } else {
                throw std::invalid_argument(std::string("Cannot parse: ") + c + "; " + json.substr(i-10, 20));
            }
            is_escaped = false;
        }
    }

    if (!value_buffer.empty()) {
        map.insert({std::to_string(index), value_buffer});
        value_buffer.clear();
    }

    return map;

}

std::map<std::string, std::string> json_simplify_object(const std::string &input, u_int64_t &offset) {

    std::string json = substring_curly_braces(input);
    offset += json.length() - 1;

    std::map<std::string, std::string> map {};
    bool is_escaped = false;
    bool in_string = false;
    bool is_value = false;
    std::string key_buffer {};
    std::string value_buffer {};

    for (u_int64_t i = 1; i < json.length() - 1; ++i) {
        const char c = json[i];
        if (isspace(c) && !in_string) {
            continue;
        } else if (c == '"' && !in_string && !is_escaped) {
            in_string = true;
        } else if (c == '"' && in_string && !is_escaped) {
            in_string = false;
        } else if (c == ':' && !in_string) {
            is_value = true;
        } else if (c == '\\' && !is_escaped) {
            is_escaped = true;
        } else if (c == ',' && !is_escaped && !in_string) {
            if (next_nonspace_char(json.substr(i + 1)) == '}' || next_nonspace_char(json.substr(i + 1)) == ',') {
                throw std::invalid_argument(std::string("Cannot parse: ") + c + "; " + json.substr(i-10, 20));
            }
            if (!key_buffer.empty()) {
                map.insert({key_buffer, value_buffer});
                key_buffer.clear();
                value_buffer.clear();
            }

            is_value = false;
        } else if (in_string && !is_value) {
            key_buffer.append(1, (is_escaped ? resolve_escape_characters(c) : c));
            is_escaped = false;
        } else if (is_value && c != '{' && c != '[') {
            std::string tmp {};
            u_int64_t skip {0};
            if (is_next_valid_value(json.substr(i), in_string, tmp, skip)) {
                value_buffer.append(tmp);
                i += skip - 1;
            } else {
                throw std::invalid_argument(std::string("Cannot parse: ") + c + "; " + json.substr(i-10, 20));
            }
            is_escaped = false;
        } else if (is_value) {
            if (c == '{') {
                for (const auto &[k, v] : json_simplify_object(input.substr(i), i)) {
                    map.insert({key_buffer + "." + k, v});
                }
            } else if (c == '[') {
                for (const auto &[k, v] : json_simplify_array(input.substr(i), i)) {
                    map.insert({key_buffer + "." + k, v});
                }
            }
            key_buffer.clear();
            is_value = false;
            is_escaped = false;
        } else {
            throw std::invalid_argument(std::string("Cannot parse: ") + c + "; " + json.substr(i-10, 20));
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

    std::vector<char> stack {};
    bool is_escaped = false;
    bool in_string = false;
    for (const char &c : input) {
        if (c == '\\' && !is_escaped) {
            is_escaped = true;
            continue;
        } else if (c == '"' && !in_string && !is_escaped) {
            in_string = true;
        } else if (c == '"' && in_string && !is_escaped) {
            in_string = false;
        } else if ((c == '{' || c == '[') && !in_string) {
            stack.push_back(c);
        } else if ((c == '}' || c == ']') && !in_string) {
            if (stack.size() <= 0) {
                throw std::invalid_argument("More braces are closed than opened");
            } else if (c == '}' && stack.back() == '{') {
                stack.pop_back();
            } else if (c == '}' && stack.back() == '[') {
                throw std::invalid_argument("Mismatching braces, expected ] but got }");
            } else if (c == ']' && stack.back() == '[') {
                stack.pop_back();
            } else if (c == ']' && stack.back() == '{') {
                throw std::invalid_argument("Mismatching braces, expected } but got ]");
            }
        }
        is_escaped = false;
    }
    if (!stack.empty()) {
        throw std::invalid_argument("Not all braces are closed");
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
    if (input.empty()) {
        return std::map<std::string, std::string>();
    }
    throw std::invalid_argument(std::string("Cannot parse: ") + input);
}
