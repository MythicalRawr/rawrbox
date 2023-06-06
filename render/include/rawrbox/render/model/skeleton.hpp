#pragma once
#include <rawrbox/math/matrix4x4.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace rawrbox {

	struct Bone;
	struct Skeleton {
		uint8_t boneIndex = 0;

		std::string name;
		std::unique_ptr<rawrbox::Bone> rootBone = nullptr;

		std::unordered_map<std::string, rawrbox::Bone*> boneMap = {}; // Map for quick lookup
		rawrbox::Matrix4x4 invTransformationMtx = {};

		explicit Skeleton(std::string _name) : name(std::move(_name)) {}
		~Skeleton() {
			this->rootBone.reset();
			this->boneMap.clear();
		}
	};

	struct Bone {
		std::string name;
		uint8_t boneId = 0;

		// Rendering ---
		rawrbox::Matrix4x4 transformationMtx = {};
		rawrbox::Matrix4x4 offsetMtx = {};
		// ----

		// Lookup ----
		rawrbox::Skeleton* owner = nullptr;
		rawrbox::Bone* parent = nullptr;

		std::vector<std::unique_ptr<rawrbox::Bone>> children = {};
		// ----

		explicit Bone(std::string _name) : name(std::move(_name)) {}
		~Bone() {
			this->parent = nullptr;
			this->owner = nullptr;

			this->children.clear();
		}
	};
} // namespace rawrbox
