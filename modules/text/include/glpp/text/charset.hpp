#pragma once

#include <string>
#include <vector>

namespace glpp::text {

class charset_t {
public:
	using value_type = std::wstring::value_type;

	charset_t() = default;

	template <class... Args>
	charset_t(Args... args) :
	m_chars(std::forward<Args>(args)...)
	{}

	void populate();

	std::vector<value_type>::const_iterator begin() const;
	std::vector<value_type>::const_iterator end() const;

	static charset_t ascii();
	static charset_t all();

private:
	std::vector<value_type> m_chars;
};

}
