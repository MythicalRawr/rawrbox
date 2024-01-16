#include <rawrbox/render/materials/skinned.hpp>
#include <rawrbox/render/utils/pipeline.hpp>

namespace rawrbox {
	// STATIC DATA ----
	bool MaterialSkinned::_built = false;
	// ----------------

	void MaterialSkinned::init() {
		const std::string id = "Model::Skinned";

		if (!this->_built) {
			this->_logger->info("Building {} material..", fmt::format(fmt::fg(fmt::color::azure), id));

			Diligent::ShaderMacroHelper helper;
			helper.AddShaderMacro("MAX_BONES", rawrbox::MAX_BONES_PER_MODEL);
			helper.AddShaderMacro("NUM_BONES_PER_VERTEX", rawrbox::MAX_BONES_PER_VERTEX);
			helper.AddShaderMacro("SKINNED", true);

			this->createPipelines(id, vertexBufferType::vLayout(), helper);
			this->_built = true;
		}

		this->setupPipelines(id);
	}
} // namespace rawrbox
