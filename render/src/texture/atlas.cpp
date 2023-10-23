
#include <rawrbox/render/static.hpp>
#include <rawrbox/render/texture/atlas.hpp>
#include <rawrbox/render/utils/texture.hpp>

#include <fmt/format.h>

namespace rawrbox {
	// NOLINTBEGIN(modernize-pass-by-value)
	TextureAtlas::TextureAtlas(const std::filesystem::path& filePath, const std::vector<uint8_t>& buffer, uint16_t spriteSize, bool useFallback) : TextureImage::TextureImage(filePath, buffer, useFallback), _spriteSize(spriteSize) {
		this->processAtlas();
	}

	TextureAtlas::TextureAtlas(const std::filesystem::path& filePath, uint16_t spriteSize, bool useFallback) : TextureImage::TextureImage(filePath, useFallback), _spriteSize(spriteSize) {
		this->processAtlas();
	}
	// NOLINTEND(modernize-pass-by-value)
	// --------------------

	// ------ PIXEL-UTILS
	size_t TextureAtlas::total() const {
		rawrbox::Vector2i totalSprites = this->_size / this->_spriteSize;
		return static_cast<size_t>(totalSprites.x * totalSprites.y);
	}

	uint16_t TextureAtlas::getSpriteSize() const {
		return this->_spriteSize;
	}

	std::vector<uint8_t> TextureAtlas::getSprite(size_t id) const {
		if (id >= this->_tiles.size()) throw std::runtime_error(fmt::format("[RawrBox-Atlas] Invalid ID {}", id));
		return this->_tiles[id];
	}
	// --------------------

	void TextureAtlas::processAtlas() {
		int totalPixels = this->_spriteSize * this->_spriteSize * this->_channels;

		int tilesX = this->_size.x / this->_spriteSize;
		int tilesY = this->_size.y / this->_spriteSize;

		int tileSizeX = this->_spriteSize * this->_channels;
		int rowLen = tilesX * tileSizeX;

		this->_tiles.clear();
		this->_tiles.resize(this->total());

		for (size_t i = 0; i < this->total(); ++i) {
			auto& pix = this->_tiles[i];
			pix.resize(this->_spriteSize * this->_spriteSize * this->_channels);

			auto y = i / tilesX;
			auto x = i - y * tilesY;

			for (int iy = 0; iy < this->_spriteSize; iy++) {
				auto offset = (iy + y * this->_spriteSize) * this->_size.x * _channels;
				offset += x * this->_spriteSize * this->_channels;

				std::copy(this->_pixels.begin() + offset, this->_pixels.begin() + offset + tileSizeX, pix.begin() + iy * tileSizeX);
			}
		}

		this->_name = "RawrBox::Texture::ATLAS";
	}

	void TextureAtlas::upload(Diligent::TEXTURE_FORMAT format, bool dynamic) {
		if (this->_failedToLoad || this->_handle != nullptr) return; // Failed texture is already bound, so skip it

		// Try to determine texture format
		if (format == Diligent::TEXTURE_FORMAT::TEX_FORMAT_UNKNOWN) {
			switch (this->_channels) {
				case 1:
					format = Diligent::TEXTURE_FORMAT::TEX_FORMAT_R8_UNORM;
				case 2:
					format = Diligent::TEXTURE_FORMAT::TEX_FORMAT_RG8_UNORM;
				default:
				case 3:
				case 4:
					format = this->_sRGB ? Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM_SRGB : Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM;
			}
		} else if (this->_channels == 0) {
			switch (format) {
				case Diligent::TEXTURE_FORMAT::TEX_FORMAT_A8_UNORM:
				case Diligent::TEXTURE_FORMAT::TEX_FORMAT_R8_UNORM:
					this->_channels = 1;
					break;
				case Diligent::TEXTURE_FORMAT::TEX_FORMAT_RG8_UNORM:
					this->_channels = 2;
					break;
				default:
				case Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM:
				case Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM_SRGB:
					this->_channels = 4;
					break;
			}
		}

		Diligent::TextureDesc desc;
		desc.Type = Diligent::RESOURCE_DIM_TEX_2D_ARRAY;
		desc.BindFlags = Diligent::BIND_SHADER_RESOURCE;
		desc.Usage = dynamic ? Diligent::USAGE_DEFAULT : Diligent::USAGE_IMMUTABLE;
		desc.CPUAccessFlags = Diligent::CPU_ACCESS_NONE;
		desc.Width = this->_spriteSize;
		desc.Height = this->_spriteSize;
		desc.MipLevels = 1;
		desc.Format = format;
		// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
		desc.ArraySize = this->total();
		// NOLINTEND(cppcoreguidelines-pro-type-union-access)
		desc.Name = this->_name.c_str();

		// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
		std::vector<Diligent::TextureSubResData> subresData(desc.ArraySize);
		// NOLINTEND(cppcoreguidelines-pro-type-union-access)

		for (uint32_t slice = 0; slice < subresData.size(); slice++) {
			auto& res = subresData[slice];

			res.pData = this->_tiles[slice].data();
			res.Stride = this->_spriteSize * this->_channels;
		}

		Diligent::TextureData data;
		data.pSubResources = subresData.data();
		data.NumSubresources = subresData.size();

		rawrbox::RENDERER->device()->CreateTexture(desc, &data, &this->_tex);
		this->_handle = this->_tex->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE);
	}
} // namespace rawrbox
