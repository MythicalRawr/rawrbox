#pragma once
#include <rawrbox/render/texture/flat.h>
#include <rawrbox/render/texture/missing.h>

#include <bgfx/bgfx.h>

#include <memory>

#define RAWRBOX_DESTROY(HANDLE) \
	if (bgfx::isValid(HANDLE)) { \
		bgfx::destroy(HANDLE); \
		(HANDLE) = BGFX_INVALID_HANDLE; \
	}

namespace rawrBox {
	// NOLINTBEGIN{cppcoreguidelines-avoid-non-const-global-variables}
	constexpr auto MAX_BONES_PER_VERTEX = 4;
	constexpr auto NUM_BONES_PER_VERTEX = 4;

	extern bgfx::ViewId CURRENT_VIEW_ID;
	extern std::shared_ptr<rawrBox::TextureMissing> MISSING_TEXTURE;
	extern std::shared_ptr<rawrBox::TextureFlat> MISSING_SPECULAR_TEXTURE;
	extern std::shared_ptr<rawrBox::TextureFlat> WHITE_TEXTURE;
	// NOLINTEND{cppcoreguidelines-avoid-non-const-global-variables}
} // namespace rawrBox
