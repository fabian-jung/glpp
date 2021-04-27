#pragma once

#include "glpp.hpp"
#include <cassert>
namespace glpp::core {

template <class destructor_t = void (*)(GLuint)>
class object_t {
public:

	object_t(GLuint id, destructor_t destroy) noexcept;
	~object_t();

	object_t(object_t&& mov) noexcept;
	object_t& operator=(object_t&& mov) noexcept;

	object_t(const object_t& cpy) = delete;
	object_t& operator=(const object_t& cpy) = delete;

	GLuint id() const noexcept;

private:
	bool m_valid = false;
	GLuint m_id = 0;
	destructor_t m_destroy = nullptr;
};

/*
 * Implementation
 */

template <class destructor_t>
object_t<destructor_t>::object_t(
	GLuint id,
	destructor_t destroy
) noexcept :
	m_valid(true),
	m_id(id),
	m_destroy(destroy)
{}

template <class destructor_t>
object_t<destructor_t>::~object_t() {
	if(m_valid) {
		m_destroy(m_id);
	}
}

template <class destructor_t>
object_t<destructor_t>::object_t(object_t&& mov) noexcept :
	m_valid(mov.m_valid),
	m_id(mov.m_id),
	m_destroy(mov.m_destroy)
{
	mov.m_valid = false;
}

template <class destructor_t>
object_t<destructor_t>& object_t<destructor_t>::operator=(object_t&& mov) noexcept {
	m_valid = mov.m_valid;
	mov.m_valid = false;
	m_id = mov.m_id;
	m_destroy = mov.m_destroy;
	return *this;
}

template <class destructor_t>
GLuint object_t<destructor_t>::id() const noexcept {
	assert(m_valid);
	return m_id;
}

}
