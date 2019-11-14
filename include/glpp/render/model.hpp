#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>

namespace glpp::render {

template <class Attribute_Description>
class model_t {
public:
	using attribute_description_t = Attribute_Description;
	using const_pointer 			= const attribute_description_t*;

	model_t() = default;

	void add(attribute_description_t vertex);

	[[nodiscard]]
	size_t size() const noexcept;

	[[nodiscard]]
	const_pointer data() const noexcept;

protected:
	std::vector<attribute_description_t> m_verticies;
};

/*
 * Implementation
 */

template <class Attribute_Description>
void model_t<Attribute_Description>::add(attribute_description_t vertex) {
	m_verticies.emplace_back(std::move(vertex));
}

template <class Attribute_Description>
size_t model_t<Attribute_Description>::size() const noexcept {
	return m_verticies.size();
}

template <class Attribute_Description>
typename model_t<Attribute_Description>::const_pointer
model_t<Attribute_Description>::data() const noexcept {
	return m_verticies.data();
}

}
