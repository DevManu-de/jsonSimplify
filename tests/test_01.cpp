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

    parsed["glossary"].add("child", parsed2);
    //parsed2["glossary"].add("child", parsed);

    /*try {
        int i {0};
        for (const auto &[k, v] : parsed.to_map()) {
            std::cout << k << ":\t" << v << std::endl;
            ++i;
        }
        std::cout << i << std::endl;
    } catch (const json_simplify::json_invalid &e) {
        std::cerr << e.format() << std::endl;
    }*/

    std::cout << parsed.to_string(true) << std::endl;

    if (parsed.to_pairs().size() == 9) {
        return 0;
    }

    return 1;
}
