#include <rawrbox/render/materials/skinned_lit.hpp>
#include <rawrbox/render/utils/render.hpp>

// NOLINTBEGIN(*)
const bgfx::EmbeddedShader model_skinned_shaders[] = {
    BGFX_EMBEDDED_SHADER(vs_clustered_skinned_base),
    BGFX_EMBEDDED_SHADER(fs_clustered_unlit_base),
    BGFX_EMBEDDED_SHADER_END()};
// NOLINTEND(*)

namespace rawrbox {
	MaterialSkinnedLit::~MaterialSkinnedLit() {
		RAWRBOX_DESTROY(u_bones);
	}

	void MaterialSkinnedLit::registerUniforms() {
		MaterialBase::registerUniforms();

		// BONES ----
		u_bones = bgfx::createUniform("u_bones", bgfx::UniformType::Mat4, rawrbox::MAX_BONES_PER_MODEL);
		// ---
	}

	void MaterialSkinnedLit::setBoneData(const std::vector<rawrbox::Matrix4x4>& data) {
		bgfx::setUniform(this->u_bones, &data.front(), static_cast<uint32_t>(data.size()));
	}

	void MaterialSkinnedLit::upload() {
		rawrbox::RenderUtils::buildShader(model_skinned_shaders, this->_program);
	}
} // namespace rawrbox
