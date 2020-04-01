#include "glpp/text/charset.hpp"

#include <numeric>
#include <stdexcept>

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

charset_t charset_t::ascii() {
	decltype(m_chars) init(128);
	std::iota(init.begin(), init.end(), 0);
	return charset_t(std::move(init));
}

charset_t charset_t::all() {
	return charset_t();
}

}
