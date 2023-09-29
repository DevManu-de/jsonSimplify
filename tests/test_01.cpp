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

    const json_simplify::json parsed {content};

    try {
        int i {0};
        for (const auto &[k, v] : parsed.to_map()) {
            std::cout << k << ":\t" << v << std::endl;
            ++i;
        }
        std::cout << i << std::endl;
    } catch (const json_simplify::json_invalid &e) {
        std::cerr << e.format() << std::endl;
    }

    std::cout << parsed.at("glossary").at("title").at() << std::endl;
    std::cout << parsed.at("glossary").at("GlossDiv").at("GlossList").at("GlossEntry").at("GlossTerm").at() << std::endl;

    std::cout << parsed.to_string(false) << std::endl;

    return 0;
}
