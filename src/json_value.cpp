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
std::map<std::string, std::string> json_simplify::json_value::to_map() const noexcept {
    return std::map<std::string, std::string>{{"", this->value}};
}
std::string json_simplify::json_value::to_string(int level, bool prettify) const noexcept {
    if (this->quoted) {
        return std::string("\"") + this->get_value() + "\"";
    }
    return this->get_value();
}

const std::string &json_simplify::json_value::get_value() const noexcept {
    return this->value;
}

bool json_simplify::json_value::is_key_truth() const noexcept {
    return false;
}
