#include "json_simplify.hpp"

json_simplify::json::json(const std::string json) : jsn(json_simplify::json_simplify(json)) {

}

json_simplify::json::json(const json_element *jsn) : jsn(jsn) {

}

const std::string json_simplify::json::at() const {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at();
}
const json_simplify::json json_simplify::json::at(std::string value) const {
    if (this->jsn == nullptr) {
        throw json_simplify::json_unsupported_function("jsn = nullptr");
    }
    return this->jsn->at(value);
}
const json_simplify::json json_simplify::json::at(u_int64_t index) const {
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

json_simplify::json::~json() noexcept {
    delete this->jsn;
}
