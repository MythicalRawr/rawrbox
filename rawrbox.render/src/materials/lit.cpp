#include <rawrbox/render/lights/manager.hpp>
#include <rawrbox/render/materials/lit.hpp>
#include <rawrbox/render/utils/pipeline.hpp>

namespace rawrbox {
	// STATIC DATA ----
	bool MaterialLit::_built = false;
	// ----------------

	void MaterialLit::init() {
		const std::string id = "Model::Lit";

		if (!rawrbox::MaterialLit::_built) {
			this->_logger->info("Building {} material..", fmt::format(fmt::fg(fmt::color::azure), id));

			this->createPipelines(id, vertexBufferType::vLayout());
			rawrbox::MaterialLit::_built = true;
		}

		this->setupPipelines(id);
	}

	void MaterialLit::createPipelines(const std::string& id, const std::vector<Diligent::LayoutElement>& layout, Diligent::ShaderMacroHelper helper) {
		auto* cluster = rawrbox::RENDERER->getPlugin<rawrbox::ClusteredPlugin>("Clustered");
		if (cluster == nullptr) throw this->_logger->error("This material requires the `ClusteredPlugin` renderer plugin");

		// PIPELINE ----
		rawrbox::PipeSettings settings;
		settings.pVS = "lit.vsh";
		settings.pPS = "lit.psh";
		settings.renderTargets = 2; // COLOR + GPUPick
		settings.cull = Diligent::CULL_MODE_FRONT;
		settings.macros = cluster->getClusterMacros() + helper;

		settings.signature = rawrbox::BindlessManager::signature;
		settings.layout = layout;
		// -------------------

		settings.fill = Diligent::FILL_MODE_SOLID;
		rawrbox::PipelineUtils::createPipeline(id, settings);

		settings.blending = {Diligent::BLEND_FACTOR_SRC_ALPHA, Diligent::BLEND_FACTOR_INV_SRC_ALPHA};
		rawrbox::PipelineUtils::createPipeline(id + "::Alpha", settings);

		settings.fill = Diligent::FILL_MODE_SOLID;
		settings.cull = Diligent::CULL_MODE_BACK;
		settings.blending = {};
		rawrbox::PipelineUtils::createPipeline(id + "::CullBack", settings);

		settings.blending = {Diligent::BLEND_FACTOR_SRC_ALPHA, Diligent::BLEND_FACTOR_INV_SRC_ALPHA};
		rawrbox::PipelineUtils::createPipeline(id + "::CullBack::Alpha", settings);

		settings.cull = Diligent::CULL_MODE_NONE;
		settings.blending = {};
		rawrbox::PipelineUtils::createPipeline(id + "::CullNone", settings);

		settings.blending = {Diligent::BLEND_FACTOR_SRC_ALPHA, Diligent::BLEND_FACTOR_INV_SRC_ALPHA};
		rawrbox::PipelineUtils::createPipeline(id + "::CullNone::Alpha", settings);
		// -----
	}
} // namespace rawrbox
