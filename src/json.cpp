#include "json_simplify.hpp"

json_simplify::json::json(const std::string json) : jsn(json_simplify::json_simplify(json)), free_jsn(true) {

}

json_simplify::json::json(json_element *jsn) : jsn(jsn), free_jsn(false) {

}

json_simplify::json::json(const std::string value, const bool quoted) : jsn(new json_value(value, quoted)), free_jsn(false) {

}

json_simplify::json::json(const JSON_SIMPLIFY_DEFAULT_ARRAY vec, const bool root) : jsn(new json_array()), free_jsn(root) {

    for (const auto &j : vec) {
        this->jsn->add(j.jsn);
    }
}

json_simplify::json::json(const JSON_SIMPLIFY_DEFAULT_OBJECT map, const bool root) : jsn(new json_object()), free_jsn(root) {

    for (const auto &[k, v] : map) {
        this->jsn->add(k, v.jsn);
    }

}

std::string json_simplify::json::at() const {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at();
}
json_simplify::json json_simplify::json::at(std::string value) const {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at(value);
}
json_simplify::json json_simplify::json::at(u_int64_t index) const {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at(index);
}
std::map<std::string, std::string> json_simplify::json::to_map() const {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->to_map();
}

bool json_simplify::json::is_key_truth() const {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->is_key_truth();
}

std::string json_simplify::json::to_string(bool prettify) const noexcept {
    return this->jsn->to_string(prettify);
}

json_simplify::json &json_simplify::json::add(json jsn) {
    this->jsn->add(jsn.jsn);
    return *this;
}

json_simplify::json &json_simplify::json::add(const std::string key, json jsn) {
    this->jsn->add(key, jsn.jsn);
    return *this;
}

bool &json_simplify::json::root() noexcept {
    return this->free_jsn;
}

enum json_simplify::json_element_type json_simplify::json::get_type() const noexcept {
    return this->jsn->get_type();
}

json_simplify::json::~json() noexcept {
    if (this->free_jsn) {
        delete this->jsn;
    }

}
