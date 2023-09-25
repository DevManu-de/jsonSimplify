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
std::map<std::string, std::string> json_simplify::json_array::to_map() const noexcept {
    std::map<std::string, std::string> map {};

    u_int64_t index {0};
    for (const auto &v : this->get_list()) {
        for (const auto &[k, v2] : v->to_map()) {
            map.insert({std::to_string(index++) + (v->is_key_truth() ? "." : "") + k, v2});
        }
    }

    return map;
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
