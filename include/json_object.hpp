#ifndef JSON_OBJECT_HPP
#define JSON_OBJECT_HPP

#include <map>
#include <string>

#include "json_element.hpp"

namespace json_simplify {

class json;

class json_object : public json_element {
private:
    std::map<std::string, json_element*> map {};

    std::map<std::string, json_element*> &get_map() noexcept;

protected:
    bool is_key_truth() const noexcept;

public:
    json_object() noexcept;
    void insert(std::pair<std::string, json_element*>) noexcept;
    const std::string at() const;
    const json at(std::string) const;
    const json at(u_int64_t) const;
    std::map<std::string, std::string> to_map() const noexcept;
    std::string to_string(int level, bool prettify = false) const noexcept;
    void add(json_element *element);
    void add(std::string key, json_element *element);

    const std::map<std::string, json_element*> &get_map() const noexcept;

    ~json_object() noexcept;
};

}

#endif // JSON_OBJECT_HPP
