#ifndef JSON_HPP
#define JSON_HPP

#include <map>
#include <string>

namespace json_simplify {

class json_element;
enum class json_element_type;

class json {
private:
    const json_element *jsn = nullptr;

public:

    json(const std::string json);
    json(const json_element *jsn);

    const std::string at() const;
    const json at(std::string) const;
    const json at(u_int64_t) const;
    std::map<std::string, std::string> to_map() const;
    bool is_key_truth() const;

    enum json_element_type get_type() const noexcept;

    virtual ~json() noexcept;
};

}

#endif // JSON_HPP
