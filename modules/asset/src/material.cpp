#include <glpp/asset/material.hpp>

#include <iterator>
#include <algorithm>
#include <ostream>
#include <memory>

std::ostream& operator<<(std::ostream& lhs, const aiMaterialProperty& property) {
	constexpr std::array<std::string_view, 6> types {
		"<unknown>",
		"float",
		"double",
		"string",
		"integer",
		"buffer"
	};

	lhs << "aiMaterialProperty{ key=" << property.mKey.C_Str() << ", type=" << types[property.mType] << ", data={ ";
	switch(property.mType) {
		case aiPTI_Float: {
			std::ostream_iterator<float> lhs_it(lhs, " ");
			std::copy_n(reinterpret_cast<float*>(property.mData), property.mDataLength/sizeof(float), lhs_it);
			break;
		}
		case aiPTI_Double: {
			std::ostream_iterator<double> lhs_it(lhs, " ");
			std::copy_n(reinterpret_cast<double*>(property.mData), property.mDataLength/sizeof(double), lhs_it);
			break;
		}
		case aiPTI_String: {
			std::ostream_iterator<char> lhs_it(lhs);
			std::copy_n(property.mData, property.mDataLength, lhs_it);
			break;
		}
		case aiPTI_Integer: {
			std::ostream_iterator<std::int32_t> lhs_it(lhs, " ");
			std::copy_n(property.mData, property.mDataLength/sizeof(std::int32_t), lhs_it);
			break;
		}
		case aiPTI_Buffer: {
			std::ostream_iterator<char> lhs_it(lhs);
			std::copy_n(property.mData, property.mDataLength, lhs_it);
			break;
		}
		default:
			lhs << "<unprintable>";
	}
	lhs << "}, length=" << property.mDataLength << ", index=" << property.mIndex << ", mSemantic=" << property.mSemantic << " }";
	return lhs;
}

namespace glpp::asset {

template <class T, class... Args>
T get(const aiMaterial* material, Args... args) {
	if constexpr(std::is_arithmetic_v<T>) {
		T value;
		material->Get(args..., &value);
		return value;
	} else {
		// For some unknown reason the Get-method takes a l-reference to a pointer.
		// Because of this it is not possible to pass a temporary of any sort.
		T* ptr = new T;
		material->Get(args..., ptr);
		T value(*ptr);
		delete ptr;
		return value;
	}
}

glm::vec3 glm_cast(const aiColor3D& ai_color) {
	return glm::vec3{ ai_color.r, ai_color.g, ai_color.b };
}


material_t::material_t(const aiMaterial* material) :
	name(get<aiString>(material, AI_MATKEY_NAME).C_Str()),
	diffuse(glm_cast(get<aiColor3D>(material, AI_MATKEY_COLOR_DIFFUSE))),
	emissive(glm_cast(get<aiColor3D>(material, AI_MATKEY_COLOR_EMISSIVE))),
	ambient(glm_cast(get<aiColor3D>(material, AI_MATKEY_COLOR_AMBIENT))),
	specular(glm_cast(get<aiColor3D>(material, AI_MATKEY_COLOR_SPECULAR)))
{
}

material_t::material_t(const aiMaterial* material, std::ostream& logger) :
	material_t(material)
{
	logger << "aiMaterial{ \n";
	std::for_each_n(material->mProperties, material->mNumProperties, [&](const auto* property) {
		logger << "	" << *property << "\n";
	});
	logger << "}\n";
}



}
