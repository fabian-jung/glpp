#pragma once

#include <string>
#include <string_view>

namespace glpp::text::util {

template <class CharT>
class string_splitter_iterator {
public:
	string_splitter_iterator() = default;
	string_splitter_iterator(std::basic_string_view<CharT> string, CharT delim) :
	m_string(string),
	m_delim(delim)
	{}

	string_splitter_iterator& operator++() {
		const auto end = m_string.find(m_delim);
		if(end == m_string.npos) {
			m_string = m_string.substr(0,0);
		} else {
			m_string =  m_string.substr(end+1);
		}
		return *this;
	}

	bool operator==(const string_splitter_iterator& rhs) const {
		return m_string == rhs.m_string;
	}

	bool operator!=(const string_splitter_iterator& rhs) const {
		return m_string != rhs.m_string;
	}

	std::basic_string_view<CharT> operator*() {
		const auto end = m_string.find(m_delim);
		return m_string.substr(0, end);
	}

private:
	std::basic_string_view<CharT> m_string;
	const CharT m_delim = 0;
};

template <class CharT>
string_splitter_iterator(const std::basic_string_view<CharT>&, CharT) -> string_splitter_iterator<CharT>;

template <class CharT>
class string_splitter_t {
public:
	explicit string_splitter_t(const std::basic_string<CharT>& string, CharT delim) :
	m_string(string),
	m_delim(delim)
	{}

	explicit string_splitter_t(const std::basic_string_view<CharT>& string, CharT delim) :
	m_string(string),
	m_delim(delim)
	{}

	auto begin() {
		return string_splitter_iterator<CharT>(m_string, m_delim);
	}

	auto end() {
		return string_splitter_iterator<CharT>();
	}
private:
	std::basic_string_view<CharT> m_string;
	const CharT m_delim;
};

template <class CharT>
string_splitter_t(const std::basic_string<CharT>&, CharT) -> string_splitter_t<CharT>;

}
