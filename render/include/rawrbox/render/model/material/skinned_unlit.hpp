#pragma once

#include <rawrbox/render/model/light/manager.h>
#include <rawrbox/render/model/material/base.hpp>
#include <rawrbox/render/model/material/unlit.hpp>
#include <rawrbox/render/shader_defines.h>
#include <rawrbox/render/static.h>

#include <bgfx/bgfx.h>
#include <fmt/format.h>
#include <generated/shaders/render/all.h>

#include <vector>

// NOLINTBEGIN(*)
static const bgfx::EmbeddedShader model_skinned_unlit_shaders[] = {
    BGFX_EMBEDDED_SHADER(vs_model_skinned_unlit),
    BGFX_EMBEDDED_SHADER(fs_model_skinned_unlit),
    BGFX_EMBEDDED_SHADER_END()};
// NOLINTEND(*)

namespace rawrBox {

	class MaterialSkinnedUnlit : public rawrBox::MaterialUnlit {

	public:
		using MaterialUnlit::MaterialUnlit;

		void upload() override {
			this->buildShader(model_skinned_unlit_shaders, "model_skinned_unlit");
		}
	};
} // namespace rawrBox
