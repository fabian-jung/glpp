#pragma once

#include <string>
#include <vector>

struct constant_definition_t {
    std::string type;
    std::string name;
    std::string value;
};

struct parameter_definition_t {
    std::string type;
    std::string name;

    parameter_definition_t(std::string compound);
};

struct function_definition_t {
    std::string result;
    std::vector<parameter_definition_t> arguments;
    std::string name;

    // The constructor is implemented explicitly to speed up compile time
    function_definition_t(
        std::string result,
        std::vector<parameter_definition_t> arguments,
        std::string name
    );
};

struct type_definition_t {
    std::string native_type;
    std::string alias;
};