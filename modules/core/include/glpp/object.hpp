#pragma once

#include "glpp.hpp"

namespace glpp {

template <class destructor_t = void (*)(GLuint)>
class object_t {
public:

	object_t(GLuint id, destructor_t destroy);
	~object_t();

	object_t(object_t&& mov) noexcept;
	object_t& operator=(object_t&& mov) noexcept;

	object_t(const object_t& cpy) = delete;
	object_t& operator=(const object_t& cpy) = delete;

	GLuint id() const noexcept;

private:
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
) :
	m_id(id),
	m_destroy(destroy)
{

}

template <class destructor_t>
object_t<destructor_t>::~object_t() {
	m_destroy(m_id);
}

template <class destructor_t>
object_t<destructor_t>::object_t(object_t&& mov) noexcept:
	m_id(mov.m_id),
	m_destroy(mov.m_destroy)
{
	mov.m_id = 0;
}

template <class destructor_t>
object_t<destructor_t>& object_t<destructor_t>::operator=(object_t&& mov) noexcept {
	m_id = mov.m_id;
	m_destroy = mov.m_destroy;
	mov.m_id = 0;
	return *this;
}

template <class destructor_t>
GLuint object_t<destructor_t>::id() const noexcept {
	return m_id;
}

}
