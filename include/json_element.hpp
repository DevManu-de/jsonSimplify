#ifndef JSON_ELEMENT_HPP
#define JSON_ELEMENT_HPP

#include <string>
#include <map>
#include <vector>

namespace json_simplify {

class json;
enum class json_element_type;

class json_element {
private:
    const json_element_type element_type;

protected:
    static std::string level_to_spaces(int level, std::string str = "    ") noexcept;

public:
    json_element(enum json_element_type element_type);

    virtual const std::string at() const = 0;
    virtual const json at(std::string) const = 0;
    virtual const json at(u_int64_t) const = 0;
    virtual std::map<std::string, std::string> to_map() const = 0;
    virtual bool is_key_truth() const = 0;
    virtual std::string to_string(int level, bool prettified = false) const noexcept = 0;
    virtual void add(json_element *element) = 0;
    virtual void add(std::string key, json_element *element) = 0;

    std::string to_string(bool prettify) const noexcept;
    enum json_element_type get_type() const noexcept;

    virtual ~json_element() = default;

};

}
#endif // JSON_ELEMENT_HPP
