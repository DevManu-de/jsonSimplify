#include <iostream>
#include <cstring>
#include <memory>

#include "json_simplify.hpp"

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[], __attribute__((unused)) char *envp[]) {

    std::ifstream file{"cases/test02f.txt"};

    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        exit(1);
    }

    std::string content {};

    std::string s {};
    while(getline(file, s)) {
        content.append(s);
    }

    try {
        const json_simplify::json parsed {json_simplify::json(content)};

        int i {0};
        for (const auto &[k, v] : parsed.to_pairs()) {
            std::cout << k << ":\t" << v << std::endl;
            ++i;
        }
        std::cout << i << std::endl;
    } catch (const std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        return 0;
    }

    return 1;
}
