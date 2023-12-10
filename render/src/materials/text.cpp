
#include <rawrbox/render/materials/text.hpp>
#include <rawrbox/render/utils/pipeline.hpp>

#include <MapHelper.hpp>
// #include <Platforms/Basic/interface/DebugUtilities.hpp>

namespace rawrbox {
	// STATIC DATA ----
	Diligent::RefCntAutoPtr<Diligent::IBuffer> MaterialText3D::_uniforms;
	// ----------------

	void MaterialText3D::init() {
		// Uniforms -------
		Diligent::BufferDesc CBDesc;
		CBDesc.Name = "rawrbox::MaterialText3D::Uniforms";
		CBDesc.Size = sizeof(rawrbox::MaterialTextUniforms);
		CBDesc.Usage = Diligent::USAGE_DYNAMIC;
		CBDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
		CBDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;

		rawrbox::RENDERER->device()->CreateBuffer(CBDesc, nullptr, &_uniforms);
		// ------------

		// PIPELINE ----
		rawrbox::PipeSettings settings;
		settings.pVS = "3dtext_unlit.vsh";
		settings.pPS = "3dtext_unlit.psh";
		settings.immutableSamplers = {{Diligent::SHADER_TYPE_PIXEL, "g_Texture"}};
		settings.cull = Diligent::CULL_MODE_FRONT;
		settings.layout = rawrbox::VertexData::vLayout();
		settings.uniforms = {{Diligent::SHADER_TYPE_VERTEX, _uniforms}};
		settings.resources = {
		    Diligent::ShaderResourceVariableDesc{Diligent::SHADER_TYPE_PIXEL, "g_Texture", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}};

		settings.fill = Diligent::FILL_MODE_WIREFRAME;
		rawrbox::PipelineUtils::createPipeline("3DText::Base::Wireframe", "3DText", settings);

		settings.fill = Diligent::FILL_MODE_SOLID;
		settings.blending = {Diligent::BLEND_FACTOR_SRC_ALPHA, Diligent::BLEND_FACTOR_INV_SRC_ALPHA};
		rawrbox::PipelineUtils::createPipeline("3DText::Base", "3DText", settings); // ALPHA by default on text
	}

	void MaterialText3D::prepareMaterial() {
		// Not a fan, but had to move it away from static, since we want to override them
		if (this->_base == nullptr) this->_base = rawrbox::PipelineUtils::getPipeline("3DText::Base");
		if (this->_base_alpha == nullptr) this->_base_alpha = rawrbox::PipelineUtils::getPipeline("3DText::Base");
		if (this->_wireframe == nullptr) this->_wireframe = rawrbox::PipelineUtils::getPipeline("3DText::Base::Wireframe");
		if (this->_bind == nullptr) this->_bind = rawrbox::PipelineUtils::getBind("3DText");
	}

} // namespace rawrbox
