#include <glpp/core/object/shader_factory.hpp>
#include <glm/glm.hpp>
#include <streambuf>

namespace glpp::core::object{

shader_factory_t::shader_factory_t(std::ifstream& code_template) :
	m_code(std::istreambuf_iterator<char>(code_template), std::istreambuf_iterator<char>())
{}

shader_factory_t::shader_factory_t(std::ifstream code_template) :
	m_code(std::istreambuf_iterator<char>(code_template), std::istreambuf_iterator<char>())
{}

shader_factory_t::shader_factory_t(std::string code_template) :
	m_code(code_template)
{}

std::string to_string(const std::string& value) {
	return value;
}

std::string to_string(const std::string_view& value) {
	return std::string{ value };
}


template <class T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
std::string to_string(const T& value) {
	return std::to_string(value);
}

template <class T, int N>
std::string to_string(const glm::vec<N, T>& value) {
	std::string result = "vec"+std::to_string(N)+"( ";
	for(int i = 0; i < N; ++i) {
		if(i != 0) {
			result += ", ";
		}
		result += std::to_string(value[i]);
	}
	result += ")";
	return result;
}

template<typename T>
shader_factory_t& shader_factory_t::set(std::string_view key, const T& value)
{
	std::string::size_type pos = 0;
	while((pos = m_code.find(key, pos)) != m_code.npos) {
		const auto replace = to_string(value);
		m_code.erase(pos, key.length());
		m_code.insert(pos, replace);
		pos += replace.length();
	}

	return *this;
}

template shader_factory_t& shader_factory_t::set(std::string_view key, const float& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const double& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::string& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::string_view& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const glm::vec1& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const glm::vec2& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const glm::vec3& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const glm::vec4& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const glm::dvec1& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const glm::dvec2& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const glm::dvec3& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const glm::dvec4& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::int8_t& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::int16_t& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::int32_t& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::int64_t& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::uint8_t& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::uint16_t& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::uint32_t& value);
template shader_factory_t& shader_factory_t::set(std::string_view key, const std::uint64_t& value);

}
