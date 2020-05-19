#include "glpp/text/charset.hpp"

#include <numeric>
#include <stdexcept>
#include <limits>

namespace glpp::text {

void charset_t::populate() {
	if(m_chars.size() == 0) {
		throw std::runtime_error("Automatic charset is not implemented yet. Use charset::asci() or initialise with the characters you want to use.");
	}
}

std::vector<charset_t::char_t>::const_iterator charset_t::begin() const {
	return m_chars.begin();
}

std::vector<charset_t::char_t>::const_iterator charset_t::end() const {
	return m_chars.end();
}

charset_t charset_t::digits() {
	return std::initializer_list<char_t>{L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9' };
}

charset_t charset_t::ascii() {
	constexpr size_t total =
		static_cast<size_t>(std::numeric_limits<char>::max()) -
		static_cast<size_t>(std::numeric_limits<char>::min()) +
		1;

	decltype(m_chars) init(total);
	std::iota(init.begin(), init.end(), std::numeric_limits<char_t>::min());
	return charset_t(std::move(init));
}

charset_t charset_t::all() {
	return charset_t();
}

}
