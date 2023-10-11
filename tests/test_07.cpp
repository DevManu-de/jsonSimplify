#include <iostream>
#include <cstring>
#include <memory>
#include <map>
#include <vector>

#include "json_simplify.hpp"

using namespace json_simplify;
using namespace std;

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[], __attribute__((unused)) char *envp[]) {

    json jsn {JSON_SIMPLIFY_DEFAULT_OBJECT(), true};
    jsn.add("list", JSON_SIMPLIFY_DEFAULT_ARRAY());
    json &list = jsn.at("list").add(json("a", true)).add(json("b", true)).add(json("c", true));
    list.add(JSON_SIMPLIFY_DEFAULT_OBJECT()).at(3).add("key", json("A", true)).add("status", json("false", false));

    cout << jsn.to_string(true) << endl;

    json jsn2 {jsn.to_string()};
    jsn2.at("list").at(3).add("definitions", JSON_SIMPLIFY_DEFAULT_ARRAY()).at("definitions").add(json("1a", true)).add(json("2", false)).add(json("3", false));

    cout << jsn2.to_string(true) << endl;
    for (const auto &[k, v] : jsn2.to_pairs()) {
        cout << k << ": " << v << endl;
    }

    try {
        json jsn3 {jsn2.to_string()};
        if (jsn3.to_pairs().size() == 8) {
            return 0;
        }
    } catch (json_invalid e) {
        cerr << e.format() << endl;
    }

    return 1;
}
