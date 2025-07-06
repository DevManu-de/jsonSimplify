#include "json_simplify.hpp"

json_simplify::json::json(const std::string json)
    : jsn(json_simplify::json_simplify(json)), free_jsn(true) {
}

json_simplify::json::json(json_element *jsn, const bool root) : jsn(jsn), free_jsn(root) {
}

json_simplify::json::json(const std::string value, const bool quoted)
    : jsn(new json_value(value, quoted)), free_jsn(true) {
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

json_simplify::json json_simplify::json::operator[] (std::string value) const {
    return this->at(value);
}

std::vector<json_simplify::json> json_simplify::json::to_vector() const {
    return this->jsn->to_vector();
}

std::map<std::string, json_simplify::json> json_simplify::json::to_map() const {
    return this->jsn->to_map();
}

json_simplify::json json_simplify::json::operator[] (u_int64_t index) const {
    return this->at(index);
}

std::map<std::string, std::string> json_simplify::json::to_pairs() const {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->to_pairs();
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

json_simplify::json &json_simplify::json::add(json &jsn, bool copy) {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    if (copy) {
        this->jsn->add(jsn.jsn->deep_copy());
    } else {
        this->jsn->add(jsn.jsn);
        jsn.root() = false;
        jsn.jsn = nullptr;
    }
    return *this;
}

json_simplify::json &json_simplify::json::add(json &&jsn, bool copy) {
    return this->add(jsn, copy);
}

json_simplify::json &json_simplify::json::add(const std::string key, json &jsn, bool copy) {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    if (copy) {
        this->jsn->add(key, jsn.jsn->deep_copy());
    } else {
        this->jsn->add(key, jsn.jsn);
        jsn.root() = false;
        jsn.jsn = nullptr;
    }
    return *this;
}

json_simplify::json &json_simplify::json::add(const std::string key, json &&jsn, bool copy) {
    return this->add(key, jsn, copy);
}

bool &json_simplify::json::root() noexcept {
    return this->free_jsn;
}

json_simplify::json json_simplify::json::deep_copy() const noexcept {
    return {this->jsn->deep_copy(), true};
}

enum json_simplify::json_element_type json_simplify::json::get_type() const noexcept {
    return this->jsn->get_type();
}

json_simplify::json::~json() noexcept {
    if (this->free_jsn) {
        delete this->jsn;
    }

}
