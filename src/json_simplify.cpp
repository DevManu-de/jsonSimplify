#include "json_simplify.hpp"

using namespace json_simplify;

std::string json::match_braces_pair(const char &brace_open, const char &brace_close, const std::string &input) {

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

std::string json::substring_curly_braces(const std::string &input) {

    return json::match_braces_pair('{', '}', input);
}

std::string json::substring_squre_braces(const std::string &input) {

    return json::match_braces_pair('[', ']', input);
}

char json::next_nonspace_char(std::string input) {
    for (const char &c : input) {
        if (!isspace(c)) {
            return c;
        }
    }
    throw std::invalid_argument("Cannot parse: ");
}

bool json::is_next_valid_value(const std::string &json, const bool &in_string, std::string &output, u_int64_t &skip) {

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

char json::resolve_escape_characters(const char &c) {

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

json_simplify::json_invalid json::generate_error_message(const std::string &message, const std::string &json, const u_int64_t &err_pos, const u_int64_t &spacing) {
    u_int64_t err_start = err_pos - std::min(spacing, err_pos);
    u_int64_t err_len = std::min(err_start + spacing + std::min(json.length() - err_pos, spacing), spacing * 2);
    std::string snippet = json.substr(err_start, err_len);
    std::string msg = message + json[err_pos] + "; ";

    return json_simplify::json_invalid(msg, snippet, err_pos - err_start, msg + snippet);
}

json_unsupported_function json::generate_unsupported_function(std::string fn_name, std::string class_name) {
    return json_unsupported_function(std::string("Function \"") + fn_name + "\" is not supported for class \"" + class_name + "\"");
}

json_array *json::json_simplify_array(const std::string &input, u_int64_t &offset) {
    std::string json = substring_squre_braces(input);
    offset += json.length() - 1;

    json_array *array = new json_array();
    bool is_escaped = false;
    bool in_string = false;
    std::string value_buffer {};
    bool is_value_sealed = false;
    bool is_value_truth = false;

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
                array->insert(new json_value(value_buffer));
                value_buffer.clear();
            }
            is_value_sealed = false;
        } else if (in_string && !is_value_sealed) {
            value_buffer.append(1, (is_escaped ? resolve_escape_characters(c) : c));
            is_escaped = false;
        } else if (c == '{' && !in_string && !is_value_sealed) {
            array->insert(json_simplify_object(input.substr(i), i));
            is_escaped = false;
            is_value_sealed = true;
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
            } else {
                throw generate_error_message("Cannot parse: ", json, i, 10);
            }
            is_escaped = false;
        }
    }

    if (is_value_truth) {
        array->insert(new json_value(value_buffer));
    }

    return array;

}

json_object *json::json_simplify_object(const std::string &input, u_int64_t &offset) {

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
                object->insert({key_buffer, new json_value(value_buffer)});
            }
            key_buffer.clear();
            value_buffer.clear();
            is_key_sealed = false;
            is_value_sealed = false;
            is_key_truth = false;

            is_value = false;
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
            } else {
                throw generate_error_message("Cannot parse: ", json_string, i, 10);
            }
            is_escaped = false;
        } else if (is_value) {
            json *j = nullptr;
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
        object->insert({key_buffer, new json_value(value_buffer)});
    }

    return object;
}

json *json::json_simplify(const std::string &input) {

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
        return new json_value("");
    }
    throw std::invalid_argument(std::string("Cannot parse: ") + input);
}


// JSON
json::json(const enum json_element_type json_element_type) : element_type(json_element_type) {

}

json::json(const std::string json) : element_type(json_element_type::JSON_NONE) {
    this->jsn = json::json_simplify(json);
}

const std::string json::at() const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at();
}
const json_view json::at(std::string value) const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at(value);
}
const json_view json::at(u_int64_t index) const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at(index);
}
std::map<std::string, std::string> json::to_map() const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->to_map();
}

bool json::is_key_truth() const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->is_key_truth();
}

enum json_element_type json::get_type() const noexcept {
    return this->element_type;
}

json::~json() noexcept {
    delete this->jsn;
}

// JSON_VIEW
json_view::json_view(json *jsn) : jsn(jsn) {

}

const std::string json_view::at() const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at();
}
const json_view json_view::at(std::string value) const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at(value);
}
const json_view json_view::at(u_int64_t index) const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at(index);
}
std::map<std::string, std::string> json_view::to_map() const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->to_map();
}

enum json_element_type json_view::get_type() const noexcept {
    return this->jsn->get_type();
}

bool json_view::is_key_truth() const {
    if (this->jsn == nullptr) {
        throw json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->is_key_truth();
}


// JSON_VALUE
json_value::json_value(std::string value) noexcept : json(json_element_type::JSON_VALUE), value(value) {

}

const std::string json_value::at() const {
    return this->value;
}
const json_view json_value::at(std::string) const {
    throw json::generate_unsupported_function("at(string)", "json_value");
}
const json_view json_value::at(u_int64_t) const {
    throw json::generate_unsupported_function("at(u_int64_t)", "json_value");
}
std::map<std::string, std::string> json_value::to_map() const noexcept {
    return std::map<std::string, std::string>{{"", this->value}};
}

const std::string &json_value::get_value() const noexcept {
    return this->value;
}

bool json_value::is_key_truth() const noexcept {
    return false;
}

// JSON_OBJECT
json_object::json_object() noexcept : json(json_element_type::JSON_OBJECT) {

}
void json_object::insert(std::pair<std::string, json*> p) noexcept {
    this->get_map().insert(p);
}
const std::string json_object::at() const {
    throw json::generate_unsupported_function("at()", "json_object");
}
const json_view json_object::at(std::string key) const {
    return this->map.at(key);
}
const json_view json_object::at(u_int64_t) const {
    throw json::generate_unsupported_function("at(u_int64_t)", "json_object");
}
std::map<std::string, std::string> json_object::to_map() const noexcept {
    std::map<std::string, std::string> map {};

    for (const auto &[k1, v1] : this->get_map()) {
        for (const auto &[k2, v2] : v1->to_map()) {
            map.insert({k1 + (v1->is_key_truth() ? "." : "") + k2, v2});
        }
    }

    return map;
}

std::map<std::string, json*> &json_object::get_map() noexcept {
    return this->map;
}

const std::map<std::string, json*> &json_object::get_map() const noexcept {
    return this->map;
}

bool json_object::is_key_truth() const noexcept {
    return true;
}

json_object::~json_object() noexcept {
    for (const auto &[k, v] : this->get_map()) {
        delete v;
    }
}

// JSON_ARRAY
json_array::json_array() noexcept : json(json_element_type::JSON_ARRAY) {

}
void json_array::insert(json *j) noexcept {
    this->get_list().push_back(j);
}

const std::string json_array::at() const {
    throw json::generate_unsupported_function("at()", "json_array");
}
const json_view json_array::at(std::string key) const {
    throw json::generate_unsupported_function("at(u_int64_t)", "json_array");
}
const json_view json_array::at(u_int64_t index) const {
    return this->list.at(index);
}
std::map<std::string, std::string> json_array::to_map() const noexcept {
    std::map<std::string, std::string> map {};

    u_int64_t index {0};
    for (const auto &v : this->get_list()) {
        for (const auto &[k, v2] : v->to_map()) {
            map.insert({std::to_string(index++) + (v->is_key_truth() ? "." : "") + k, v2});
        }
    }

    return map;
}

std::vector<json*> &json_array::get_list() noexcept {
    return this->list;
}

const std::vector<json*> &json_array::get_list() const noexcept {
    return this->list;
}

bool json_array::is_key_truth() const noexcept {
    return true;
}

json_array::~json_array() noexcept {
    for (const auto &v : this->get_list()) {
        delete v;
    }
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
