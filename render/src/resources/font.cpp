
#include <rawrbox/render/resources/font.hpp>

namespace rawrbox {
	// Resource ----
	std::shared_ptr<rawrbox::Font> ResourceFont::getSize(int size, uint32_t flags) {
		return rawrbox::TextEngine::load(filePath.generic_string(), size);
	}
	// -------

	// Loader ----
	FontLoader::~FontLoader() { rawrbox::TextEngine::shutdown(); };
	std::unique_ptr<rawrbox::Resource> FontLoader::createEntry(uint32_t flags) {
		return std::make_unique<rawrbox::ResourceFont>();
	}

	bool FontLoader::canLoad(const std::string& fileExtention) {
		return fileExtention == ".ttf" ||
		       fileExtention == ".otf" ||
		       fileExtention == ".ttc" ||
		       fileExtention == ".pfa" ||
		       fileExtention == ".pfb" ||
		       fileExtention == ".ttc";
	}
	// -------
} // namespace rawrbox
