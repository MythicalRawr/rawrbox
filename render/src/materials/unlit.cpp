
#include <rawrbox/render/materials/unlit.hpp>

namespace rawrbox {

	// STATIC DATA ----
	Diligent::RefCntAutoPtr<Diligent::IBuffer> MaterialUnlit::_uniforms;
	// ----------------

	void MaterialUnlit::init() {
		// Uniforms -------
		Diligent::BufferDesc CBDesc;
		CBDesc.Name = "rawrbox::MaterialUnlit::Uniforms";
		CBDesc.Size = sizeof(rawrbox::MaterialUnlitUniforms);
		CBDesc.Usage = Diligent::USAGE_DYNAMIC;
		CBDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
		CBDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;

		rawrbox::RENDERER->device()->CreateBuffer(CBDesc, nullptr, &_uniforms);
		// ------------

		// PIPELINE ----
		rawrbox::PipeSettings settings;
		settings.pVS = "unlit.vsh";
		settings.pPS = "unlit.psh";
		settings.cull = Diligent::CULL_MODE_FRONT;
		settings.layout = rawrbox::VertexData::vLayout();
		settings.immutableSamplers = {false, true};
		settings.resources = {
		    Diligent::ShaderResourceVariableDesc{Diligent::SHADER_TYPE_PIXEL, "g_Texture", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
		    Diligent::ShaderResourceVariableDesc{Diligent::SHADER_TYPE_VERTEX, "g_Displacement", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}};
		settings.uniforms = {{Diligent::SHADER_TYPE_VERTEX, _uniforms}};

		rawrbox::PipelineUtils::createPipelines("Model::Unlit", "Model::Unlit", settings);

		settings.blending = {Diligent::BLEND_FACTOR_SRC_ALPHA, Diligent::BLEND_FACTOR_INV_SRC_ALPHA};
		rawrbox::PipelineUtils::createPipelines("Model::Unlit::Alpha", "Model::Unlit", settings);

		settings.blending = {};
		settings.topology = Diligent::PRIMITIVE_TOPOLOGY_LINE_LIST;
		settings.cull = Diligent::CULL_MODE_NONE;
		rawrbox::PipelineUtils::createPipelines("Model::Line", "Model::Unlit", settings);

		settings.topology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		settings.cull = Diligent::CULL_MODE_BACK;
		rawrbox::PipelineUtils::createPipelines("Model::Unlit::CullBack", "Model::Unlit", settings);

		settings.blending = {Diligent::BLEND_FACTOR_SRC_ALPHA, Diligent::BLEND_FACTOR_INV_SRC_ALPHA};
		rawrbox::PipelineUtils::createPipelines("Model::Unlit::CullBack::Alpha", "Model::Unlit", settings);

		settings.cull = Diligent::CULL_MODE_NONE;
		rawrbox::PipelineUtils::createPipelines("Model::Unlit::CullNone::Alpha", "Model::Unlit", settings);

		settings.blending = {};
		rawrbox::PipelineUtils::createPipelines("Model::Unlit::CullNone", "Model::Unlit", settings);

		settings.blending = {Diligent::BLEND_FACTOR_SRC_ALPHA, Diligent::BLEND_FACTOR_INV_SRC_ALPHA};
		rawrbox::PipelineUtils::createPipelines("Model::Unlit::CullNone::Alpha", "Model::Unlit", settings);

		settings.blending = {};
		settings.fill = Diligent::FILL_MODE_WIREFRAME;
		rawrbox::PipelineUtils::createPipelines("Model::Unlit::Wireframe", "Model::Unlit", settings);
		// -----
	}

	void MaterialUnlit::prepareMaterial() {
		// Not a fan, but had to move it away from static, since we want to override them
		if (this->_base == nullptr) this->_base = rawrbox::PipelineUtils::getPipeline("Model::Unlit");
		if (this->_base_alpha == nullptr) this->_base_alpha = rawrbox::PipelineUtils::getPipeline("Model::Unlit::Alpha");

		if (this->_line == nullptr) this->_line = rawrbox::PipelineUtils::getPipeline("Model::Line");

		if (this->_cullback == nullptr) this->_cullback = rawrbox::PipelineUtils::getPipeline("Model::Unlit::CullBack");
		if (this->_cullback_alpha == nullptr) this->_cullback_alpha = rawrbox::PipelineUtils::getPipeline("Model::Unlit::CullBack::Alpha");

		if (this->_wireframe == nullptr) this->_wireframe = rawrbox::PipelineUtils::getPipeline("Model::Unlit::Wireframe");

		if (this->_cullnone == nullptr) this->_cullnone = rawrbox::PipelineUtils::getPipeline("Model::Unlit::CullNone");
		if (this->_cullnone_alpha == nullptr) this->_cullnone_alpha = rawrbox::PipelineUtils::getPipeline("Model::Unlit::CullNone::Alpha");

		if (this->_bind == nullptr) this->_bind = rawrbox::PipelineUtils::getBind("Model::Unlit");
	}

	void MaterialUnlit::bindShaderResources() {
		if (this->_bind == nullptr) throw std::runtime_error("[RawrBox-MaterialUnlit] Bind not set!");

		auto context = rawrbox::RENDERER->context();
		context->CommitShaderResources(this->_bind, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}
} // namespace rawrbox
