#include "json_simplify.hpp"

json_simplify::json_element::json_element(enum json_element_type element_type) : element_type(element_type) {

}

std::string json_simplify::json_element::to_string(bool prettify) const noexcept {
    return this->to_string(1, prettify);
}

std::string json_simplify::json_element::level_to_spaces(int level, std::string str) noexcept {
    std::string buffer;
    for (int i = 0; i < level; ++i) {
        buffer.append(str);
    }
    return buffer;
}

enum json_simplify::json_element_type json_simplify::json_element::get_type() const noexcept {
    return this->element_type;
}
