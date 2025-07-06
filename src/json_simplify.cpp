#include "json_simplify.hpp"

using namespace json_simplify;

std::string json_simplify::match_braces_pair(const char &brace_open, const char &brace_close, const std::string &input) {

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

std::string json_simplify::substring_curly_braces(const std::string &input) {

    return json_simplify::match_braces_pair('{', '}', input);
}

std::string json_simplify::substring_squre_braces(const std::string &input) {

    return json_simplify::match_braces_pair('[', ']', input);
}

char json_simplify::next_nonspace_char(std::string input) {
    for (const char &c : input) {
        if (!isspace(c)) {
            return c;
        }
    }
    throw std::invalid_argument("Cannot parse: ");
}

bool json_simplify::is_next_valid_value(const std::string &json, const bool &in_string, std::string &output, u_int64_t &skip) {

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

char json_simplify::resolve_escape_characters(const char &c) {

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

json_simplify::json_invalid json_simplify::generate_error_message(const std::string &message, const std::string &json, const u_int64_t &err_pos, const u_int64_t &spacing) {
    u_int64_t err_start = err_pos - std::min(spacing, err_pos);
    u_int64_t err_len = std::min(err_start + spacing + std::min(json.length() - err_pos, spacing), spacing * 2);
    std::string snippet = json.substr(err_start, err_len);
    std::string msg = message + json[err_pos] + "; ";

    return json_simplify::json_invalid(msg, snippet, err_pos - err_start, msg + snippet);
}

json_unsupported_function json_simplify::generate_unsupported_function(std::string fn_name, std::string class_name) {
    return json_unsupported_function(std::string("Function \"") + fn_name + "\" is not supported for class \"" + class_name + "\"");
}

json_array *json_simplify::json_simplify_array(const std::string &input, u_int64_t &offset) {
    std::string json = substring_squre_braces(input);
    offset += json.length() - 1;

    json_array *array = new json_array();
    bool is_escaped = false;
    bool in_string = false;
    std::string value_buffer {};
    bool is_value_sealed = false;
    bool is_value_truth = false;
    bool was_value_quoted = false;

    for (u_int64_t i = 1; i < json.length() - 1; ++i) {
        const char c = json[i];
        if (isspace(c) && !in_string) {
            continue;
        } else if (c == '"' && !in_string && !is_escaped && !is_value_sealed) {
            in_string = true;
        } else if (c == '"' && in_string && !is_escaped) {
            in_string = false;
            is_value_sealed = true;
            is_value_truth = true;
        } else if (c == '\\' && !is_escaped) {
            is_escaped = true;
        } else if (c == ',' && !is_escaped && !in_string) {
            if (next_nonspace_char(json.substr(i + 1)) == ']' || next_nonspace_char(json.substr(i + 1)) == ',') {
                throw generate_error_message("Cannot parse: ", json, i, 10);
            }
            if (is_value_truth) {
                array->insert(new json_value(value_buffer, was_value_quoted));
                value_buffer.clear();
            }
            is_value_sealed = false;
            was_value_quoted = false;
        } else if (in_string && !is_value_sealed) {
            value_buffer.append(1, (is_escaped ? resolve_escape_characters(c) : c));
            was_value_quoted = in_string;
            is_escaped = false;
        } else if (c == '{' && !in_string && !is_value_sealed) {
            array->insert(json_simplify_object(input.substr(i), i));
            is_escaped = false;
            is_value_sealed = true;
            is_value_truth = false;
        } else if (c == '[' && !in_string && !is_value_sealed) {
            array->insert(json_simplify_array(json.substr(i), i));
            is_escaped = false;
            is_value_sealed = true;
            is_value_truth = false;
        } else {
            std::string tmp {};
            u_int64_t skip {0};
            if (is_next_valid_value(json.substr(i), in_string, tmp, skip) && !is_value_sealed) {
                value_buffer.append(tmp);
                i += skip - 1;
                is_value_sealed = true;
                was_value_quoted = in_string;
            } else {
                throw generate_error_message("Cannot parse: ", json, i + skip - 1, 10);
            }
            is_escaped = false;
        }
    }

    if (is_value_truth) {
        array->insert(new json_value(value_buffer, was_value_quoted));
    }

    return array;

}

json_object *json_simplify::json_simplify_object(const std::string &input, u_int64_t &offset) {

    std::string json_string = substring_curly_braces(input);
    offset += json_string.length() - 1;

    json_object *object = new json_object();
    bool is_escaped = false;
    bool in_string = false;
    bool is_value = false;
    std::string key_buffer {};
    bool is_key_sealed = false;
    bool is_key_truth = false;
    std::string value_buffer {};
    bool is_value_sealed = false;
    bool was_value_quoted = false;

    for (u_int64_t i = 1; i < json_string.length() - 1; ++i) {
        const char c = json_string[i];
        if (isspace(c) && !in_string) {
            continue;
        } else if (c == '"' && !in_string && !is_escaped && ((!is_value && !is_key_sealed) || is_value && !is_value_sealed)) {
            in_string = true;
        } else if (c == '"' && in_string && !is_escaped) {
            in_string = false;
            if (!is_value) {
                is_key_sealed = true;
            } else {
                is_value_sealed = true;
                is_key_truth = true;
                is_value = false;
            }
        } else if (c == ':' && !in_string) {
            is_value = true;
        } else if (c == '\\' && !is_escaped) {
            is_escaped = true;
        } else if (c == ',' && !is_escaped && !in_string) {
            if (next_nonspace_char(json_string.substr(i + 1)) == '}' || next_nonspace_char(json_string.substr(i + 1)) == ',') {
                throw generate_error_message("Cannot parse: ", json_string, i, 10);
            }
            if (!is_key_sealed || !is_value_sealed) {
                throw generate_error_message("Cannot parse: ", json_string, i, 10);
            } else if (is_key_truth) {
                object->insert({key_buffer, new json_value(value_buffer, was_value_quoted)});
            }
            key_buffer.clear();
            value_buffer.clear();
            is_key_sealed = false;
            is_value_sealed = false;
            is_key_truth = false;

            is_value = false;
            was_value_quoted = false;
        } else if (in_string && !is_value && !is_key_sealed) {
            key_buffer.append(1, (is_escaped ? resolve_escape_characters(c) : c));
            is_escaped = false;
        } else if (is_value && !is_value_sealed && c != '{' && c != '[') {
            std::string tmp {};
            u_int64_t skip {0};
            if (is_next_valid_value(json_string.substr(i), in_string, tmp, skip)) {
                value_buffer.append(tmp);
                i += skip - 1;
                is_value_sealed = true;
                is_key_truth = true;
                was_value_quoted = in_string;
            } else {
                throw generate_error_message("Cannot parse: ", json_string, i, 10);
            }
            is_escaped = false;
        } else if (is_value) {
            json_element *j = nullptr;
            if (c == '{') {
                j = json_simplify_object(json_string.substr(i), i);
            } else if (c == '[') {
                j = json_simplify_array(json_string.substr(i), i);
            }
            object->insert({key_buffer, j});
            is_value = false;
            is_escaped = false;
            is_value_sealed = true;
            is_key_truth = false;
        } else {
            throw generate_error_message("Cannot parse: ", json_string, i, 10);
        }
    }

    if (is_key_truth) {
        object->insert({key_buffer, new json_value(value_buffer, was_value_quoted)});
    }

    return object;
}

json_element *json_simplify::json_simplify(const std::string &input) {

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

    if (next_nonspace_char(input) == '{') {
        u_int64_t tmp = 0;
        return json_simplify_object(input, tmp);
    } else if (next_nonspace_char(input) == '[') {
        u_int64_t tmp = 0;
        return json_simplify_array(input, tmp);
    }

    if (input.empty()) {
        return new json_value("", true);
    }
    throw std::invalid_argument(std::string("Cannot parse: ") + input);
}


// JSON_INVALID
json_invalid::json_invalid(std::string msg, std::string snippet, u_int64_t pos, std::string what)
    : std::runtime_error(what), msg(msg), snippet(snippet), pos(pos) {

}
std::string json_invalid::format() const {
    return std::string().append(this->what()).append(1, '\n').append(this->msg.length(), ' ').append(this->pos, '~').append(1, '^');
}

// JSON_UNSUPPORTED_FUNCTION
json_unsupported_function::json_unsupported_function(std::string msg) : std::runtime_error(msg) {

}
