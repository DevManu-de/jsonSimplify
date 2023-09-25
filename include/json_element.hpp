#ifndef JSON_ELEMENT_HPP
#define JSON_ELEMENT_HPP

#include <string>
#include <map>

namespace json_simplify {

class json;
enum class json_element_type;

class json_element {
private:
    const json_element_type element_type;
public:
    json_element(enum json_element_type element_type);

    virtual const std::string at() const = 0;
    virtual const json at(std::string) const = 0;
    virtual const json at(u_int64_t) const = 0;
    virtual std::map<std::string, std::string> to_map() const = 0;
    virtual bool is_key_truth() const = 0;

    enum json_element_type get_type() const noexcept;

    virtual ~json_element() = default;

};

}
#endif // JSON_ELEMENT_HPP
