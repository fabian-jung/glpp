#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>

namespace glpp::render {

template <class Attribute_Description>
using model_t = std::vector<Attribute_Description>;

template <class Model>
class model_traits {};

template <class Attribute_Description>
struct model_traits<model_t<Attribute_Description>> {

	using attribute_description_t = Attribute_Description;

	constexpr static size_t buffer_count() { return 1; }
	constexpr static bool instanced() { return false; }

	constexpr static  const model_t<Attribute_Description>& verticies(const model_t<Attribute_Description>& model) {
		return model;
	}
};

template <class Attribute_Description, class Index = GLuint>
struct indexed_model_t {
	model_t<Attribute_Description> verticies;
	std::vector<Index> indicies;
};

template <class Attribute_Description>
model_traits(const model_t<Attribute_Description>&) -> model_traits<model_t<Attribute_Description>>;

template <class Attribute_Description, class Index>
struct model_traits<indexed_model_t<Attribute_Description, Index>> {

	using attribute_description_t = Attribute_Description;

	constexpr static size_t buffer_count() { return 1; }
	constexpr static bool instanced() { return true; }

	constexpr static const model_t<Attribute_Description>& verticies(const indexed_model_t<Attribute_Description, Index>& model) {
		return model.verticies;
	}

	constexpr static const auto& indicies(const indexed_model_t<Attribute_Description, Index>& model) {
		return model.indicies;
	}
};

template <class Attribute_Description, class Index>
model_traits(const indexed_model_t<Attribute_Description, Index>&) -> model_traits<indexed_model_t<Attribute_Description, Index>>;


}
