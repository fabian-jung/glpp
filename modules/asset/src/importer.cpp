#include "glpp/asset/importer.hpp"

namespace glpp::asset {

importer_t::importer_t(const std::string& file) :
	scene(m_importer.ReadFile(file, aiProcess_Triangulate))
{
	if(!scene) {
		throw std::runtime_error(m_importer.GetErrorString());
	}
}

std::vector<mesh_t> importer_t::meshes() const {
	std::vector<mesh_t> meshes;
	meshes.reserve(scene->mNumMeshes);
	const auto begin = scene->mMeshes;
	const auto end = scene->mMeshes+scene->mNumMeshes;
	std::transform(begin, end, std::back_inserter(meshes), [](aiMesh* mesh){
		return mesh_t(mesh);
	});
	return meshes;
}

std::vector<material_t> importer_t::materials() const {
	std::vector<material_t> result;
	std::transform(
		scene->mMaterials,
		scene->mMaterials+scene->mNumMaterials,
		std::back_inserter(result), [](const auto* m){
			return material_t(m);
		}
	);
	return result;
}

std::vector<material_t> importer_t::materials(std::ostream& logger) const {
	std::vector<material_t> result;
	std::transform(
		scene->mMaterials,
		scene->mMaterials+scene->mNumMaterials,
		std::back_inserter(result), [&](const auto* m){
			return material_t(m, logger);
		}
	);
	return result;
}

std::vector<render::camera_t> importer_t::cameras() const {
	std::vector<render::camera_t> cameras;
	cameras.reserve(scene->mNumCameras);
	std::transform(scene->mCameras, scene->mCameras+scene->mNumCameras, std::back_inserter(cameras), [&](const aiCamera* cam){
		aiNode* cameraNode = scene->mRootNode->FindNode(cam->mName);
		auto transform = cameraNode->mTransformation;
		auto aiPos = transform*cam->mPosition;
		const glm::vec3 position { aiPos.x,  aiPos.y,  aiPos.z };
		auto aiLookAt = transform*cam->mLookAt;
		const glm::vec3 look_at { aiLookAt.x,  aiLookAt.y,  aiLookAt.z };
		auto aiUp = transform*cam->mUp;
		const glm::vec3 up { aiUp.x,  aiUp.y,  aiUp.z };
		return render::camera_t(
			position,
			look_at,
			up,
			glm::degrees(cam->mHorizontalFOV),
								cam->mClipPlaneNear,
						  cam->mClipPlaneFar,
						  cam->mAspect
		);
	});
	return cameras;
}

struct light_cast_t {
	const aiLight* light;

	template <class T>
	bool has_type() const {
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
	T cast() const {
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
		const glm::vec3 position { light->mPosition.x, light->mPosition.y, light->mPosition.z };
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

private:
	template <class T>
	void check_type() const {
		if(!has_type<T>()) {
			throw std::runtime_error("Light type missmatch");
		}
	}
};

template <class T>
std::vector<T> importer_t::lights() {
	std::vector<T> result;
	std::for_each(scene->mLights, scene->mLights+scene->mNumLights, [&](const auto light) {
		light_cast_t caster{ light };
		if(caster.has_type<T>()) {
			result.emplace_back(caster.cast<T>());
		}
	});
	return result;
}

std::vector<any_light_t> importer_t::all_lights() const {
	std::vector<any_light_t> result;
	std::for_each(scene->mLights, scene->mLights+scene->mNumLights, [&](const auto light) {
		light_cast_t caster { light };
		if(caster.has_type<ambient_light_t>()) {
			result.emplace_back(caster.cast<ambient_light_t>());
		} else if(caster.has_type<directional_light_t>()) {
			result.emplace_back(caster.cast<directional_light_t>());
		} else if(caster.has_type<point_light_t>()) {
			result.emplace_back(caster.cast<point_light_t>());
		} else if(caster.has_type<spot_light_t>()) {
			result.emplace_back(caster.cast<spot_light_t>());
		}
	});
	return result;
}

}
