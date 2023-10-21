
#include <rawrbox/render/renderers/base.hpp>
#include <rawrbox/render/texture/render.hpp>

#include <fmt/format.h>

namespace rawrbox {
	TextureRender::TextureRender(const rawrbox::Vector2i& size) : _size(size) {}
	TextureRender::~TextureRender() {
		RAWRBOX_DESTROY(this->_depthHandle);
		RAWRBOX_DESTROY(this->_depthTex);
	}

	// ------ UTILS
	const Diligent::ITextureView* TextureRender::getDepth() const { return this->_depthHandle; }
	// ------------

	// ------ RENDER
	void TextureRender::startRecord(bool clear) {
		const std::array<float, 4> ClearColor = {0.0F, 0.0F, 0.0F, 1.0F};
		rawrbox::RENDERER->context->SetRenderTargets(1, &this->_rtHandle, this->_depthHandle, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		if (clear) {
			rawrbox::RENDERER->context->ClearRenderTarget(this->_rtHandle, ClearColor.data(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			rawrbox::RENDERER->context->ClearDepthStencil(this->_depthHandle, Diligent::CLEAR_DEPTH_FLAG, 1.0F, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		}
	}

	void TextureRender::stopRecord() {
		auto* pRTV = rawrbox::RENDERER->swapChain->GetCurrentBackBufferRTV();
		auto* depth = rawrbox::RENDERER->swapChain->GetDepthBufferDSV();

		rawrbox::RENDERER->context->SetRenderTargets(1, &pRTV, depth, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}

	void TextureRender::upload(Diligent::TEXTURE_FORMAT format, bool /*dynamic*/) {
		if (this->_failedToLoad || this->_rtHandle != nullptr) return; // Failed texture is already bound, so skip it

		// Render target -----
		Diligent::TextureDesc desc;
		desc.Type = Diligent::RESOURCE_DIM_TEX_2D;
		desc.BindFlags = Diligent::BIND_SHADER_RESOURCE | Diligent::BIND_RENDER_TARGET;
		desc.Width = this->_size.x;
		desc.Height = this->_size.y;
		desc.MipLevels = 1;
		desc.Format = format;
		desc.Name = this->_name.c_str();
		desc.ClearValue.Format = desc.Format;
		desc.ClearValue.Color[0] = 0.F;
		desc.ClearValue.Color[1] = 0.F;
		desc.ClearValue.Color[2] = 0.F;
		desc.ClearValue.Color[3] = 1.F;

		rawrbox::RENDERER->device->CreateTexture(desc, nullptr, &this->_tex);
		this->_rtHandle = this->_tex->GetDefaultView(Diligent::TEXTURE_VIEW_RENDER_TARGET);
		this->_handle = this->_tex->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE);
		// --------------

		// Depth ----
		desc.BindFlags = Diligent::BIND_SHADER_RESOURCE | Diligent::BIND_DEPTH_STENCIL;
		desc.Format = Diligent::TEX_FORMAT_D32_FLOAT;
		desc.Name = (this->_name + "::DEPTH").c_str();
		desc.ClearValue.Format = desc.Format;
		desc.ClearValue.DepthStencil.Depth = 1;
		desc.ClearValue.DepthStencil.Stencil = 0;

		rawrbox::RENDERER->device->CreateTexture(desc, nullptr, &this->_depthTex);
		this->_depthHandle = this->_depthTex->GetDefaultView(Diligent::TEXTURE_VIEW_DEPTH_STENCIL);
		// ----------
	}

} // namespace rawrbox
