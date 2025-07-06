#ifndef JSON_ARRAY_HPP
#define JSON_ARRAY_HPP

#include <map>
#include <string>
#include <vector>

#include "json_element.hpp"

namespace json_simplify {

class json;

class json_array : public json_element {
private:
    std::vector<json_element*> list {};

    std::vector<json_element *> &get_list() noexcept;

protected:
    bool is_key_truth() const noexcept;

public:
    json_array() noexcept;
    void insert(json_element*) noexcept;
    const std::string at() const;
    const json at(std::string) const;
    const json at(u_int64_t) const;
    std::vector<json> to_vector() const;
    std::map<std::string, json> to_map() const;
    std::map<std::string, std::string> to_pairs() const noexcept;
    std::string to_string(int level, bool prettify = false) const noexcept;
    void add(json_element *element);
    void add(std::string key, json_element *element);
    json_element *deep_copy() const noexcept;

    const std::vector<json_element *> &get_list() const noexcept;

    ~json_array() noexcept;
};
}

#endif // JSON_ARRAY_HPP
