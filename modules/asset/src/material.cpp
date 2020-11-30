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

std::ostream& operator<<(std::ostream& lhs, const glpp::asset::material_t::op_t& rhs) {
	switch(rhs) {
		case glpp::asset::material_t::op_t::addition:
			lhs << "ADD";
			break;
		case glpp::asset::material_t::op_t::multiplication:
			lhs << "MUL";
			break;
		case glpp::asset::material_t::op_t::division:
			lhs << "DIV";
			break;
		case glpp::asset::material_t::op_t::smooth_addition:
			lhs << "SMOOTH_ADD";
			break;
		case glpp::asset::material_t::op_t::signed_addition:
			lhs << "SIGNED_ADD";
			break;
	}
	return lhs;
}

namespace glpp::asset {

template <class T, class... Args>
T get(const aiMaterial* material, Args... args) {
	T value;
	material->Get(args..., value);
	return value;
}

glm::vec3 glm_cast(const aiColor3D& ai_color) {
	return glm::vec3{ ai_color.r, ai_color.g, ai_color.b };
}

namespace detail {
	auto texture_stack_from_key(const aiMaterial* material, const aiTextureType key, const texture_store_t& texture_storage) {
		const auto count = material->GetTextureCount(key);
		material_t::texture_stack_t result;
		std::generate_n(std::back_inserter(result), count, [index = 0, key, material, texture_storage]() mutable {
			ai_real strength = 1.0f;
			aiString file;
			aiTextureOp op;
			material->GetTexture(key, index, &file, nullptr, nullptr, &strength, &op, nullptr);
			++index;

			material_t::op_t cop = material_t::op_t::addition;
			switch(op) {
				case aiTextureOp_Subtract:
					strength *= -1.0f;
					break;
				case aiTextureOp_Multiply:
					cop = material_t::op_t::multiplication;
					break;
				case aiTextureOp_Divide:
					cop = material_t::op_t::division;
					break;
				case aiTextureOp_SmoothAdd:
					cop = material_t::op_t::smooth_addition;
					break;
				case aiTextureOp_SignedAdd:
					cop = material_t::op_t::signed_addition;
					break;
				default:
					break;
			}

			const auto id = texture_storage.get(file.C_Str());
			return material_t::texture_stack_entry_t {
				id,
				texture_storage.get_filename_view(id),
				strength,
				cop
			};
		});
		return result;
	}
}

material_t::material_t(const aiMaterial* material, const texture_store_t& texture_storage) :
	name(get<aiString>(material, AI_MATKEY_NAME).C_Str()),
	diffuse(glm_cast(get<aiColor3D>(material, AI_MATKEY_COLOR_DIFFUSE))),
	diffuse_textures(detail::texture_stack_from_key(material, aiTextureType_DIFFUSE)),
	emissive(glm_cast(get<aiColor3D>(material, AI_MATKEY_COLOR_EMISSIVE))),
	emissive_textures(detail::texture_stack_from_key(material, aiTextureType_EMISSIVE)),
	ambient(glm_cast(get<aiColor3D>(material, AI_MATKEY_COLOR_AMBIENT))),
	ambient_textures(detail::texture_stack_from_key(material, aiTextureType_AMBIENT)),
	specular(glm_cast(get<aiColor3D>(material, AI_MATKEY_COLOR_SPECULAR))),
	specular_textures(detail::texture_stack_from_key(material, aiTextureType_SPECULAR)),
	shininess(get<float>(material, AI_MATKEY_SHININESS))
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
