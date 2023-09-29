#include "json_simplify.hpp"

json_simplify::json_object::json_object() noexcept : json_element(json_element_type::JSON_OBJECT) {

}
void json_simplify::json_object::insert(std::pair<std::string, json_element*> p) noexcept {
    this->get_map().insert(p);
}
const std::string json_simplify::json_object::at() const {
    throw json_simplify::generate_unsupported_function("at()", "json_object");
}
const json_simplify::json json_simplify::json_object::at(std::string key) const {
    return this->map.at(key);
}
const json_simplify::json json_simplify::json_object::at(u_int64_t) const {
    throw json_simplify::generate_unsupported_function("at(u_int64_t)", "json_object");
}
std::map<std::string, std::string> json_simplify::json_object::to_map() const noexcept {
    std::map<std::string, std::string> map {};

    for (const auto &[k1, v1] : this->get_map()) {
        for (const auto &[k2, v2] : v1->to_map()) {
            map.insert({k1 + (v1->is_key_truth() ? "." : "") + k2, v2});
        }
    }

    return map;
}

std::string json_simplify::json_object::to_string(int level, bool prettify) const noexcept {
    std::string buffer {"{"};

    if (prettify) {
        buffer.append("\n");
    }

    for (const auto &[k, v] : this->get_map()) {
        if (prettify) {
            buffer.append(json_element::level_to_spaces(level));
        }
        buffer.append("\"");
        buffer.append(k);
        buffer.append("\":");
        if (prettify) {
            buffer.append(" ");
        }
        buffer.append(v->to_string(level + 1, prettify));
        if (prettify) {
            buffer.append(",\n");
        } else {
            buffer.append(",");
        }

    }

    if (prettify) {
        if (buffer.ends_with(",\n")) {
            buffer.pop_back();
            buffer.pop_back();
        }
        buffer.append("\n");
        buffer.append(json_element::level_to_spaces(level - 1));
    } else {
        if (buffer.ends_with(",")) {
            buffer.pop_back();
        }
    }



    buffer.append("}");
    return buffer;
}

std::map<std::string, json_simplify::json_element*> &json_simplify::json_object::get_map() noexcept {
    return this->map;
}

const std::map<std::string, json_simplify::json_element*> &json_simplify::json_object::get_map() const noexcept {
    return this->map;
}

bool json_simplify::json_object::is_key_truth() const noexcept {
    return true;
}

json_simplify::json_object::~json_object() noexcept {
    for (const auto &[k, v] : this->get_map()) {
        delete v;
    }
}
