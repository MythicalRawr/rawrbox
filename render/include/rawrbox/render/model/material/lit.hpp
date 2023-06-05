#pragma once

#include <rawrbox/render/model/material/base.hpp>

namespace rawrbox {
	class MaterialLit : public rawrbox::MaterialBase {
	public:
		bgfx::UniformHandle s_texSpecularColor = BGFX_INVALID_HANDLE;
		bgfx::UniformHandle s_texEmissionColor = BGFX_INVALID_HANDLE;
		bgfx::UniformHandle s_texOpacityColor = BGFX_INVALID_HANDLE;

		bgfx::UniformHandle u_specularColor = BGFX_INVALID_HANDLE;
		bgfx::UniformHandle u_emissionColor = BGFX_INVALID_HANDLE;

		bgfx::UniformHandle u_cameraPos = BGFX_INVALID_HANDLE;
		bgfx::UniformHandle u_texMatData = BGFX_INVALID_HANDLE;

		bgfx::UniformHandle u_lightsSetting = BGFX_INVALID_HANDLE;
		bgfx::UniformHandle u_lightsPosition = BGFX_INVALID_HANDLE;
		bgfx::UniformHandle u_lightsData = BGFX_INVALID_HANDLE;

		using vertexBufferType = rawrbox::VertexLitData;

		MaterialLit() = default;
		MaterialLit(MaterialLit&&) = delete;
		MaterialLit& operator=(MaterialLit&&) = delete;
		MaterialLit(const MaterialLit&) = delete;
		MaterialLit& operator=(const MaterialLit&) = delete;
		~MaterialLit() override;

		void registerUniforms() override;
		void preProcess(const rawrbox::Vector3f& camPos) override;

		template <typename T>
		void process(std::shared_ptr<rawrbox::Mesh<T>> mesh) {
			rawrbox::MaterialBase::process(mesh);

			if (mesh->specularTexture != nullptr && mesh->specularTexture->valid() && !mesh->wireframe) {
				bgfx::setTexture(1, s_texSpecularColor, mesh->specularTexture->getHandle());
			} else {
				bgfx::setTexture(1, s_texSpecularColor, rawrbox::BLACK_TEXTURE->getHandle());
			}

			if (mesh->emissionTexture != nullptr && mesh->emissionTexture->valid() && !mesh->wireframe) {
				bgfx::setTexture(2, s_texEmissionColor, mesh->emissionTexture->getHandle());
			} else {
				bgfx::setTexture(2, s_texEmissionColor, rawrbox::BLACK_TEXTURE->getHandle());
			}

			if (mesh->opacityTexture != nullptr && mesh->opacityTexture->valid() && !mesh->wireframe) {
				bgfx::setTexture(3, s_texOpacityColor, mesh->opacityTexture->getHandle());
			} else {
				bgfx::setTexture(3, s_texOpacityColor, rawrbox::WHITE_TEXTURE->getHandle());
			}

			bgfx::setUniform(u_specularColor, mesh->specularColor.data().data());
			bgfx::setUniform(u_emissionColor, mesh->emissionColor.data().data());

			std::array<float, 2> matData = {mesh->specularShininess, mesh->emissionIntensity};
			bgfx::setUniform(u_texMatData, matData.data());
		}

		void upload() override;
	};
} // namespace rawrbox
