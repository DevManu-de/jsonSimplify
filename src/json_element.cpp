#include "json_simplify.hpp"

json_simplify::json_element::json_element(enum json_element_type element_type) : element_type(element_type) {

}

enum json_simplify::json_element_type json_simplify::json_element::get_type() const noexcept {
    return this->element_type;
}
