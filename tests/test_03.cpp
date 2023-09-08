#include <memory>
#include "json_simplify.hpp"


int main(int argc, char *argv[]) {

    std::unique_ptr<json_simplify::json> a {json_simplify::json::json_simplify("{ \"a\": false}")};
    std::string value {((json_simplify::json_value *) (a->at("a")))->get_value()};

    if (value == "false") {
        return 0;
    }

    return 1;
}
