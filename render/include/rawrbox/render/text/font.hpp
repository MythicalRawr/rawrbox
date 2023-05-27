#pragma once

#include <rawrbox/math/vector2.hpp>
#include <rawrbox/render/texture/atlas.hpp>

#include <bgfx/bgfx.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <memory>
#include <string>
#include <vector>

namespace rawrbox {
	enum class Alignment {
		Left,
		Center,
		Right
	};

	struct Glyph {
		uint32_t atlasID{};

		FT_ULong codepoint{};
		FT_UInt glyphIndex{};

		rawrbox::Vector2f bearing;
		rawrbox::Vector2f advance;
		rawrbox::Vector2f textureTopLeft;
		rawrbox::Vector2f textureBottomRight;

		rawrbox::Vector2i size;
	};

	class TextEngine;
	class Font {

	private:
		std::vector<Glyph> _glyphs;

		std::string _file;
		FT_Render_Mode _mode = FT_RENDER_MODE_NORMAL;

		// SIZE ----
		uint32_t bitmapR = -1;
		uint32_t bitmapW = -1;
		FT_Int bitmapX = -1;
		FT_Int bitmapY = -1;
		// -----

		// GLYPH LOADING -----
		Glyph loadGlyph(FT_ULong character);
		std::vector<unsigned char> generateGlyph();

		void preloadGlyphs(const std::string& chars);
		std::string getFontInSystem(const std::string& path);
		// -----
	public:
		FT_Face face = {};
		uint32_t size;

		Font(std::string  filename, uint32_t size, FT_Render_Mode renderMode = FT_RENDER_MODE_NORMAL);
		Font(Font&&) = delete;
		Font& operator=(Font&&) = delete;
		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;
		~Font();

		// UTILS --
		[[nodiscard]] float getLineHeight() const;
		[[nodiscard]] bool hasGlyph(uint32_t codepoint) const;
		[[nodiscard]] const Glyph& getGlyph(uint32_t codepoint) const;
		[[nodiscard]] float getKerning(const Glyph& left, const Glyph& right) const;
		[[nodiscard]] rawrbox::Vector2 getStringSize(const std::string& text) const;

		std::shared_ptr<rawrbox::TextureAtlas> getAtlasTexture(const Glyph& g);
		// ----
	};

} // namespace rawrbox
