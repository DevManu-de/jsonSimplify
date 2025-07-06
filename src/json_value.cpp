#include "json_simplify.hpp"

json_simplify::json_value::json_value(std::string value, bool quoted) noexcept : json_element(json_element_type::JSON_VALUE), value(value), quoted(quoted) {

}

const std::string json_simplify::json_value::at() const {
    return this->value;
}
const json_simplify::json json_simplify::json_value::at(std::string) const {
    throw json_simplify::generate_unsupported_function("at(string)", "json_value");
}
const json_simplify::json json_simplify::json_value::at(u_int64_t) const {
    throw json_simplify::generate_unsupported_function("at(u_int64_t)", "json_value");
}

std::vector<json_simplify::json> json_simplify::json_value::to_vector() const {
    throw json_simplify::generate_unsupported_function("to_vector", "json_value");
}

std::map<std::string, json_simplify::json> json_simplify::json_value::to_map() const {
    throw json_simplify::generate_unsupported_function("to_map", "json_value");
}
std::map<std::string, std::string> json_simplify::json_value::to_pairs() const noexcept {
    return std::map<std::string, std::string>{{"", this->value}};
}
std::string json_simplify::json_value::to_string(int level, bool prettify) const noexcept {
    if (this->quoted) {
        return std::string("\"") + this->get_value() + "\"";
    }
    return this->get_value();
}

void json_simplify::json_value::add(json_element *element) {
    throw json_simplify::generate_unsupported_function("add(json_element)", "json_value");
}

void json_simplify::json_value::add(std::string key, json_element *element) {
    throw json_simplify::generate_unsupported_function("add(string, json_element)", "json_value");
}

json_simplify::json_element *json_simplify::json_value::deep_copy() const noexcept {
    json_element *j = new json_value(this->get_value(), this->quoted);
    return j;
}

const std::string &json_simplify::json_value::get_value() const noexcept {
    return this->value;
}

bool json_simplify::json_value::is_key_truth() const noexcept {
    return false;
}
