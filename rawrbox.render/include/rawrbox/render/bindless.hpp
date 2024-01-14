#pragma once

#include <rawrbox/math/vector4.hpp>
#include <rawrbox/render/textures/base.hpp>
#include <rawrbox/render/textures/render.hpp>

#include <RefCntAutoPtr.hpp>

#include <ShaderResourceBinding.h>

namespace rawrbox {
	constexpr auto MAX_PIXEL_DATA = 4;
	constexpr auto MAX_POST_DATA = 2;

	enum class BufferType {
		CONSTANT,
		INDEX,
		VERTEX,
		UNORDERED_ACCESS,
		SHADER
	};

	// --------------------------
	struct BindlessVertexBuffer {
		// UNLIT ---
		rawrbox::Colorf colorOverride = {};
		rawrbox::Vector4f textureFlags = {};

		std::array<rawrbox::Vector4f, MAX_PIXEL_DATA> data = {}; // Other mesh data, like vertex / displacement / billboard settings / masks
									 // ----------
	};
	// --------------------------

	struct BindlessPixelBuffer {
		rawrbox::Vector4_t<uint32_t> textureIDs = {}; // BASE, NORMAL, ROUGHTMETAL, EMISSION
		rawrbox::Vector4f litData = {};               // Texture data
	};

	struct BindlessPostProcessBuffer {
		std::array<rawrbox::Vector4f, MAX_POST_DATA> data = {};

		uint32_t textureID = 0;
		uint32_t depthTextureID = 0;
	};
	// --------------------------

	class BindlessManager {
	protected:
		static std::vector<Diligent::IDeviceObject*> _textureHandles;
		static std::vector<Diligent::IDeviceObject*> _vertexTextureHandles;

		static std::vector<rawrbox::TextureBase*> _updateTextures;

		static std::vector<Diligent::StateTransitionDesc> _barriers;
		static std::vector<std::function<void()>> _barriersCallbacks;

		static std::unique_ptr<rawrbox::Logger> _logger;

		static void processBarriers();

		static void registerUpdateTexture(rawrbox::TextureBase& tex);
		static void unregisterUpdateTexture(rawrbox::TextureBase& tex);

		static Diligent::RESOURCE_STATE mapResource(rawrbox::BufferType type);
		static uint32_t internalRegister(Diligent::ITextureView* view, rawrbox::TEXTURE_TYPE type);

	public:
		static Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> signature;
		static Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> signatureBind;

		static Diligent::RefCntAutoPtr<Diligent::IBuffer> signatureBufferPixel;
		static Diligent::RefCntAutoPtr<Diligent::IBuffer> signatureBufferVertex;
		static Diligent::RefCntAutoPtr<Diligent::IBuffer> signatureBufferPostProcess;

		static void init();
		static void shutdown();
		static void update();

		// BARRIERS -------
		static void barrier(const rawrbox::TextureBase& texture, std::function<void()> callback = nullptr);
		static void barrier(Diligent::ITexture& texture, Diligent::RESOURCE_STATE state = Diligent::RESOURCE_STATE_SHADER_RESOURCE, std::function<void()> callback = nullptr);
		static void barrier(Diligent::IBuffer& buffer, rawrbox::BufferType type, std::function<void()> callback = nullptr);
		// ----------------

		// TEXTURES -------
		static void registerTexture(rawrbox::TextureBase& texture);
		static void registerTexture(rawrbox::TextureRender& texture);

		static void unregisterTexture(rawrbox::TextureBase& texture);
		// ----------------
	};
} // namespace rawrbox
