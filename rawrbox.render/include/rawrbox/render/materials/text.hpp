#pragma once

#include <rawrbox/render/materials/unlit.hpp>

namespace rawrbox {
	struct MaterialTextUniforms {
		rawrbox::Vector4f _gBillboard;
	};

	class MaterialText3D : public rawrbox::MaterialUnlit {
		static bool _build;

		static Diligent::RefCntAutoPtr<Diligent::IBuffer> _uniforms;
		static Diligent::RefCntAutoPtr<Diligent::IBuffer> _uniforms_pixel;

	public:
		using vertexBufferType = rawrbox::VertexData;

		MaterialText3D() = default;
		MaterialText3D(const MaterialText3D&) = delete;
		MaterialText3D(MaterialText3D&&) = delete;
		MaterialText3D& operator=(const MaterialText3D&) = delete;
		MaterialText3D& operator=(MaterialText3D&&) = delete;
		~MaterialText3D() override = default;

		void init() override;
		void createUniforms() override;

		template <typename T = rawrbox::VertexData>
		void bindUniforms(const rawrbox::Mesh<T>& mesh) {
			auto renderer = rawrbox::RENDERER;
			auto context = renderer->context();

			// SETUP VERTEX UNIFORMS ----------------------------
			{
				Diligent::MapHelper<rawrbox::MaterialTextUniforms> CBConstants(context, this->_uniforms, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
				CBConstants->_gBillboard = mesh.getData("billboard_mode");
			}
			// -----------

			// SETUP PIXEL UNIFORMS ----------------------------
			{
				Diligent::MapHelper<rawrbox::MaterialBasePixelUniforms> CBConstants(context, this->_uniforms_pixel, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
				this->bindBasePixelUniforms<T, rawrbox::MaterialBasePixelUniforms>(mesh, CBConstants);
			}
			// -----------

			// Bind ---
			rawrbox::PipelineUtils::signatureBind->GetVariableByName(Diligent::SHADER_TYPE_VERTEX, "Constants")->Set(this->_uniforms);
			rawrbox::PipelineUtils::signatureBind->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, "Constants")->Set(this->_uniforms_pixel);
			// --------
		}
	};

} // namespace rawrbox
