#pragma once

#include <rawrbox/render/bindless.hpp>
#include <rawrbox/render/models/mesh.hpp>
#include <rawrbox/render/models/vertex.hpp>
#include <rawrbox/utils/logger.hpp>

#include <InputLayout.h>
#include <PipelineState.h>

namespace rawrbox {
	class MaterialBase {

	protected:
		Diligent::IPipelineState* _base = nullptr;
		Diligent::IPipelineState* _base_alpha = nullptr;
		Diligent::IPipelineState* _line = nullptr;
		Diligent::IPipelineState* _cullback = nullptr;
		Diligent::IPipelineState* _cullback_alpha = nullptr;
		Diligent::IPipelineState* _wireframe = nullptr;
		Diligent::IPipelineState* _cullnone = nullptr;
		Diligent::IPipelineState* _cullnone_alpha = nullptr;

		std::unique_ptr<rawrbox::Logger> _logger = std::make_unique<rawrbox::Logger>("RawrBox-Material");

	public:
		using vertexBufferType = rawrbox::VertexData;

		MaterialBase() = default;
		MaterialBase(MaterialBase&&) = delete;
		MaterialBase& operator=(MaterialBase&&) = delete;
		MaterialBase(const MaterialBase&) = delete;
		MaterialBase& operator=(const MaterialBase&) = delete;
		virtual ~MaterialBase() = default;

		virtual void init() = 0;

		virtual void createPipelines(const std::string& id, const std::vector<Diligent::LayoutElement>& layout, Diligent::ShaderMacroHelper helper = {});
		virtual void setupPipelines(const std::string& id);

		template <typename T = rawrbox::VertexData>
		void bindBaseUniforms(const rawrbox::Mesh<T>& mesh, Diligent::MapHelper<rawrbox::BindlessVertexBuffer>& helper) {
			// MODEL -------
			std::array<rawrbox::Vector4f, rawrbox::MAX_VERTEX_DATA>
			    data = {rawrbox::Vector4f{0.F, 0.F, 0.F, 0.F}, {0.F, 0.F, 0.F, 0.F}, {0.F, 0.F, 0.F, 0.F}, {0.F, 0.F, 0.F, 0.F}};

			if (mesh.hasData("billboard_mode")) {
				data[0] = mesh.getData("billboard_mode").data();
			}

			if (mesh.hasData("vertex_snap")) {
				data[1] = mesh.getData("vertex_snap").data();
			}

			if (mesh.hasData("displacement")) {
				data[2] = mesh.getData("displacement").data();
			}

			if (mesh.hasData("mask")) {
				data[3] = mesh.getData("mask").data();
			}

			helper->colorOverride = mesh.color;
			helper->textureFlags = mesh.textures.texture == nullptr ? rawrbox::Vector4f() : mesh.textures.texture->getData();
			helper->bones = mesh.boneTransforms;
			helper->data = data;
			// ----------------------------
		}

		template <typename T = rawrbox::VertexData>
		void bindBasePixelUniforms(const rawrbox::Mesh<T>& mesh, Diligent::MapHelper<rawrbox::BindlessPixelBuffer>& helper) {
			helper->textureIDs = mesh.textures.getPixelIDs();
		}

		template <typename T = rawrbox::VertexData>
		void bindUniforms(const rawrbox::Mesh<T>& /*mesh*/) {
			throw this->_logger->error("Missing implementation");
		}

		template <typename T = rawrbox::VertexData>
		void bindPipeline(const rawrbox::Mesh<T>& mesh) {
			if (this->_base == nullptr) throw this->_logger->error("Material not initialized!");
			auto context = rawrbox::RENDERER->context();

			if (mesh.wireframe) {
				if (this->_line == nullptr) throw this->_logger->error("Wireframe not supported on material");
				context->SetPipelineState(this->_wireframe);
			} else if (mesh.lineMode) {
				if (this->_line == nullptr) throw this->_logger->error("Line not supported on material");
				context->SetPipelineState(this->_line);
			} else {
				if (mesh.culling == Diligent::CULL_MODE_NONE) {
					if (this->_cullnone == nullptr) throw this->_logger->error("Disabled cull not supported on material");
					if (mesh.alphaBlend && this->_cullnone_alpha == nullptr) throw this->_logger->error("Disabled alpha cull not supported on material");
					context->SetPipelineState(mesh.alphaBlend ? this->_cullnone_alpha : this->_cullnone);
				} else if (mesh.culling == Diligent::CULL_MODE_BACK) {
					if (this->_cullback == nullptr) throw this->_logger->error("Cull back not supported on material");
					if (mesh.alphaBlend && this->_cullback_alpha == nullptr) throw this->_logger->error("Cull back alpha not supported on material");
					context->SetPipelineState(mesh.alphaBlend ? this->_cullback_alpha : this->_cullback);
				} else {
					if (mesh.alphaBlend && this->_base_alpha == nullptr) throw this->_logger->error("Alpha not supported on material");
					context->SetPipelineState(mesh.alphaBlend ? this->_base_alpha : this->_base);
				}
			}
		}
	};
} // namespace rawrbox
