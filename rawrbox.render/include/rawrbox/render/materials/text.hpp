#pragma once

#include <rawrbox/render/materials/unlit.hpp>

namespace rawrbox {
	struct MaterialTextUniforms {
		rawrbox::Vector4f _gBillboard;
	};

	class MaterialText3D : public rawrbox::MaterialUnlit {
		static Diligent::RefCntAutoPtr<Diligent::IBuffer> _uniforms;
		static bool _build;

	public:
		using vertexBufferType = rawrbox::VertexData;

		MaterialText3D() = default;
		MaterialText3D(const MaterialText3D&) = delete;
		MaterialText3D(MaterialText3D&&) = delete;
		MaterialText3D& operator=(const MaterialText3D&) = delete;
		MaterialText3D& operator=(MaterialText3D&&) = delete;
		~MaterialText3D() override = default;

		void init() override;

		template <typename T = rawrbox::VertexData>
		void bindUniforms(const rawrbox::Mesh<T>& mesh) {
			auto renderer = rawrbox::RENDERER;
			auto context = renderer->context();

			// SETUP UNIFORMS ----------------------------
			Diligent::MapHelper<rawrbox::MaterialTextUniforms> CBConstants(context, this->_uniforms, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
			CBConstants->_gBillboard = mesh.getData("billboard_mode");
		}

		template <typename T = rawrbox::VertexData>
		void bindTexture(const rawrbox::Mesh<T>& mesh) {
			if (this->_bind == nullptr) throw std::runtime_error("[RawrBox-MaterialText3D] Material not bound, did you call 'init'?");
			auto context = rawrbox::RENDERER->context();

			if (mesh.texture != nullptr && mesh.texture->isValid() && !mesh.wireframe) {
				mesh.texture->update(); // Update texture
				this->_bind->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, "g_Texture")->Set(mesh.texture->getHandle());
			} else {
				this->_bind->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, "g_Texture")->Set(rawrbox::WHITE_TEXTURE->getHandle());
			}
		}
	};

} // namespace rawrbox
