#include <rawrbox/render/plugins/post_process.hpp>
#include <rawrbox/render/static.hpp>

namespace rawrbox {
	PostProcessPlugin::~PostProcessPlugin() {
		this->_postProcesses.clear();
		RAWRBOX_DESTROY(this->_buffer);
	}

	void PostProcessPlugin::initialize(const rawrbox::Vector2u& /*size*/) {
		Diligent::BufferDesc BuffPixelDesc;
		BuffPixelDesc.Name = "rawrbox::PostProcess";
		BuffPixelDesc.Usage = Diligent::USAGE_DYNAMIC;
		BuffPixelDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
		BuffPixelDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
		BuffPixelDesc.Size = sizeof(rawrbox::BindlessPostProcessBuffer);

		rawrbox::RENDERER->device()->CreateBuffer(BuffPixelDesc, nullptr, &this->_buffer);

		// BARRIER -----
		rawrbox::BarrierUtils::barrier({{this->_buffer, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_CONSTANT_BUFFER, Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE}});
		// ---------------
	}

	void PostProcessPlugin::upload() {
		for (const auto& _postProcess : this->_postProcesses) {
			_postProcess->init();
		}
	}

	void PostProcessPlugin::signatures(std::vector<Diligent::PipelineResourceDesc>& sig) {
		sig.emplace_back(Diligent::SHADER_TYPE_PIXEL, "PostProcessConstants", 1, Diligent::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER, Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC);
	}

	void PostProcessPlugin::bindStatic(Diligent::IPipelineResourceSignature& sig) {
		sig.GetStaticVariableByName(Diligent::SHADER_TYPE_PIXEL, "PostProcessConstants")->Set(this->_buffer);
	}

	void PostProcessPlugin::postRender(rawrbox::TextureRender& renderTexture) {
		for (auto& process : this->_postProcesses) {
			if (!process->isEnabled()) continue;

			renderTexture.startRecord(false, 1);
			process->applyEffect(renderTexture);
			renderTexture.stopRecord();
		}
	}

	// Post utils ----
	void PostProcessPlugin::remove(size_t indx) {
		if (this->_postProcesses.empty() || indx >= this->_postProcesses.size()) throw this->_logger->error("Failed to remove {}!", indx);
		this->_postProcesses.erase(this->_postProcesses.begin() + indx);
	}

	rawrbox::PostProcessBase* PostProcessPlugin::get(size_t indx) const {
		if (indx >= this->_postProcesses.size()) throw this->_logger->error("Failed to get {}!", indx);
		return this->_postProcesses[indx].get();
	}

	Diligent::IBuffer* PostProcessPlugin::getBuffer() const { return this->_buffer; }
	size_t PostProcessPlugin::count() { return this->_postProcesses.size(); }

	std::string PostProcessPlugin::getID() { return "PostProcess"; }
	// ----
} // namespace rawrbox
