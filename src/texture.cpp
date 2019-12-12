#include "glpp/object/texture.hpp"
#include <algorithm>

namespace glpp::object {

texture_slot_t texture_t::bind_to_texture_slot() const {
	return texture_slot_t(*this);
}

GLuint texture_t::init() {
	GLuint tex;
	glpp::call(glCreateTextures, GL_TEXTURE_2D, 1, &tex);
	return tex;
}

void texture_t::destroy(GLuint id) {
	glpp::call(glDeleteTextures, 1, &id);
}

texture_slot_t::texture_slot_t(const texture_t& texture) :
	m_id(next_free_id())
{
	if(id() >= max_texture_units()) {
		throw std::runtime_error("Trying to bind texture unit to an invalid offset.");
	}
	units[id()] = texture.id();
	glpp::call(glBindTextureUnit, id(), texture.id());
}

texture_slot_t::texture_slot_t(texture_slot_t&& mov) = default;

texture_slot_t::~texture_slot_t() {
	units[id()] = -1;
}

GLint texture_slot_t::id() const {
	return m_id;
}

int texture_slot_t::next_free_id() {
	auto it = std::find(units.begin(), units.end(), -1);
	if(it == units.end()) {
		units.emplace_back(-1);
		it = units.end()-1;
	}
	return it - units.begin();
}

int max_texture_units_impl() {
	int i;
	glpp::call(glGetIntegerv, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &i);
	return i;
}

int texture_slot_t::max_texture_units() const {
	static const int units = max_texture_units_impl();
	return units;
}

std::vector<int> texture_slot_t::units;

} // End of namespace glpp::object
