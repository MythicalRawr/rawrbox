#include <rawrbox/render/materials/decal.hpp>
#include <rawrbox/render/utils/render.hpp>

// NOLINTBEGIN(*)
const bgfx::EmbeddedShader model_decal_shaders[] = {
    BGFX_EMBEDDED_SHADER(vs_decal_base),
    BGFX_EMBEDDED_SHADER(fs_decal_unlit_base),
    BGFX_EMBEDDED_SHADER_END()};
// NOLINTEND(*)

namespace rawrbox {
	MaterialDecal::~MaterialDecal() {
		RAWRBOX_DESTROY(this->_s_depth);
		RAWRBOX_DESTROY(this->_u_decalMatrix);
	}

	void MaterialDecal::registerUniforms() {
		rawrbox::MaterialBase::registerUniforms();

		this->_s_depth = bgfx::createUniform("s_depth", bgfx::UniformType::Sampler);
		this->_u_decalMatrix = bgfx::createUniform("u_decalMatrix", bgfx::UniformType::Mat4);
	}

	void MaterialDecal::upload() {
		rawrbox::RenderUtils::buildShader(model_decal_shaders, this->_program);
	}

	void MaterialDecal::process(const rawrbox::Mesh& mesh) {
		/*if (rawrbox::RENDERER == nullptr) return;
		rawrbox::MaterialBase::process(mesh);

		auto inv = mesh.vertexPos;
		// inv.inverse();

		bgfx::setUniform(this->_u_decalMatrix, inv.data());
		bgfx::setTexture(rawrbox::SAMPLE_DEPTH, this->_s_depth, rawrbox::RENDERER->getDepth());*/
	}
} // namespace rawrbox
