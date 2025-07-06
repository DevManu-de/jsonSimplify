#include "json_simplify.hpp"

json_simplify::json_array::json_array() noexcept : json_element(json_element_type::JSON_ARRAY) {

}
void json_simplify::json_array::insert(json_element *j) noexcept {
    this->get_list().push_back(j);
}

const std::string json_simplify::json_array::at() const {
    throw json_simplify::generate_unsupported_function("at()", "json_array");
}
const json_simplify::json json_simplify::json_array::at(std::string key) const {
    throw json_simplify::generate_unsupported_function("at(u_int64_t)", "json_array");
}
const json_simplify::json json_simplify::json_array::at(u_int64_t index) const {
    return this->list.at(index);
}

std::vector<json_simplify::json> json_simplify::json_array::to_vector() const {
    std::vector<json> vec{};
    for (json_element *elem : this->get_list()) {
        vec.push_back(json(elem));
    }
    return vec;
}

std::map<std::string, json_simplify::json> json_simplify::json_array::to_map() const {
    throw generate_unsupported_function("to_map", "json_array");
}
std::map<std::string, std::string> json_simplify::json_array::to_pairs() const noexcept {
    std::map<std::string, std::string> map{};

    u_int64_t index {0};
    for (const auto &v : this->get_list()) {
        for (const auto &[k, v2] : v->to_pairs()) {
            map.insert({std::to_string(index) + (v->is_key_truth() ? "." : "") + k, v2});
        }
        ++index;
    }

    return map;
}

std::string json_simplify::json_array::to_string(int level, bool prettify) const noexcept {

    std::string buffer {"["};

    if (prettify) {
        buffer.append("\n");
    }

    for (const json_element *jsn : this->get_list()) {
        if (prettify) {
            buffer.append(json_element::level_to_spaces(level));
        }
        buffer.append(jsn->to_string(level + 1, prettify));
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

    buffer.append("]");

    return buffer;
}

void json_simplify::json_array::add(json_element *element) {
    this->insert(element);
}

void json_simplify::json_array::add(std::string key, json_element *element) {
    throw json_simplify::generate_unsupported_function("add(string, json_element)", "json_array");
}

json_simplify::json_element *json_simplify::json_array::deep_copy() const noexcept {
    json_element *j = new json_array();
    for (const auto *je : this->get_list()) {
        j->add(je->deep_copy());
    }
    return j;
}

std::vector<json_simplify::json_element*> &json_simplify::json_array::get_list() noexcept {
    return this->list;
}

const std::vector<json_simplify::json_element*> &json_simplify::json_array::get_list() const noexcept {
    return this->list;
}

bool json_simplify::json_array::is_key_truth() const noexcept {
    return true;
}

json_simplify::json_array::~json_array() noexcept {
    for (const auto &v : this->get_list()) {
        delete v;
    }
}
