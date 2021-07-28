#pragma once

#include <string>
#include <istream>
#include <fstream>
#include <sstream>

namespace glpp::core::object {

class shader_factory_t {
public:
	explicit shader_factory_t(std::ifstream& file);
	explicit shader_factory_t(std::ifstream&& file);

	explicit shader_factory_t(const std::stringstream& sstream);

	explicit shader_factory_t(std::string code_template);
	
	shader_factory_t(const shader_factory_t& cpy) = default;
	shader_factory_t(shader_factory_t&& mov) noexcept = default;

	shader_factory_t& operator=(const shader_factory_t& cpy) = default;
	shader_factory_t& operator=(shader_factory_t&& mov) noexcept = default;

	template <class T>
	shader_factory_t& set(std::string_view key, const T& value);
	
	shader_factory_t& set(std::string_view key, const char* value);

	std::string code() const ;
	
	
private:
	std::string m_code;
};

}