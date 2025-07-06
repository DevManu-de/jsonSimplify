#include <iostream>
#include <cstring>
#include <memory>

#include "json_simplify.hpp"

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[], __attribute__((unused)) char *envp[]) {

    std::ifstream file{"cases/test01.txt"};

    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        exit(1);
    }

    std::string content {};

    std::string s {};
    while(getline(file, s)) {
        content.append(s);
    }

    json_simplify::json parsed {content};
    json_simplify::json parsed2 {content};

    parsed["glossary"].add("child1", parsed2, true);
    parsed2["glossary"].add("child2", parsed);
    json_simplify::json parsed2deepcopy = parsed2.deep_copy();

    std::cout << parsed2.to_string(true) << std::endl;

    try {
        parsed.at("asdf");
        return 1;
    } catch (json_simplify::json_unsupported_function &e) {
    }

    if (parsed2.to_pairs() == parsed2deepcopy.to_pairs()) {
        return 0;
    }

    return 1;
}
