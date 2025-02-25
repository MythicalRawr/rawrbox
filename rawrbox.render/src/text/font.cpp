#include <rawrbox/math/utils/color.hpp>
#include <rawrbox/render/text/engine.hpp>
#include <rawrbox/render/text/font.hpp>

// NOLINTBEGIN(clang-diagnostic-unknown-pragmas)
#pragma warning(push)
#pragma warning(disable : 4505)
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.hpp>
#pragma warning(pop)
// NOLINTEND(clang-diagnostic-unknown-pragmas)

#include <fmt/format.h>
#include <utf8.h>

#include <string>

namespace rawrbox {
	Font::~Font() {
		if (this->_font == nullptr) return;

		this->_font.reset();
		this->_glyphs.clear();
	}

	// NOLINTBEGIN(modernize-pass-by-value)
	Font::Font(
	    const std::filesystem::path& fileName, int16_t widthPadding, int16_t heightPadding) : _fileName(fileName), _widthPadding(widthPadding), _heightPadding(heightPadding), _info({}) {}
	// NOLINTEND(modernize-pass-by-value)

	// INTERNAL ---
	void Font::loadFontInfo() {
		if (this->_font == nullptr) RAWRBOX_CRITICAL("Font not loaded");

		int ascent = 0;
		int descent = 0;
		int lineGap = 0;
		stbtt_GetFontVMetrics(this->_font.get(), &ascent, &descent, &lineGap);

		int x0 = 0;
		int y0 = 0;
		int x1 = 0;
		int y1 = 0;
		stbtt_GetFontBoundingBox(this->_font.get(), &x0, &y0, &x1, &y1);

		this->_info = {};
		this->_info.scale = 1.0F;
		this->_info.ascender = std::round(ascent * this->_scale);
		this->_info.descender = std::round(descent * this->_scale);
		this->_info.lineGap = std::round(lineGap * this->_scale);
		this->_info.maxAdvanceWidth = std::round((y1 - y0) * this->_scale);

		this->_info.underlinePosition = (x1 - x0) * this->_scale - ascent;
		this->_info.underlineThickness = (x1 - x0) * this->_scale / 24.F;
	}

	std::unique_ptr<rawrbox::Glyph> Font::bakeGlyphAlpha(uint32_t codePoint) {
		if (this->_font == nullptr) RAWRBOX_CRITICAL("Font not loaded");

		int32_t ascent = 0;
		int32_t descent = 0;
		int32_t lineGap = 0;
		stbtt_GetFontVMetrics(this->_font.get(), &ascent, &descent, &lineGap);

		int32_t advance = 0;
		int32_t lsb = 0;
		stbtt_GetCodepointHMetrics(this->_font.get(), codePoint, &advance, &lsb);

		const float scale = this->_scale;
		int32_t x0 = 0;
		int32_t y0 = 0;
		int32_t x1 = 0;
		int32_t y1 = 0;
		stbtt_GetCodepointBitmapBox(this->_font.get(), codePoint, scale, scale, &x0, &y0, &x1, &y1);

		const auto ww = static_cast<int16_t>(x1 - x0);
		const auto hh = static_cast<int16_t>(y1 - y0);

		std::unique_ptr<rawrbox::Glyph> glyph = std::make_unique<rawrbox::Glyph>();
		glyph->codePoint = codePoint;
		glyph->offset = {static_cast<float>(x0), static_cast<float>(y0)};
		glyph->size = {static_cast<float>(ww), static_cast<float>(hh)};
		glyph->advance = {std::round(static_cast<float>(advance) * scale), std::round((static_cast<float>(ascent + descent + lineGap)) * scale)};

		// Bitmap ----
		uint16_t bpp = 1;
		uint16_t dstPitch = ww * bpp;

		std::vector<uint8_t> buffer = {};
		buffer.resize(ww * hh * sizeof(uint8_t));
		stbtt_MakeCodepointBitmap(this->_font.get(), buffer.data(), ww, hh, dstPitch, scale, scale, codePoint);

		auto pack = rawrbox::TextEngine::requestPack(ww, hh, Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM); // FONT_TYPE_ALPHA
		if (pack.second == nullptr) RAWRBOX_CRITICAL("Failed to generate / get atlas texture");

		auto& packNode = pack.second->addSprite(ww, hh, rawrbox::ColorUtils::setChannels(1, 4, ww, hh, buffer));
		auto size = pack.second->getSize();

		glyph->packID = pack.first;
		glyph->textureTopLeft = {packNode.x / static_cast<float>(size.x), packNode.y / static_cast<float>(size.y)};
		glyph->textureBottomRight = {(packNode.x + packNode.width) / static_cast<float>(size.x), (packNode.y + packNode.height) / static_cast<float>(size.y)};

		glyph->scale = this->_info.scale;

		glyph->advance *= this->_info.scale;
		glyph->offset *= this->_info.scale;
		glyph->size *= this->_info.scale;

		return glyph;
	}

	void Font::generateGlyph(uint32_t codePoint) {
		if (this->hasGlyph(codePoint)) return;
		this->_glyphs[codePoint] = this->bakeGlyphAlpha(codePoint);
	}
	// ----

	// LOADING ---
	void Font::load(const std::vector<uint8_t>& buffer, uint16_t pixelHeight, uint32_t fontIndex) {
		int offset = stbtt_GetFontOffsetForIndex(buffer.data(), fontIndex); // Get the offset for `otf` fonts

		// Load
		this->_font = std::make_shared<stbtt_fontinfo>();
		if (stbtt_InitFont(this->_font.get(), buffer.data(), offset) == 0) RAWRBOX_CRITICAL("Failed to load font");
		this->_scale = stbtt_ScaleForMappingEmToPixels(this->_font.get(), static_cast<float>(pixelHeight));
		this->_pixelSize = static_cast<float>(pixelHeight);

		this->loadFontInfo();
		this->addChars("�~!@#$%^&*()_+`1234567890-=QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm|<>?,./:;\"'}{][ \\§°Ø");

		this->_charSize = this->getStringSize("�"); // Biggest char in the font
	}

	void Font::addChars(const std::string& chars) {
		auto charsIter = chars.begin();
		while (charsIter < chars.end()) {
			this->generateGlyph(utf8::next(charsIter, chars.end()));
		}
	}

	rawrbox::Font* Font::scale(uint16_t size) {
		if (size == this->getSize()) return this;
		return rawrbox::TextEngine::load(this->_fileName, size);
	}
	// ----

	// UTILS ---
	const rawrbox::FontInfo& Font::getFontInfo() const { return this->_info; }

	bool Font::hasGlyph(uint32_t codepoint) const {
		return this->_glyphs.find(codepoint) != this->_glyphs.end();
	}

	rawrbox::Glyph* Font::getGlyph(uint32_t codepoint) const {
		auto fnd = this->_glyphs.find(codepoint);
		if (fnd == this->_glyphs.end()) return this->_glyphs.find(65533)->second.get(); // �
		return fnd->second.get();
	}

	float Font::getSize() const { return this->_pixelSize; }
	float Font::getScale() const { return this->_scale; }
	float Font::getLineHeight() const { return this->_info.ascender - this->_info.descender + this->_info.lineGap; }
	float Font::getKerning(uint32_t prevCodePoint, uint32_t nextCodePoint) const {
		if (this->_font == nullptr || ((this->_font->kern == 0) && (this->_font->gpos == 0))) return 0; // no kerning
		return this->_info.scale * static_cast<float>(stbtt__GetGlyphKernInfoAdvance(this->_font.get(), prevCodePoint, nextCodePoint));
	}

	const rawrbox::Vector2f& Font::getCharSize() const {
		return this->_charSize;
	}

	rawrbox::Vector2f Font::getStringSize(const std::string& text) const {
		rawrbox::Vector2f size = {};
		if (this->_font == nullptr) return size;

		const float lineHeight = this->getLineHeight();
		size.y = lineHeight;

		if (text.empty()) return size;

		uint32_t prevCodePoint = 0;
		float cursorX = 0.F;

		auto beginIter = text.begin();
		auto endIter = utf8::find_invalid(text.begin(), text.end()); // Find invalid utf8

		while (beginIter != endIter) {
			uint32_t point = utf8::next(beginIter, endIter); // get codepoint

			auto* const glyph = this->getGlyph(point);
			if (glyph == nullptr) continue;

			if (point == '\n') {
				size.y += lineHeight;
				cursorX = 0;
				prevCodePoint = 0;
			}

			float kerning = this->getKerning(prevCodePoint, point);
			cursorX += kerning;
			cursorX += glyph->advance.x;

			if (cursorX > size.x) size.x = cursorX;
			prevCodePoint = point;
		}

		return size;
	}

	rawrbox::TexturePack* Font::getPackTexture(rawrbox::Glyph* g) const {
		if (g == nullptr) return nullptr;
		return rawrbox::TextEngine::getPack(g->packID);
	}

	void Font::render(const std::string& text, const rawrbox::Vector2f& pos, bool yIsUp, const std::function<void(rawrbox::Glyph*, float, float, float, float)>& renderGlyph) const {
		if (renderGlyph == nullptr) RAWRBOX_CRITICAL("Failed to render glyph! Missing 'renderGlyph' param");

		auto info = this->getFontInfo();
		const float lineHeight = this->getLineHeight();

		rawrbox::Vector2f cursor = {pos.x, pos.y + lineHeight + info.descender};
		uint32_t prevCodePoint = 0;

		auto beginIter = text.begin();
		auto endIter = utf8::find_invalid(text.begin(), text.end()); // Find invalid utf8

		while (beginIter != endIter) {
			uint32_t point = utf8::next(beginIter, endIter); // get codepoint
			if (point == L'\n') {
				if (!yIsUp) {
					cursor.y += lineHeight;
				} else {
					cursor.y -= lineHeight;
				}

				cursor.x = pos.x;
				prevCodePoint = 0;
				continue;
			}

			auto* const glyph = this->getGlyph(point);
			float kerning = this->getKerning(prevCodePoint, point);
			cursor.x += kerning;

			float x0 = cursor.x + glyph->offset.x;
			float y0 = cursor.y + glyph->offset.y;
			float x1 = x0 + glyph->size.x;
			float y1 = y0 + glyph->size.y;

			renderGlyph(glyph, x0, y0, x1, y1);

			cursor.x += glyph->advance.x;
			prevCodePoint = point;
		}
	}
	// ----

	// GLOBAL UTILS ---
	size_t Font::getByteCount(const std::string& text, size_t characterPosition) {
		if (characterPosition <= 0) return 0;

		size_t count = 0;
		auto beginIter = text.begin();
		auto endIter = text.end();
		while (beginIter != endIter) {
			utf8::next(beginIter, endIter);

			count++;
			if (characterPosition == count) {
				return std::distance(text.begin(), beginIter);
			}
		}

		return text.size();
	}

	size_t Font::getCharacterCount(const std::string& text) {
		size_t count = 0;

		auto beginIter = text.begin();
		auto endIter = text.end();
		while (beginIter != endIter) {
			utf8::next(beginIter, endIter);
			count++;
		}

		return count;
	}

	std::string Font::toUTF8(const std::wstring& text) {
		std::string result;

		utf8::utf16to8(text.begin(), text.end(), std::back_inserter(result));
		return result;
	}
	// -------------
} // namespace rawrbox
