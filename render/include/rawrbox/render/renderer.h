#pragma once

#include <rawrbox/math/vector2.hpp>
#include <rawrbox/render/stencil.h>

#include <bgfx/bgfx.h>
#include <memory>

namespace rawrBox {
	class Renderer {
		private:
			// Default settings
			uint32_t _clearColor = 0x000000FF;
			bgfx::ViewId _id = 0;
			// -----

			rawrBox::Vector2i _size = {};
			std::unique_ptr<rawrBox::Stencil> _stencil = nullptr;
		public:
			~Renderer();
			Renderer(const bgfx::ViewId& id, const rawrBox::Vector2i& size);

			void initialize();
			void setClearColor(uint32_t clearColor);
			void resizeView(const rawrBox::Vector2i& size);
			void clear();

			// ------RENDERING
			void swapBuffer() const;
			void render() const;
			// --------------------

			// ------UTILS
			bgfx::ViewId& getID();
			rawrBox::Vector2i& getSize();
			rawrBox::Stencil& getStencil() const;
			// --------------------
	};
}
