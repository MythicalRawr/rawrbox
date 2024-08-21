#pragma once

#include <rawrbox/gltf/importer.hpp>
#include <rawrbox/gltf/utils/model.hpp>
#include <rawrbox/render/lights/directional.hpp>
#include <rawrbox/render/lights/point.hpp>
#include <rawrbox/render/lights/spot.hpp>
#include <rawrbox/render/models/model.hpp>

namespace rawrbox {
	template <typename M = MaterialUnlit>
		requires(std::derived_from<M, rawrbox::MaterialBase>)
	class GLTFModel : public rawrbox::Model<M> {
	protected:
		// INTERNAL -------
		void loadMeshes(const rawrbox::GLTFImporter& model) {
			for (size_t i = 0; i < model.meshes.size(); i++) {
				this->addMesh(rawrbox::GLTFUtils::extractMesh<M>(model, i));
			}
		}

		void loadAnimations(const rawrbox::GLTFImporter& model) {
			/*this->_animations = model.animations;
			this->_animatedMeshes.clear();

			// Mark animated meshes ---
			for (const auto& anim : model.animatedMeshes) {
				auto fnd = std::find_if(this->_meshes.begin(), this->_meshes.end(), [anim](std::unique_ptr<rawrbox::Mesh<typename M::vertexBufferType>>& msh) {
					return msh->getName() == anim.second->name;
				});
				if (fnd == this->_meshes.end()) continue;

				(*fnd)->setOptimizable(false);
				this->_animatedMeshes[anim.first] = (*fnd).get();
			}
			// -----------------------*/
		}

		void loadBlendShapes(const rawrbox::GLTFImporter& model) {
			/*this->_blend_shapes.clear();

			for (const auto& blend : model.blendShapes) {
				if (blend.second.mesh_index >= this->_meshes.size()) {
					this->_logger->warn("Failed to find mesh {}", blend.second.mesh_index);
					continue;
				}

				auto s = std::make_unique<rawrbox::BlendShapes<M>>();
				s->normals = blend.second.norms;
				s->pos = blend.second.pos;
				s->weight = blend.second.weight;

				s->mesh = this->_meshes[blend.second.mesh_index].get();
				s->mesh->setOptimizable(false);

				this->_blend_shapes[blend.first] = std::move(s);
			}*/
		}

		void loadLights(const rawrbox::GLTFImporter& model) {
			/*for (const auto& assimpLights : model.lights) {
				rawrbox::LightBase* light = nullptr;

				switch (assimpLights.type) {
					case rawrbox::LightType::POINT:
						light = this->template addLight<rawrbox::PointLight>(assimpLights.parentID, assimpLights.pos, assimpLights.diffuse, 10.F); // TODO: BROKEN
						break;
					case rawrbox::LightType::SPOT:
						light = this->template addLight<rawrbox::SpotLight>(assimpLights.parentID, assimpLights.pos, assimpLights.direction, assimpLights.diffuse, assimpLights.angleInnerCone, assimpLights.angleOuterCone, 10.F); // TODO: BROKEN
						break;
					case rawrbox::LightType::DIR:
						light = this->template addLight<rawrbox::DirectionalLight>(assimpLights.parentID, assimpLights.pos, assimpLights.direction, assimpLights.diffuse);
						break;

					default:
					case rawrbox::LightType::UNKNOWN:
						this->_logger->warn("Failed to create unknown light '{}'", assimpLights.name);
						break;
				}

				if (light != nullptr) light->setIntensity(assimpLights.intensity);
			}*/
		}
		// -------------------

	public:
		GLTFModel() = default;
		GLTFModel(const GLTFModel&) = delete;
		GLTFModel(GLTFModel&&) = delete;
		GLTFModel& operator=(const GLTFModel&) = delete;
		GLTFModel& operator=(GLTFModel&&) = delete;
		~GLTFModel() override = default;

		void load(const rawrbox::GLTFImporter& model) {
			this->loadMeshes(model);
			this->loadBlendShapes(model);

			if constexpr (supportsBones<typename M::vertexBufferType>) {
				this->loadAnimations(model);
			}

			if constexpr (supportsNormals<typename M::vertexBufferType>) {
				this->loadLights(model);
			}
		}
	};
} // namespace rawrbox
