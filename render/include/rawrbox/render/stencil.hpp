#pragma once

#include <rawrbox/math/aabb.hpp>
#include <rawrbox/math/color.hpp>
#include <rawrbox/math/pi.hpp>
#include <rawrbox/math/vector2.hpp>
#include <rawrbox/math/vector3.hpp>
#include <rawrbox/render/text/font.hpp>
#include <rawrbox/render/texture/flat.hpp>
#include <rawrbox/render/texture/render.hpp>

#include <bgfx/bgfx.h>

#include <memory>

namespace rawrbox {
	struct PosUVColorVertexData {
		float x = 0;
		float y = 0;
		float z = 0;

		float u = 0;
		float v = 0;

		uint32_t abgr = 0xFFFFFFFF;

		PosUVColorVertexData() = default;
		PosUVColorVertexData(const rawrbox::Vector3f& pos, const rawrbox::Vector2f& uv, const rawrbox::Color& cl) : x(pos.x), y(pos.y), z(pos.z), u(uv.x), v(uv.y), abgr(rawrbox::Color::toHEX(cl)) {}
		PosUVColorVertexData(float _x, float _y, float _z, float _u, float _v, uint32_t _abgr) : x(_x), y(_y), z(_z), u(_u), v(_v), abgr(_abgr) {}
	};

	struct StencilRotation {
		float rotation = 0.F;
		rawrbox::Vector2f origin = {};

		StencilRotation() = default;
		StencilRotation(float _rotation, const rawrbox::Vector2f& _origin) : rotation(_rotation), origin(_origin) {}

		StencilRotation operator-(const StencilRotation& other) const {
			return {this->rotation - other.rotation, this->origin - other.origin};
		}

		StencilRotation& operator-=(const StencilRotation& other) {
			this->rotation -= other.rotation;
			this->origin -= other.origin;

			return *this;
		}

		StencilRotation operator+(const StencilRotation& other) const {
			return {this->rotation + other.rotation, this->origin + other.origin};
		}

		StencilRotation& operator+=(const StencilRotation& other) {
			this->rotation += other.rotation;
			this->origin += other.origin;

			return *this;
		}
	};

	struct StencilOutline {
		float thickness = 0.F;
		float stipple = 0.F;

		StencilOutline() = default;

		// NOLINTBEGIN(hicpp-explicit-conversions)
		StencilOutline(float _thickness) : thickness(_thickness) {}
		// NOLINTEND(hicpp-explicit-conversions)
		StencilOutline(float _thickness, float _stipple) : thickness(_thickness), stipple(_stipple) {}

		bool isSet() { return thickness > 0.F || stipple > 0.F; }

		StencilOutline operator-(const StencilOutline& other) const {
			return {this->thickness - other.thickness, this->stipple - other.stipple};
		}

		StencilOutline& operator-=(const StencilOutline& other) {
			this->thickness -= other.thickness;
			this->stipple -= other.stipple;

			return *this;
		}

		StencilOutline operator+(const StencilOutline& other) const {
			return {this->thickness + other.thickness, this->stipple + other.stipple};
		}

		StencilOutline& operator+=(const StencilOutline& other) {
			this->thickness += other.thickness;
			this->stipple += other.stipple;

			return *this;
		}
	};

	class Stencil {
	private:
		bgfx::ViewId _viewId;
		bgfx::VertexLayout _vLayout;

		bgfx::ProgramHandle _stencilProgram = BGFX_INVALID_HANDLE;

		bgfx::ProgramHandle _2dprogram = BGFX_INVALID_HANDLE;
		bgfx::ProgramHandle _lineprogram = BGFX_INVALID_HANDLE;
		bgfx::ProgramHandle _textprogram = BGFX_INVALID_HANDLE;

		bgfx::TextureHandle _textureHandle = BGFX_INVALID_HANDLE;
		bgfx::UniformHandle _texColor = BGFX_INVALID_HANDLE;

		std::shared_ptr<rawrbox::TextureFlat> _pixelTexture;
		std::shared_ptr<rawrbox::TextureRender> _renderTexture;

		uint64_t _drawMode = 0;
		bool _cull = true;

		rawrbox::Vector2i _windowSize;

		// Offset handling ----
		rawrbox::Vector2f _offset;
		rawrbox::Vector2f _oldOffset;
		std::vector<rawrbox::Vector2f> _offsets;
		// ----------

		// Clip handling ----
		std::vector<rawrbox::AABB> _clips;
		// ----------

		// Outline handling ----
		std::vector<StencilOutline> _outlines; // thickness, stipple
		StencilOutline _outline;
		// ----------

		// Rotation handling ----
		std::vector<StencilRotation> _rotations;
		StencilRotation _rotation;
		// ----------

		// Scale handling ----
		std::vector<rawrbox::Vector2f> _scales;
		rawrbox::Vector2f _scale;
		// ----------

		std::vector<PosUVColorVertexData> _vertices;
		std::vector<uint16_t> _indices;

		bool _recording = false;

		// ------ UTILS
		void pushVertice(rawrbox::Vector2f pos, const rawrbox::Vector2f& uv, const rawrbox::Color& col);
		void pushIndices(uint16_t a, uint16_t b, uint16_t c);

		void applyRotation(rawrbox::Vector2f& vert);
		void applyScale(rawrbox::Vector2f& vert);
		// --------------------

		// ------ RENDERING
		void internalDraw();
		void drawRecording();
		// --------------------
	public:
		Stencil(bgfx::ViewId id, const rawrbox::Vector2i& size);
		Stencil(Stencil&&) = delete;
		Stencil& operator=(Stencil&&) = delete;
		Stencil(const Stencil&) = delete;
		Stencil& operator=(const Stencil&) = delete;

		~Stencil();

		void upload();
		void resize(const rawrbox::Vector2i& size);

		// ------ UTILS
		void drawTriangle(const rawrbox::Vector2f& a, const rawrbox::Vector2f& aUV, const rawrbox::Color& colA, const rawrbox::Vector2f& b, const rawrbox::Vector2f& bUV, const rawrbox::Color& colB, const rawrbox::Vector2f& c, const rawrbox::Vector2f& cUV, const rawrbox::Color& colC);
		void drawBox(const rawrbox::Vector2f& pos, const rawrbox::Vector2f& size, const rawrbox::Color& col = rawrbox::Colors::White);
		void drawTexture(const rawrbox::Vector2f& pos, const rawrbox::Vector2f& size, std::shared_ptr<rawrbox::TextureBase> tex, const rawrbox::Color& col = rawrbox::Colors::White, const rawrbox::Vector2f& uvStart = {0, 0}, const rawrbox::Vector2f& uvEnd = {1, 1});
		void drawCircle(const rawrbox::Vector2f& pos, const rawrbox::Vector2f& size, const rawrbox::Color& col = rawrbox::Colors::White, size_t roundness = 32, float angleStart = 0.F, float angleEnd = 360.F);
		void drawLine(const rawrbox::Vector2& from, const rawrbox::Vector2& to, const rawrbox::Color& col = rawrbox::Colors::White);
		void drawText(rawrbox::Font* font, const std::string& text, const rawrbox::Vector2f& pos, const rawrbox::Color& col = rawrbox::Colors::White, rawrbox::Alignment alignX = rawrbox::Alignment::Left, rawrbox::Alignment alignY = rawrbox::Alignment::Left);
		// --------------------

		// ------ RENDERING
		void setTexture(const bgfx::TextureHandle& tex);
		void setShaderProgram(const bgfx::ProgramHandle& handle);
		void setDrawMode(uint64_t mode = 0);

		void begin();
		void end();
		// --------------------

		// ------ LOCATION
		void pushOffset(const rawrbox::Vector2f& offset);
		void popOffset();
		void pushLocalOffset();
		void popLocalOffset();
		// --------------------

		// ------ ROTATION
		void pushRotation(const StencilRotation& rot);
		void popRotation();
		// --------------------

		// ------ OUTLINE
		void pushOutline(const StencilOutline& outline);
		void popOutline();
		// --------------------

		// ------ CLIPPING
		void pushClipping(const rawrbox::AABB& rect);
		void popClipping();
		// --------------------

		// ------ SCALE
		void pushScale(const rawrbox::Vector2f& scale);
		void popScale();
		// --------------------

		// ------ OTHER
		[[nodiscard]] const std::vector<PosUVColorVertexData>& getVertices() const;
		[[nodiscard]] const std::vector<uint16_t>& getIndices() const;
		void clear();
		// --------------------
	};
} // namespace rawrbox
