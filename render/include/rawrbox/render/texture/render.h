#pragma once

#include <rawrbox/render/texture/base.h>
#include <rawrbox/math/vector2.hpp>

#include <bgfx/bgfx.h>

namespace rawrBox {
	class TextureRender : public TextureBase {
		static uint32_t renderID;

	private:
		bgfx::FrameBufferHandle _renderView = BGFX_INVALID_HANDLE;
		rawrBox::Vector2i _size;

		bgfx::ViewId _viewId;
		bgfx::ViewId _renderId;

	public:
		TextureRender(const bgfx::ViewId& viewId, const rawrBox::Vector2i& size);

		// ------RENDER
		virtual void startRecord();
		virtual void stopRecord();

		virtual void upload() override;
		// --------------------

		virtual const bgfx::ViewId& id();
	};
}
