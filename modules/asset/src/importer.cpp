#include "glpp/asset/scene.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>  
#include <assimp/Importer.hpp>

namespace glpp::asset {

void traverse_scene_graph(
    const aiScene* scene, 
    const aiNode* node, 
    glm::mat4 old,
    std::vector<mesh_t>& meshes
);

struct light_cast_t {
	const aiScene* scene;
	const aiLight* light;

	template <class T>
	bool has_type() const;
		
	template <class T>
	T cast() const;

	private:
	template <class T>
	void check_type() const;
};

scene_t::scene_t(const char* file_name) {
    Assimp::Importer importer;
    auto scene = importer.ReadFile(file_name, aiProcess_Triangulate);
    if(!scene) {
        throw std::runtime_error(std::string("Could not open ")+file_name+".");
    }

    traverse_scene_graph(scene, scene->mRootNode, glm::mat4(1.0f), meshes);

	std::for_each_n(scene->mLights, scene->mNumLights, [this, scene](const aiLight* light) {
		light_cast_t light_cast{ scene, light };
		if(light_cast.has_type<ambient_light_t>()) {
			ambient_lights.emplace_back(light_cast.cast<ambient_light_t>());
		} else if(light_cast.has_type<directional_light_t>()) {
			directional_lights.emplace_back(light_cast.cast<directional_light_t>());
		} else if(light_cast.has_type<spot_light_t>()) {
			spot_lights.emplace_back(light_cast.cast<spot_light_t>());
		} else if(light_cast.has_type<point_light_t>()) {
			point_lights.emplace_back(light_cast.cast<point_light_t>());
		}
	});
}

mesh_t mesh_from_assimp(const aiMesh* aiMesh, const glm::mat4& model_matrix) {
    mesh_t mesh;
    using vertex_description_t = mesh_t::vertex_description_t;
    for(auto i = 0u; i < aiMesh->mNumVertices; ++i) {
        const auto pos = aiMesh->mVertices[i];
        const auto normal = aiMesh->mNormals[i];
        aiVector3D tex;
       
        if(aiMesh->GetNumUVChannels() > 0) {
            tex = aiMesh->mTextureCoords[0][i];
        }
        mesh.model.verticies.emplace_back(vertex_description_t{
            glm::vec3{ pos.x, pos.y, pos.z },
            glm::vec3{ normal.x, normal.y, normal.z },
            glm::vec2{ tex.x, tex.y },
        });
    }

    std::for_each_n(
        aiMesh->mFaces, aiMesh->mNumFaces, [&](const aiFace& face){
            std::copy_n(face.mIndices, face.mNumIndices, std::back_inserter(mesh.model.indicies));
        }
    );
  
    mesh.model_matrix = model_matrix;

    mesh.material_index = static_cast<unsigned>(-1);
    return mesh;
}

void traverse_scene_graph(
    const aiScene* scene, 
    const aiNode* node, 
    glm::mat4 old,
    std::vector<mesh_t>& meshes
) {
	glm::mat4 transformation = glm::transpose(glm::make_mat4(&(node->mTransformation.a1)))*old;
	std::for_each_n(node->mMeshes, node->mNumMeshes, [&](unsigned int meshId){
		const auto* mesh = scene->mMeshes[meshId];
		meshes.emplace_back(
            mesh_from_assimp(mesh, transformation)
        );
	});

    std::for_each_n(node->mChildren, node->mNumChildren, [&](const aiNode* next) {
        traverse_scene_graph(scene, next, transformation, meshes);
    });
}

aiMatrix4x4 get_transformation(const aiNode* node) {
	aiMatrix4x4 transform;
	do {
		const auto node_transform = node->mTransformation;
		transform = transform*node_transform;
		node = node->mParent;
	} while(node);
	return transform;
}


template <class T>
bool light_cast_t::has_type() const {
	if constexpr(std::is_same_v<T, ambient_light_t>) {
		return light->mType == aiLightSourceType::aiLightSource_AMBIENT;
	}
	if constexpr(std::is_same_v<T, directional_light_t>) {
		return light->mType == aiLightSourceType::aiLightSource_DIRECTIONAL;
	}
	if constexpr(std::is_same_v<T, point_light_t>) {
		return light->mType == aiLightSourceType::aiLightSource_POINT;
	}
	if constexpr(std::is_same_v<T, spot_light_t>) {
		return light->mType == aiLightSourceType::aiLightSource_SPOT;
	}
	return false;
}
		
template <class T>
T light_cast_t::cast() const {
	check_type<T>();
	const glm::vec3 ambient { light->mColorAmbient.r, light->mColorAmbient.g, light->mColorAmbient.b };
	const glm::vec3 diffuse  { light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b };
	const glm::vec3 specular { light->mColorSpecular.r, light->mColorSpecular.g, light->mColorSpecular.b };
	if constexpr(std::is_same_v<T, ambient_light_t>) {
		return ambient_light_t{
			ambient,
			diffuse,
			specular
		};
	}
	const glm::vec3 direction { light->mDirection.x, light->mDirection.y, light->mDirection.z };
	if constexpr(std::is_same_v<T, directional_light_t>) {
		return directional_light_t {
			direction,
			ambient,
			diffuse,
			specular
		};
	}
	const auto transform = get_transformation(scene->mRootNode->FindNode(light->mName));
	const auto ai_position = transform*light->mPosition;
	const glm::vec3 position { ai_position.x, ai_position.y, ai_position.z };
	const float attenuation_constant = light->mAttenuationConstant;
	const float attenuation_linear = light->mAttenuationLinear;
	const float attenuation_quadratic = light->mAttenuationQuadratic;
	if constexpr(std::is_same_v<T, point_light_t>) {
		return point_light_t {
			position,
			ambient,
			diffuse,
			specular,
			attenuation_constant,
			attenuation_linear,
			attenuation_quadratic
		};
	}
	float inner_cone = light->mAngleInnerCone;
	float outer_cone = light->mAngleOuterCone;
	if constexpr(std::is_same_v<T, spot_light_t>) {
		return spot_light_t {
			position,
			direction,
			ambient,
			diffuse,
			specular,
			inner_cone,
			outer_cone,
			attenuation_constant,
			attenuation_linear,
			attenuation_quadratic
		};
	}
}

template <class T>
void light_cast_t::check_type() const {
	if(!has_type<T>()) {
		throw std::runtime_error("Light type missmatch");
	}
}

}

// namespace detail {

// void insert_sorted(std::vector<std::string>& vector, std::string str) {
// 	auto pos = std::lower_bound(vector.begin(), vector.end(), str);
// 	if(*pos != str) {
// 		vector.insert(pos, std::move(str));
// 	}
// }

// void collect_textures(aiScene*  scene, texture_store_t& textures) {
// 	constexpr std::array<aiTextureType, 4> texture_keys {
// 		aiTextureType_EMISSIVE,
// 		aiTextureType_AMBIENT,
// 		aiTextureType_DIFFUSE,
// 		aiTextureType_SPECULAR
// 	};

// 	std::for_each_n(
// 		scene->mMaterials,
// 		scene->mNumMaterials,
// 		[&](const aiMaterial* material) {
// 			for(auto key : texture_keys) {
// 				const auto count = material->GetTextureCount(key);
// 				for(auto i = 0u; i < count; ++i) {
// 					aiString file;
// 					material->GetTexture(key, i, &file, nullptr, nullptr, nullptr, nullptr, nullptr);
// 					textures.insert(file.C_Str());
// 				}
// 			}
// 		}
// 	);
// }

// }

// importer_t::importer_t(const std::string& file,  material_map_t material_map, material_policy_t material_policy) :
// 	m_scene(m_importer.ReadFile(file, aiProcess_Triangulate)),
// 	m_material_map(material_map),
// 	m_material_policy(material_policy),
// 	m_textures()
// {	
// 	if(!m_scene) {
// 		throw std::runtime_error(m_importer.GetErrorString());
// 	}
// }

// scene_t importer_t::import() const {
// 	scene_t result;
// 	import(result);
// 	return result;
// }

// void importer_t::import(scene_t& scene) const {
// 	// TODO
// 	// std::vector<mesh_t> meshes;
// 	// std::vector<std::string> textures;
// 	scene.textures = m_textures;
// 	// std::vector<material_t> materials;

// 	// std::vector<ambient_light_t> ambient_lights;
// 	// std::vector<directional_light_t> directional_lights;
// 	// std::vector<point_light_t> point_lights;
// 	// std::vector<spot_light_t> spot_lights;
// }

// const texture_store_t& importer_t::textures() const {
// 	return m_textures;
// }

// std::vector<mesh_t> importer_t::meshes() const {
// 	std::vector<mesh_t> meshes;
// 	detail::traverse_scene_graph(m_scene, m_scene->mRootNode, glm::mat4(1.0f), meshes);
// 	return meshes;
// }

// std::vector<material_t> importer_t::materials() const {
// 	std::vector<material_t> result;
// 	std::transform(
// 		m_scene->mMaterials,
// 		m_scene->mMaterials+m_scene->mNumMaterials,
// 		std::back_inserter(result),
// 		[&](const aiMaterial* m){
// 			material_t native_material(m);
// 			auto material_library_it = m_material_map.find(native_material.name);
// 			if(material_library_it==m_material_map.end()) {
// 				switch(m_material_policy) {
// 					case material_policy_t::augment:
// 						return native_material;
// 					case material_policy_t::replace:
// 						throw std::runtime_error("Try to load material \""+native_material.name+"\", that is not in material_map.");
// 				}
// 			}
// 			return material_library_it->second;
// 		}
// 	);
// 	return result;
// }

// std::vector<material_t> importer_t::materials(std::ostream& logger) const {
// 	std::vector<material_t> result;
// 	std::transform(
// 		m_scene->mMaterials,
// 		m_scene->mMaterials+m_scene->mNumMaterials,
// 		std::back_inserter(result),
// 		[&](const aiMaterial* m){
// 			material_t native_material(m, logger);
// 			auto material_library_it = m_material_map.find(native_material.name);
// 			if(material_library_it==m_material_map.end()) {
// 				switch(m_material_policy) {
// 					case material_policy_t::augment:
// 						return native_material;
// 					case material_policy_t::replace:
// 						throw std::runtime_error("Try to load material, that is not in material_map.");
// 				}
// 			}
// 			return material_library_it->second;
// 		}
// 	);
// 	return result;
// }

// std::vector<render::camera_t> importer_t::cameras() const {
// 	std::vector<render::camera_t> cameras;
// 	cameras.reserve(m_scene->mNumCameras);
// 	std::transform(m_scene->mCameras, m_scene->mCameras+m_scene->mNumCameras, std::back_inserter(cameras), [&](const aiCamera* cam){
// 		aiMatrix4x4 transform = detail::get_transformation(m_scene->mRootNode->FindNode(cam->mName));

// 		auto aiPos = transform*cam->mPosition;
// 		const glm::vec3 position { aiPos.x,  aiPos.y,  aiPos.z };
// 		auto aiLookAt = transform*cam->mLookAt;
// 		const glm::vec3 look_at { aiLookAt.x,  aiLookAt.y,  aiLookAt.z };
// 		auto aiUp = transform*cam->mUp;
// 		const glm::vec3 up { aiUp.x,  aiUp.y,  aiUp.z };
// 		return render::camera_t(
// 			position,
// 			look_at,
// 			up,
// 			glm::degrees(cam->mHorizontalFOV),
// 			cam->mClipPlaneNear,
// 			cam->mClipPlaneFar,
// 			cam->mAspect
// 		);
// 	});
// 	return cameras;
// }

// namespace detail {
// }

// template <class T>
// std::vector<T> importer_t::lights() const {
// 	std::vector<T> result;
// 	std::for_each(m_scene->mLights, m_scene->mLights+m_scene->mNumLights, [&](const auto light) {
// 		detail::light_cast_t caster{ m_scene, light };
// 		if(caster.has_type<T>()) {
// 			result.emplace_back(caster.cast<T>());
// 		}
// 	});
// 	return result;
// }

// template std::vector<ambient_light_t> importer_t::lights<ambient_light_t>() const;
// template std::vector<directional_light_t> importer_t::lights<directional_light_t>() const;
// template std::vector<point_light_t> importer_t::lights<point_light_t>() const;
// template std::vector<spot_light_t> importer_t::lights<spot_light_t>() const;

// std::vector<any_light_t> importer_t::all_lights() const {
// 	std::vector<any_light_t> result;
// 	std::for_each(m_scene->mLights, m_scene->mLights+m_scene->mNumLights, [&](const auto light) {
// 		detail::light_cast_t caster { m_scene, light };
// 		if(caster.has_type<ambient_light_t>()) {
// 			result.emplace_back(caster.cast<ambient_light_t>());
// 		} else if(caster.has_type<directional_light_t>()) {
// 			result.emplace_back(caster.cast<directional_light_t>());
// 		} else if(caster.has_type<point_light_t>()) {
// 			result.emplace_back(caster.cast<point_light_t>());
// 		} else if(caster.has_type<spot_light_t>()) {
// 			result.emplace_back(caster.cast<spot_light_t>());
// 		}
// 	});
// 	return result;
// }

// }