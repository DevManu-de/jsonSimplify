#ifndef JSON_HPP
#define JSON_HPP

#include <map>
#include <string>
#include <vector>

namespace json_simplify {

class json_element;
class json_value;
class json_object;
class json_array;
enum class json_element_type;

#define JSON_SIMPLIFY_DEFAULT_OBJECT std::map<std::string, json>
#define JSON_SIMPLIFY_DEFAULT_ARRAY std::vector<json>

class json {
private:
    json_element *jsn = nullptr;
    bool free_jsn;

    json(json_element *jsn, const bool root = false);

    friend class json_value;
    friend class json_object;
    friend class json_array;

    friend json_object *json_simplify_object(const std::string &, u_int64_t &);
    friend json_array *json_simplify_array(const std::string &, u_int64_t &);

public:

    json(const std::string json);

    json(const std::string value, const bool quoted);
    json(const JSON_SIMPLIFY_DEFAULT_ARRAY vec, const bool root = false);
    json(const JSON_SIMPLIFY_DEFAULT_OBJECT map, const bool root = false);

    std::string at() const;
    json at(std::string) const;
    json at(u_int64_t) const;
    json operator[] (std::string) const;
    json operator[] (u_int64_t) const;
    std::vector<json> to_vector() const;
    std::map<std::string, json> to_map() const;
    std::map<std::string, std::string> to_pairs() const;
    bool is_key_truth() const;
    std::string to_string(bool prettify = false) const noexcept;
    json_simplify::json &add(json &jsn, bool copy = false);
    json_simplify::json &add(json &&jsn, bool copy = false);
    json_simplify::json &add(const std::string key, json &jsn, bool copy = false);
    json_simplify::json &add(const std::string key, json &&jsn, bool copy = false);
    json_simplify::json deep_copy() const noexcept;

    bool &root() noexcept;
    enum json_element_type get_type() const noexcept;

    virtual ~json() noexcept;
};

}

#endif // JSON_HPP
