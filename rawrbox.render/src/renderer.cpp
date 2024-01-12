
#ifndef ENGINE_DLL
	#define ENGINE_DLL 1
#endif

#if RAWRBOX_SUPPORT_DX11
	#include <EngineFactoryD3D11.h>
#endif

#if RAWRBOX_SUPPORT_DX12
	#include <EngineFactoryD3D12.h>
#endif

#if RAWRBOX_SUPPORT_GL
	#include <EngineFactoryOpenGL.h>
#endif

#if RAWRBOX_SUPPORT_VULKAN
	#include <EngineFactoryVk.h>
#endif

#if RAWRBOX_SUPPORT_METAL
	#include <EngineFactoryMtl.h>
#endif

#include <rawrbox/render/bindless.hpp>
#include <rawrbox/render/renderer.hpp>
#include <rawrbox/render/static.hpp>
#include <rawrbox/render/text/engine.hpp>
#include <rawrbox/render/textures/webp.hpp>
#include <rawrbox/render/utils/render.hpp>
#include <rawrbox/utils/path.hpp>
#include <rawrbox/utils/threading.hpp>

namespace rawrbox {
	RendererBase::RendererBase(Diligent::RENDER_DEVICE_TYPE type, Diligent::NativeWindow window, const rawrbox::Vector2i& size, const rawrbox::Vector2i& screenSize, const rawrbox::Colorf& clearColor) : _clearColor(clearColor.toLinear()), _size(size), _monitorSize(screenSize), _window(window), _type(type) {}
	RendererBase::~RendererBase() {
		this->_render.reset();
		this->_stencil.reset();

		rawrbox::BindlessManager::shutdown();
		rawrbox::PipelineUtils::shutdown();

		RAWRBOX_DESTROY(this->_device);
		RAWRBOX_DESTROY(this->_context);
		RAWRBOX_DESTROY(this->_swapChain);
	}

	void RendererBase::init(Diligent::DeviceFeatures features, uint32_t HEAP_SIZE) {
		Diligent::SwapChainDesc SCDesc;

		// Enable required features --------------------------
		features.WireframeFill = Diligent::DEVICE_FEATURE_STATE_ENABLED;
		features.SparseResources = Diligent::DEVICE_FEATURE_STATE_ENABLED;

		features.BindlessResources = Diligent::DEVICE_FEATURE_STATE_ENABLED;
		features.ShaderResourceRuntimeArray = Diligent::DEVICE_FEATURE_STATE_ENABLED;
		// ---------------------------------------------------

#ifdef _DEBUG
		features.PipelineStatisticsQueries = Diligent::DEVICE_FEATURE_STATE_OPTIONAL;
		features.DurationQueries = Diligent::DEVICE_FEATURE_STATE_OPTIONAL;
#endif
		// --------------

		// PLUGIN REQUIREMENTS ---
		for (auto& plugin : this->_renderPlugins) {
			if (plugin.second == nullptr) continue;
			plugin.second->requirements(features);
		}
		// -----------------------

		// Initialize engine -----
		switch (this->_type) {
#if RAWRBOX_SUPPORT_DX12
			case Diligent::RENDER_DEVICE_TYPE_D3D12:
				{
	#if ENGINE_DLL
					// Load the dll and import GetEngineFactoryD3D12() function
					auto* GetEngineFactoryD3D12 = Diligent::LoadGraphicsEngineD3D12();
	#endif
					auto* pFactoryD3D12 = GetEngineFactoryD3D12();
					this->_engineFactory = pFactoryD3D12;

					Diligent::EngineD3D12CreateInfo EngineCI;
					EngineCI.Features = features;
					if (HEAP_SIZE != 0) {
						EngineCI.GPUDescriptorHeapDynamicSize[0] = HEAP_SIZE * 4;
						EngineCI.GPUDescriptorHeapSize[0] = HEAP_SIZE; // For mutable mode
					}

					pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &this->_device, &this->_context);
					pFactoryD3D12->CreateSwapChainD3D12(this->_device, this->_context, SCDesc, Diligent::FullScreenModeDesc{}, this->_window, &this->_swapChain);
				}
				break;
#endif // D3D12_SUPPORTED

#if RAWRBOX_SUPPORT_VULKAN
			case Diligent::RENDER_DEVICE_TYPE_VULKAN:
				{
	#if EXPLICITLY_LOAD_ENGINE_VK_DLL
					// Load the dll and import GetEngineFactoryVk() function
					auto* GetEngineFactoryVk = Diligent::LoadGraphicsEngineVk();
					auto* pFactoryVk = GetEngineFactoryVk();
	#else
					auto* pFactoryVk = Diligent::GetEngineFactoryVk();
	#endif
					this->_engineFactory = pFactoryVk;

					Diligent::EngineVkCreateInfo EngineCI;
					EngineCI.Features = features;
					pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &this->_device, &this->_context);
					pFactoryVk->CreateSwapChainVk(this->_device, this->_context, SCDesc, this->_window, &this->_swapChain);
				}
				break;
#endif // VULKAN_SUPPORTED

#if RAWRBOX_SUPPORT_DX11
			case Diligent::RENDER_DEVICE_TYPE_D3D11:
				{
	#if ENGINE_DLL
					auto* GetEngineFactoryD3D11 = Diligent::LoadGraphicsEngineD3D11(); // Load the dll and import GetEngineFactoryD3D11() function
	#endif
					auto* pFactoryD3D11 = GetEngineFactoryD3D11();
					this->_engineFactory = pFactoryD3D11;

					Diligent::EngineD3D11CreateInfo EngineCI;
					EngineCI.Features = features;

					pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &this->_device, &this->_context);
					pFactoryD3D11->CreateSwapChainD3D11(this->_device, this->_context, SCDesc, Diligent::FullScreenModeDesc{}, this->_window, &this->_swapChain);
				}
				break;
#endif

#if RAWRBOX_SUPPORT_GL
			case Diligent::RENDER_DEVICE_TYPE_GL:
				{
	#if EXPLICITLY_LOAD_ENGINE_GL_DLL
					// Load the dll and import GetEngineFactoryOpenGL() function
					auto GetEngineFactoryOpenGL = Diligent::LoadGraphicsEngineOpenGL();
					auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
	#else
					auto* pFactoryOpenGL = Diligent::GetEngineFactoryOpenGL();
	#endif
					this->_engineFactory = pFactoryOpenGL;

					Diligent::EngineGLCreateInfo EngineCI;
					EngineCI.Features = features;
					EngineCI.Window = this->_window;
					pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &this->_device, &this->_context, SCDesc, &this->_swapChain);
				}
				break;
#endif // GL_SUPPORTED

			default: throw std::runtime_error("[RawrBox-Window] Invalid diligent engine");
		}

		// Setup shader pipeline if not exists
		if (rawrbox::SHADER_FACTORY == nullptr) {
			auto dirs = rawrbox::PathUtils::glob("assets/shaders", true);
			auto paths = fmt::format("{}", fmt::join(dirs, ";"));

			this->_engineFactory->CreateDefaultShaderSourceStreamFactory(paths.c_str(), &rawrbox::SHADER_FACTORY);
		}
		// -----------

		if (this->_engineFactory == nullptr) throw std::runtime_error("[RawrBox-Renderer] Failed to initialize");

		// Setup camera -----
		if (this->_camera != nullptr) this->_camera->initialize();
		// ------------------

		// PLUGIN INITIALIZE ---
		for (auto& plugin : this->_renderPlugins) {
			if (plugin.second == nullptr) continue;
			plugin.second->initialize(this->getSize());
		}
		// -----------------------

		// Init pipelines ---
		rawrbox::PipelineUtils::init(*this->device());
		rawrbox::BindlessManager::init();
		// ----------------------

		// Init default textures ---
		if (rawrbox::MISSING_TEXTURE == nullptr) {
			rawrbox::MISSING_TEXTURE = std::make_shared<rawrbox::TextureMissing>();
			rawrbox::MISSING_TEXTURE->upload();
		}

		if (rawrbox::MISSING_VERTEX_TEXTURE == nullptr) {
			rawrbox::MISSING_VERTEX_TEXTURE = std::make_shared<rawrbox::TextureMissing>();
			rawrbox::MISSING_VERTEX_TEXTURE->setType(rawrbox::TEXTURE_TYPE::VERTEX);
			rawrbox::MISSING_VERTEX_TEXTURE->upload();
		}

		if (rawrbox::WHITE_TEXTURE == nullptr) {
			rawrbox::WHITE_TEXTURE = std::make_shared<rawrbox::TextureFlat>(rawrbox::Vector2i(2, 2), rawrbox::Colors::White());
			rawrbox::WHITE_TEXTURE->upload();
		}

		if (rawrbox::BLACK_TEXTURE == nullptr) {
			rawrbox::BLACK_TEXTURE = std::make_shared<rawrbox::TextureFlat>(rawrbox::Vector2i(2, 2), rawrbox::Colors::Black());
			rawrbox::BLACK_TEXTURE->upload();
		}

		if (rawrbox::NORMAL_TEXTURE == nullptr) {
			rawrbox::NORMAL_TEXTURE = std::make_shared<rawrbox::TextureFlat>(rawrbox::Vector2i(2, 2), rawrbox::Color::RGBHex(0x7f7fff));
			rawrbox::NORMAL_TEXTURE->upload();
		}
		// -------------------------

		// Init default fonts ------
		if (rawrbox::DEBUG_FONT_REGULAR == nullptr) {
			rawrbox::DEBUG_FONT_REGULAR = rawrbox::TextEngine::load("./assets/fonts/SometypeMono-Regular.ttf", 12);
		}

		if (rawrbox::DEBUG_FONT_BOLD == nullptr) {
			rawrbox::DEBUG_FONT_BOLD = rawrbox::TextEngine::load("./assets/fonts/SometypeMono-Bold.ttf", 12);
		}

		if (rawrbox::DEBUG_FONT_ITALIC == nullptr) {
			rawrbox::DEBUG_FONT_ITALIC = rawrbox::TextEngine::load("./assets/fonts/SometypeMono-Italic.ttf", 12);
		}

		RenderUtils::init();
		// -------------------------

		// Setup stencil ----
		this->_stencil = std::make_unique<rawrbox::Stencil>(this->getSize());
		this->_stencil->upload();
		// ------------------

		// Setup renderer --
		this->_render = std::make_unique<rawrbox::TextureRender>(this->getSize()); // TODO: RESCALE
		this->_render->upload(Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB);
		// --------

		this->playIntro();
		this->_initialized = true;
	}

	void RendererBase::resize(const rawrbox::Vector2i& size, const rawrbox::Vector2i& monitorSize) {
		if (this->_swapChain == nullptr) return;
		this->_swapChain->Resize(size.x, size.y);

		// Resize plugins --
		for (auto& plugin : this->_renderPlugins) {
			if (plugin.second == nullptr) continue;
			plugin.second->resize(size);
		}
		// --------------------

		if (this->_stencil != nullptr) this->_stencil->resize(size);

		this->_size = size;
		this->_monitorSize = monitorSize;
	}

	// PLUGINS ---------------------------
	const std::map<std::string, std::unique_ptr<rawrbox::RenderPlugin>>& RendererBase::getPlugins() const { return this->_renderPlugins; }
	// -----------------------------------

	void RendererBase::setDrawCall(std::function<void(const rawrbox::DrawPass& pass)> call) { this->_drawCall = call; }

	void RendererBase::update() {
		if (this->_currentIntro != nullptr) {
			if (this->_introComplete) {
				this->_introList.erase(this->_introList.begin());

				// Done?
				if (this->_introList.empty()) {
					this->setDrawCall(this->_tempRender);
					this->introComplete();
				} else {
					this->_currentIntro = &this->_introList.begin()->second;
					this->_introComplete = false;
				}

				return;
			}
		} else {
			if (this->_camera != nullptr) {
				this->_camera->update();
			}

			// Update plugins --
			for (auto& plugin : this->_renderPlugins) {
				if (plugin.second == nullptr || !plugin.second->isEnabled()) continue;
				plugin.second->update();
			}
			// --------------------
		}
	}

	void RendererBase::render() {
		if (this->_swapChain == nullptr || this->_context == nullptr || this->_device == nullptr) throw std::runtime_error("[Rawrbox-Renderer] Failed to bind swapChain/context/device! Did you call 'init' ?");
		if (this->_drawCall == nullptr) throw std::runtime_error("[Rawrbox-Renderer] Missing draw call! Did you call 'setDrawCall' ?");

		// Clear backbuffer ----
		this->clear();
		// ---------------------

		// Update camera -------
		if (this->_camera != nullptr) this->_camera->updateBuffer();
		// ---------------------

		// Update textures -----
		rawrbox::BindlessManager::update();
		// ---------------------

		// Perform pre-render --
		for (auto& plugin : this->_renderPlugins) {
			if (plugin.second == nullptr || !plugin.second->isEnabled()) continue;
			plugin.second->preRender();
		}
		// -----------------------

		// Commit signature --
		this->_context->CommitShaderResources(rawrbox::BindlessManager::signatureBind, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		// -----------------------

// Perform world --
#ifdef _DEBUG
		// this->beginQuery("WORLD");
#endif
		this->_render->startRecord();
		this->_drawCall(rawrbox::DrawPass::PASS_OPAQUE);
		this->_render->stopRecord();
#ifdef _DEBUG
		// this->endQuery("WORLD");
#endif
		//  -----------------

		// Perform post-render --
		for (auto& plugin : this->_renderPlugins) {
			if (plugin.second == nullptr || !plugin.second->isEnabled()) continue;
			plugin.second->postRender(this->_render.get());
		}
		// -----------------------

		// Render world ----
		rawrbox::RenderUtils::renderQUAD(*this->_render);
		// ------------------

		// Perform overlay --
#ifdef _DEBUG
		// this->beginQuery("OVERLAY");
#endif
		this->_drawCall(rawrbox::DrawPass::PASS_OVERLAY);
#ifdef _DEBUG
		// this->endQuery("OVERLAY");
#endif
		// ------------------

		// Submit ---
		this->frame();
		// ---------------------
	}

	void RendererBase::clear() {
		if (this->_swapChain == nullptr || this->_context == nullptr) return;

		auto* pRTV = this->_swapChain->GetCurrentBackBufferRTV();
		auto* pDSV = this->_swapChain->GetDepthBufferDSV();

		// Reset render target
		this->_context->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		// Clear the back buffer
		this->_context->ClearRenderTarget(pRTV, this->_clearColor.array().data(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		this->_context->ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.F, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}

	void RendererBase::frame() {
		this->_swapChain->Present(this->_vsync ? 1 : 0); // Submit
		rawrbox::FRAME++;
	}
	/*void RendererBase::gpuCheck() {
		if (this->_gpuReadFrame == rawrbox::BGFX_FRAME) {
			std::unordered_map<uint32_t, uint32_t> ids = {};
			const bgfx::Caps* caps = bgfx::getCaps();

			uint32_t max = 0;
			uint32_t id = 0;

			// ----------
			for (uint8_t* x = this->_gpuPixelData.data(); x < this->_gpuPixelData.data() + rawrbox::GPU_PICK_SAMPLE_SIZE;) {
				uint8_t rr = *x++;
				uint8_t gg = *x++;
				uint8_t bb = *x++;
				uint8_t aa = *x++;

				if (aa == 0) continue;
				if (caps->rendererType == bgfx::RendererType::Direct3D9) {
					std::swap(rr, bb);
				}

				uint32_t hashKey = rawrbox::PackUtils::toABGR(rr / 255.F, gg / 255.F, bb / 255.F, 1.F);
				auto& v = ids[hashKey];
				v++;

				if (v > max) {
					max = v;
					id = hashKey;
				}
			}

			// --------------
			for (auto& p : this->_gpuPickCallbacks) {
				p(id);
			}

			this->_gpuPickCallbacks.clear();
		}
	}*/

	// INTRO ------
	void RendererBase::playIntro() {
		if (this->_skipIntros) {
			this->introComplete();
			return;
		}

		// Temp store renders for overriding ---
		this->_tempRender = this->_drawCall;
		// -------------

		this->_drawCall = [this](const rawrbox::DrawPass& pass) {
			if (pass != rawrbox::DrawPass::PASS_OVERLAY) return;
			auto screenSize = this->_size.cast<float>();

			if (this->_currentIntro != nullptr) {
				this->_stencil->drawBox({}, screenSize, this->_currentIntro->background); // Background

				if (this->_currentIntro->cover) {
					this->_stencil->drawTexture({0, 0}, {screenSize.x, screenSize.y}, *this->_currentIntro->texture);
				} else {
					auto size = this->_currentIntro->texture->getSize().cast<float>();
					this->_stencil->drawTexture({screenSize.x / 2.F - size.x / 2.F, screenSize.y / 2.F - size.y / 2.F}, {size.x, size.y}, *this->_currentIntro->texture);
				}
			} else {
				this->_stencil->drawBox({}, screenSize, rawrbox::Colors::Black()); // Background
			}

			this->_stencil->render();
		};

		// Load webp intros -----------------------
		rawrbox::ASYNC::run([this]() {
			// Load ----
			for (auto& intro : this->_introList) {
				intro.second.texture = std::make_shared<rawrbox::TextureWEBP>(intro.first);
				intro.second.texture->setLoop(false);
				intro.second.texture->setSpeed(intro.second.speed);
				intro.second.texture->onEnd += [this]() {
					this->_introComplete = true;
				};

				intro.second.texture->upload();
			}

			// First intro on the list
			this->_currentIntro = &this->_introList.begin()->second;
		});
		// -------------------------
	}

	void RendererBase::introComplete() {
		this->_introList.clear();
		this->_currentIntro = nullptr;
		this->_introComplete = true;

		this->onIntroCompleted();
	}

	void RendererBase::skipIntros(bool skip) {
		if (skip) fmt::print("[RawrBox] Skipping intros :(\n");
		this->_skipIntros = skip;
	}

	void RendererBase::addIntro(const std::filesystem::path& webpPath, float speed, bool cover, const rawrbox::Colorf& color) {
		if (webpPath.extension() != ".webp") throw std::runtime_error(fmt::format("[RawrBox-RenderBase] Invalid intro '{}', only '.webp' format is supported!", webpPath.generic_string()));
		this->_introList[webpPath.generic_string()] = {speed, cover, color};
	}
//-------------------------

// QUERIES ------
#ifdef _DEBUG
	void RendererBase::beginQuery(const std::string& query) {
		const auto& supportedFeatures = this->device()->GetDeviceInfo().Features;
		if (!supportedFeatures.PipelineStatisticsQueries || !supportedFeatures.DurationQueries) return;

		auto pipeName = fmt::format("{}::PIPELINE", query);
		auto durationName = fmt::format("{}::DURATION", query);

		Diligent::ScopedQueryHelper* pipelineHelper = nullptr;
		Diligent::ScopedQueryHelper* durationHelper = nullptr;

		// PIPELINE QUERY -----
		auto fndPipeline = this->_query.find(pipeName);
		if (fndPipeline == this->_query.end()) {
			auto name = fmt::format("RawrBox::STATS::PIPELINE::{}", query);

			Diligent::QueryDesc queryDesc;
			queryDesc.Name = name.c_str();
			queryDesc.Type = Diligent::QUERY_TYPE_PIPELINE_STATISTICS;

			auto helper = std::make_unique<Diligent::ScopedQueryHelper>(this->device(), queryDesc, 1);
			pipelineHelper = helper.get();
			this->_query[pipeName] = std::move(helper);

			fmt::print("[RawrBox-Renderer] Created query {} -> QUERY_TYPE_PIPELINE_STATISTICS\n", query);
		} else {
			pipelineHelper = fndPipeline->second.get();
		}
		// --------------

		// DURATION QUERY -----
		auto fndDuration = this->_query.find(durationName);
		if (fndDuration == this->_query.end()) {
			auto name = fmt::format("RawrBox::STATS::DURATION::{}", query);

			Diligent::QueryDesc queryDesc;
			queryDesc.Name = name.c_str();
			queryDesc.Type = Diligent::QUERY_TYPE_DURATION;

			auto helper = std::make_unique<Diligent::ScopedQueryHelper>(this->device(), queryDesc, 1);
			durationHelper = helper.get();
			this->_query[durationName] = std::move(helper);

			fmt::print("[RawrBox-Renderer] Created query {} -> QUERY_TYPE_DURATION\n", query);
		} else {
			durationHelper = fndDuration->second.get();
		}
		// --------------

		if (pipelineHelper == nullptr || durationHelper == nullptr) throw std::runtime_error(fmt::format("[RawrBox-Renderer] Failed to create & begin query '{}'", query));

		// Start queries ---
		pipelineHelper->Begin(this->context());
		durationHelper->Begin(this->context());
		// -----------------
	}

	void RendererBase::endQuery(const std::string& query) {
		const auto& supportedFeatures = this->device()->GetDeviceInfo().Features;
		if (!supportedFeatures.PipelineStatisticsQueries || !supportedFeatures.DurationQueries) return;

		auto pipeName = fmt::format("{}::PIPELINE", query);
		auto durationName = fmt::format("{}::DURATION", query);

		auto fndPipeline = this->_query.find(pipeName);
		auto fndDuration = this->_query.find(durationName);

		if (fndPipeline->second == nullptr || fndDuration->second == nullptr) throw std::runtime_error(fmt::format("[RawrBox-Renderer] Failed to end query '{}', not found!", query));

		// End queries ---
		auto pipeData = &this->_pipelineData[pipeName];
		auto durationData = &this->_durationData[durationName];

		fndPipeline->second->End(this->context(), pipeData, sizeof(Diligent::QueryDataPipelineStatistics));
		fndDuration->second->End(this->context(), durationData, sizeof(Diligent::QueryDataDuration));
		// -----------------
	}
#endif
	// ----------------

	// Utils ----
	void RendererBase::setMainCamera(rawrbox::CameraBase* camera) const { rawrbox::MAIN_CAMERA = camera; }
	rawrbox::CameraBase* RendererBase::camera() const { return this->_camera.get(); }
	rawrbox::Stencil& RendererBase::stencil() const { return *this->_stencil.get(); }

	Diligent::IDeviceContext* RendererBase::context() const { return this->_context; }
	Diligent::ISwapChain* RendererBase::swapChain() const { return this->_swapChain; }
	Diligent::IRenderDevice* RendererBase::device() const { return this->_device; }

	Diligent::ITextureView* RendererBase::getDepth() const {
		return this->_render->getDepth();
	}

	Diligent::ITextureView* RendererBase::getColor(bool rt) const {
		return rt ? this->_render->getRT() : this->_render->getHandle();
	}

#ifdef _DEBUG
	// Only available on DEBUG, since it might hit performance :S
	const Diligent::QueryDataPipelineStatistics& RendererBase::getPipelineStats(const std::string& query) {
		auto pipeName = fmt::format("{}::PIPELINE", query);
		return this->_pipelineData[pipeName];
	}
	// Only available on DEBUG, since it might hit performance :S
	const Diligent::QueryDataDuration& RendererBase::getDurationStats(const std::string& query) {
		auto durationName = fmt::format("{}::DURATION", query);
		return this->_durationData[durationName];
	}
#endif

	/*const bgfx::TextureHandle RendererBase::getMask() const {
		if (this->_render == nullptr) return BGFX_INVALID_HANDLE;
		return this->_render->getTexture(1);
	}

	const bgfx::TextureHandle RendererBase::getGPUPick() const {
		if (this->_render == nullptr) return BGFX_INVALID_HANDLE;
		return this->_render->getTexture(2);
	}*/

	const rawrbox::Vector2i& RendererBase::getSize() const { return this->_size; }

	bool RendererBase::getVSync() const { return this->_vsync; }
	void RendererBase::setVSync(bool vsync) { this->_vsync = vsync; }

	/*void RendererBase::gpuPick(const rawrbox::Vector2i& pos, std::function<void(uint32_t)> callback) {
		if (this->_render == nullptr || pos.x < 0 || pos.y < 0 || pos.x >= this->_size.x || pos.y >= this->_size.y) return;

		auto tex = this->getGPUPick();
		if (!bgfx::isValid(tex)) return;

		bgfx::blit(rawrbox::BLIT_VIEW, this->_GPUBlitTex, 0, 0, tex, static_cast<uint16_t>(pos.x), static_cast<uint16_t>(pos.y));
		this->_gpuReadFrame = bgfx::readTexture(this->_GPUBlitTex, this->_gpuPixelData.data());
		this->_gpuPickCallbacks.emplace_back(callback);
	}*/
	// ------
} // namespace rawrbox
