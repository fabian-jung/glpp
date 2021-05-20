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

struct parse_texture_stack_t {
	const aiMaterial* material;
	scene_t& scene;

	using texture_stack_description_t = std::pair<aiTextureType , texture_stack_t material_t::*>;

	glpp::asset::op_t convert(aiTextureOp op) const;

	void operator()(const texture_stack_description_t channel_desc);
};

aiMatrix4x4 get_transformation(const aiNode* node);

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

	materials.reserve(scene->mNumMaterials);
	std::for_each_n(
		scene->mMaterials,
		scene->mNumMaterials,
		[&](const aiMaterial* material) {
			aiString name;
			const auto success = material->Get(AI_MATKEY_NAME, name);
			if(aiReturn_SUCCESS == success) {
				aiColor3D diffuse, specular, ambient, emissive;
				material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
				material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
				material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
				material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
				float shininess;
				material->Get(AI_MATKEY_SHININESS, shininess);
				materials.emplace_back(material_t{
					name.C_Str(),
					glm::vec3(diffuse.r, diffuse.g, diffuse.b),
					{},
					glm::vec3(emissive.r, emissive.g, emissive.b),
					{},
					glm::vec3(ambient.r, ambient.g, ambient.b),
					{},
					glm::vec3(specular.r, specular.g, specular.b),
					{},
					shininess
				});

				constexpr std::array channels { 
					std::make_pair(aiTextureType_DIFFUSE, &material_t::diffuse_textures), 
					std::make_pair(aiTextureType_EMISSIVE, &material_t::emissive_textures),
					std::make_pair(aiTextureType_AMBIENT, &material_t::ambient_textures), 
					std::make_pair(aiTextureType_SPECULAR, &material_t::specular_textures)
				};
				std::for_each(channels.begin(), channels.end(), parse_texture_stack_t{ material, *this});
			}
		}
	);

	cameras.reserve(scene->mNumCameras);
	std::transform(scene->mCameras, scene->mCameras+scene->mNumCameras, std::back_inserter(cameras), [&](const aiCamera* cam){
		aiMatrix4x4 transform = get_transformation(scene->mRootNode->FindNode(cam->mName));

		auto aiPos = transform*cam->mPosition;
		const glm::vec3 position { aiPos.x,  aiPos.y,  aiPos.z };
		auto aiLookAt = transform*cam->mLookAt;
		const glm::vec3 look_at { aiLookAt.x,  aiLookAt.y,  aiLookAt.z };
		auto aiUp = transform*cam->mUp;
		const glm::vec3 up { aiUp.x,  aiUp.y,  aiUp.z };
		return core::render::camera_t(
			position,
			look_at,
			up,
			glm::degrees(cam->mHorizontalFOV),
			cam->mClipPlaneNear,
			cam->mClipPlaneFar,
			cam->mAspect
		);
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

glpp::asset::op_t parse_texture_stack_t::convert(aiTextureOp op) const {
	switch(op) {
		case aiTextureOp::aiTextureOp_Add:
			return glpp::asset::op_t::addition;
		case aiTextureOp::aiTextureOp_Divide:
			return glpp::asset::op_t::division;
		case aiTextureOp::aiTextureOp_Multiply:
			return glpp::asset::op_t::multiplication;
		case aiTextureOp::aiTextureOp_SignedAdd:
			return glpp::asset::op_t::signed_addition;
		case aiTextureOp::aiTextureOp_SmoothAdd:
			return glpp::asset::op_t::smooth_addition;
		case aiTextureOp::aiTextureOp_Subtract:
			return glpp::asset::op_t::addition;
		default:
			throw std::runtime_error("Could not convert texture op");
	}
}

void parse_texture_stack_t::operator()(const texture_stack_description_t channel_desc) {
	auto [channel, corresponding_texture_stack] = channel_desc;
	const auto stack_size = material->GetTextureCount(aiTextureType_DIFFUSE);
	for(auto i = 0u; i < stack_size; ++i) {
		float strength = 1.0f;
		aiString path;
		aiTextureOp op = aiTextureOp_Add;
		material->GetTexture(channel, i, &path, nullptr, nullptr, &strength, &op, nullptr);

		std::string std_path{ path.C_Str() };
		const auto it = std::find(scene.textures.begin(), scene.textures.end(), std_path);
		const size_t key = std::distance(scene.textures.begin(), it);
		if(it == scene.textures.end()) {
			scene.textures.emplace_back(std::move(std_path));
		}
		
		const auto glpp_op = convert(op);
		if(op == aiTextureOp_Subtract) {
			strength *= -1.0;
		}
		(scene.materials.back().*corresponding_texture_stack).emplace_back(
			texture_stack_entry_t{
				key,
				strength,
				glpp_op
			}
		);
	}
}

}