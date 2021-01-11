#pragma once

#include <string>

namespace glpp::asset::shading {

class shader_factory_t {
public:
	shader_factory_t(std::string code_template);
	
	template <class T>
	shader_factory_t& set(std::string_view key, const T& value);
	
	std::string code() const {
		return m_code;
	}
	
private:
	std::string m_code;
};

}
