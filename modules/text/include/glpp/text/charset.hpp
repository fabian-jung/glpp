#pragma once

#include <string>
#include <vector>

namespace glpp::text {

class charset_t {
public:
	using char_t = char32_t;

	charset_t() = default;

	template <class... Args>
	charset_t(Args... args) :
		m_chars(std::forward<Args>(args)...)
	{}

	void populate();

	std::vector<char_t>::const_iterator begin() const;
	std::vector<char_t>::const_iterator end() const;

	static charset_t digits();
	static charset_t ascii();
	static charset_t all();

private:
	std::vector<char_t> m_chars;
};

}
