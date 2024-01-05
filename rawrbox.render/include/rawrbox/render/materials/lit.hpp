#pragma once

#include <rawrbox/render/materials/unlit.hpp>
#include <rawrbox/render/plugins/clustered.hpp>

namespace rawrbox {
	struct MaterialLitPixelUniforms : public rawrbox::MaterialBasePixelUniforms {
		rawrbox::Vector4f g_LitData = {};
	};

	class MaterialLit : public rawrbox::MaterialBase {
		static bool _built;

	protected:
		static Diligent::RefCntAutoPtr<Diligent::IBuffer> _uniforms;
		static Diligent::RefCntAutoPtr<Diligent::IBuffer> _uniforms_pixel;

	public:
		using vertexBufferType = rawrbox::VertexNormData;

		MaterialLit() = default;
		MaterialLit(MaterialLit&&) = delete;
		MaterialLit& operator=(MaterialLit&&) = delete;
		MaterialLit(const MaterialLit&) = delete;
		MaterialLit& operator=(const MaterialLit&) = delete;
		~MaterialLit() override = default;

		void init() override;
		void createUniforms() override;
		void createPipelines(const std::string& id, const std::vector<Diligent::LayoutElement>& layout, Diligent::IBuffer* uniforms, Diligent::IBuffer* pixelUniforms = nullptr, Diligent::ShaderMacroHelper helper = {}) override;

		template <typename T = rawrbox::VertexData>
		void bindUniforms(const rawrbox::Mesh<T>& mesh) {
			auto context = rawrbox::RENDERER->context();

			// SETUP VERTEX UNIFORMS ----------------------------
			{
				Diligent::MapHelper<rawrbox::MaterialBaseUniforms> CBConstants(context, this->_uniforms, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
				this->bindBaseUniforms<T, rawrbox::MaterialBaseUniforms>(mesh, CBConstants);
			}
			// -----------

			// SETUP PIXEL UNIFORMS ----------------------------
			{
				Diligent::MapHelper<rawrbox::MaterialLitPixelUniforms> CBConstants(context, this->_uniforms_pixel, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
				this->bindBasePixelUniforms<T, rawrbox::MaterialLitPixelUniforms>(mesh, CBConstants);

				CBConstants->g_LitData = mesh.textures.getData();
			}
			// -----------
		}
	};
} // namespace rawrbox
