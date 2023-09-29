#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include <string>
#include <map>

#include "json.hpp"
#include "json_element.hpp"

namespace json_simplify {

class json_value : public json_element {
private:
    const std::string value;
    const bool quoted;

protected:
    bool is_key_truth() const noexcept;

public:
    json_value(std::string value, bool quoted) noexcept;

    const std::string at() const;
    const json at(std::string) const;
    const json at(u_int64_t) const;
    std::map<std::string, std::string> to_map() const noexcept;
    std::string to_string(int level, bool prettify = false) const noexcept;

    const std::string &get_value() const noexcept;

    ~json_value() noexcept = default;

};

}

#endif // JSON_VALUE_HPP
