
#include <rawrbox/render/static.hpp>

namespace rawrbox {
	bool ENGINE_INITIALIZED = false;

	// TEXTURE FALLBACKS ---
	std::shared_ptr<rawrbox::TextureMissing> MISSING_TEXTURE = nullptr;
	std::shared_ptr<rawrbox::TextureFlat> WHITE_TEXTURE = nullptr;
	std::shared_ptr<rawrbox::TextureFlat> BLACK_TEXTURE = nullptr;
	std::shared_ptr<rawrbox::TextureFlat> NORMAL_TEXTURE = nullptr;
	// ---------

	// GLOBAL UNIFORMS ---
	uint8_t SAMPLE_MAT_ALBEDO = 0;
	uint8_t SAMPLE_MAT_NORMAL = 1;
	uint8_t SAMPLE_MAT_SPECULAR = 2;
	uint8_t SAMPLE_MAT_EMISSION = 3;
	uint8_t SAMPLE_MAT_DISPLACEMENT = 4;

	uint8_t SAMPLE_LIGHTS = 5;

	uint8_t SAMPLE_DEPTH = 6;
	uint8_t SAMPLE_MASK = 7;

	uint8_t SAMPLE_CLUSTERS = 0;
	uint8_t SAMPLE_ATOMIC_INDEX = 1;

	uint8_t SAMPLE_LIGHTINDICES = 8; // Light data used on models
	uint8_t SAMPLE_LIGHTGRID = 9;

	uint8_t SAMPLE_INSTANCE_DATA = 10;
	// ---------

	// ID GENERATION
	size_t SOUND_ID = 0;
	size_t LIGHT_ID = 0;
	size_t EMITTER_ID = 0;
	// -----

	// QUICK ACCESS ---
	// uint32_t BGFX_FRAME = 0;
	//
	rawrbox::CameraBase* MAIN_CAMERA = nullptr;
	rawrbox::RendererBase* RENDERER = nullptr;
	rawrbox::Matrix4x4 TRANSFORM = {};

	Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> SHADER_FACTORY = {};
	//
	// rawrbox::RENDER_DEBUG_MODE RENDERER_DEBUG = rawrbox::RENDER_DEBUG_MODE::DEBUG_OFF;
	// ----

	// INTERNAL, DO NOT USE
	size_t __OPEN_WINDOWS__ = 0;
	bool __LIGHT_DIRTY__ = true;
	// -------
} // namespace rawrbox
