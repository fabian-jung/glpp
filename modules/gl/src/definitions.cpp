#include "definitions.hpp"

parameter_definition_t::parameter_definition_t(std::string compound) {
    const auto seperator = compound.find_last_of(' ');
    type = compound.substr(0, seperator);
    name = compound.substr(seperator+1);
}

function_definition_t::function_definition_t(
    std::string result,
    std::vector<parameter_definition_t> arguments,
    std::string name
) :
    result(std::move(result)),
    arguments(std::move(arguments)),
    name(std::move(name))
{}