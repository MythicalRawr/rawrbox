#pragma once

#include <rawrbox/render/materials/unlit.hpp>
#include <rawrbox/render/utils/topology.hpp>

namespace rawrbox {

	class MeshUtils {

	public:
		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialBase>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateBBOX(const rawrbox::Vector3f& pos, const rawrbox::BBOXf& bbox) {
			auto mesh = generateCube<M>(pos, bbox.size(), rawrbox::Colorf(1.0F, 0.1F, 0.1F, 1.0F));
			mesh.setWireframe(true);

			return mesh;
		}

		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialBase>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateLine(const rawrbox::Vector3f& a, const rawrbox::Vector3f& b, const rawrbox::Color& cl) {
			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};

			mesh.lineMode = true;
			mesh.setOptimizable(false);

			if constexpr (supportsNormals<typename M::vertexBufferType>) {
				mesh.vertices = {
				    rawrbox::VertexNormData(a, {}, {}, {}),
				    rawrbox::VertexNormData(b, {}, {}, {}),
				};
			} else {
				mesh.vertices = {
				    rawrbox::VertexData(a, {}),
				    rawrbox::VertexData(b, {}),
				};
			}

			mesh.indices = {0, 1};

			mesh.baseVertex = 0;
			mesh.baseIndex = 0;
			mesh.totalVertex = 2;
			mesh.totalIndex = 2;

			mesh.setColor(cl);
			return mesh;
		}

		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialBase>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateTriangle(const rawrbox::Vector3f& pos, const rawrbox::Vector3f& a, const rawrbox::Vector2f& aUV, const rawrbox::Vector3f& b, const rawrbox::Vector2f& bUV, const rawrbox::Vector3f& c, const rawrbox::Vector2f& cUV, const rawrbox::Color& col = rawrbox::Colors::White()) {
			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};

			rawrbox::Vector3f norm = -rawrbox::Vector3f::forward();
			if constexpr (supportsNormals<typename M::vertexBufferType>) {
				mesh.vertices = {
				    rawrbox::VertexNormData(pos + a, aUV, norm, norm),
				    rawrbox::VertexNormData(pos + b, bUV, norm, norm),
				    rawrbox::VertexNormData(pos + c, cUV, norm, norm),
				};
			} else {
				mesh.vertices = {
				    rawrbox::VertexData(pos + a, aUV),
				    rawrbox::VertexData(pos + b, bUV),
				    rawrbox::VertexData(pos + c, cUV),
				};
			};

			mesh.indices = {0, 1, 2};

			mesh.baseVertex = 0;
			mesh.baseIndex = 0;
			mesh.totalVertex = 3;
			mesh.totalIndex = 3;

			// AABB ---
			auto calcAABB = [&mesh](const rawrbox::Vector3f& point) -> void {
				if (point.x < mesh.bbox._min.x) mesh.bbox._min.x = point.x;
				if (point.y < mesh.bbox._min.y) mesh.bbox._min.y = point.y;
				if (point.z < mesh.bbox._min.z) mesh.bbox._min.z = point.z;

				if (point.x > mesh.bbox._max.x) mesh.bbox._max.x = point.x;
				if (point.y > mesh.bbox._max.y) mesh.bbox._max.y = point.y;
				if (point.z > mesh.bbox._max.z) mesh.bbox._max.z = point.z;
			};

			calcAABB(a);
			calcAABB(b);
			calcAABB(c);

			mesh.bbox._size = mesh.bbox._min.abs() + mesh.bbox._max.abs();
			// -----

			mesh.setColor(col);
			return mesh;
		}

		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialBase>)
		static rawrbox::Mesh<typename M::vertexBufferType> generatePlane(const rawrbox::Vector3f& pos, const rawrbox::Vector2f& size, const rawrbox::Colorf& cl = rawrbox::Colors::White()) {
			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};
			rawrbox::Vector2f hSize = size / 2.F;
			rawrbox::Vector3f norm = -rawrbox::Vector3f::forward();

			if constexpr (supportsNormals<typename M::vertexBufferType>) {
				mesh.vertices = {
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, 0), rawrbox::Vector2f(0, 1), norm, norm),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, hSize.y, 0), rawrbox::Vector2f(1, 0), norm, norm),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, 0), rawrbox::Vector2f(0, 0), norm, norm),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, 0), rawrbox::Vector2f(1, 1), norm, norm),
				};
			} else {
				mesh.vertices = {
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, 0), rawrbox::Vector2f(0, 1)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, hSize.y, 0), rawrbox::Vector2f(1, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, 0), rawrbox::Vector2f(0, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, 0), rawrbox::Vector2f(1, 1)),
				};
			}

			mesh.indices = {
			    0, 1, 2,
			    0, 3, 1};

			mesh.baseVertex = 0;
			mesh.baseIndex = 0;

			mesh.totalVertex = 4;
			mesh.totalIndex = 6;

			// AABB ---
			mesh.bbox._min = {-hSize.x, -hSize.y, 0};
			mesh.bbox._max = {hSize.x, hSize.y, 0};
			mesh.bbox._size = mesh.bbox._min.abs() + mesh.bbox._max.abs();
			// -----

			mesh.setColor(cl);
			return mesh;
		}

		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialBase>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateCube(const rawrbox::Vector3f& pos, const rawrbox::Vector3f& size, const rawrbox::Colorf& cl = rawrbox::Colors::White()) {
			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};
			rawrbox::Vector3f hSize = size / 2.F;

			auto nmrlB = rawrbox::Vector3f(0, 0, 1);
			auto nmrlF = rawrbox::Vector3f(0, 0, -1);
			auto nmrlR = rawrbox::Vector3f(1, 0, 0);
			auto nmrlL = rawrbox::Vector3f(-1, 0, 0);
			auto nmrlT = rawrbox::Vector3f(0, 1, 0);
			auto nmrlBT = rawrbox::Vector3f(0, -1, 0);

			if constexpr (supportsNormals<typename M::vertexBufferType>) {
				mesh.vertices = {
				    // Back
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(0, 0), nmrlB, nmrlB),   // A
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(1, 1), nmrlB, nmrlB), // B
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(1, 0), nmrlB, nmrlB),  // C
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(0, 1), nmrlB, nmrlB),  // D

				    // Front
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(0, 0), nmrlF, nmrlF),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(1, 1), nmrlF, nmrlF),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(1, 0), nmrlF, nmrlF),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(0, 1), nmrlF, nmrlF),

				    // Right
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(0, 0), nmrlR, nmrlR),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(1, 1), nmrlR, nmrlR),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(1, 0), nmrlR, nmrlR),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(0, 1), nmrlR, nmrlR),

				    // Left
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(0, 0), nmrlL, nmrlL),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(1, 1), nmrlL, nmrlL),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(1, 0), nmrlL, nmrlL),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(0, 1), nmrlL, nmrlL),

				    // Top
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(0, 0), nmrlT, nmrlT),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(1, 1), nmrlT, nmrlT),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(1, 0), nmrlT, nmrlT),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(0, 1), nmrlT, nmrlT),

				    // Bottom
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(0, 0), nmrlBT, nmrlBT),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(1, 1), nmrlBT, nmrlBT),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(1, 0), nmrlBT, nmrlBT),
				    rawrbox::VertexNormData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(0, 1), nmrlBT, nmrlBT)};
			} else {
				mesh.vertices = {
				    // Back
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(0, 0)),   // A
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(1, 1)), // B
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(1, 0)),  // C
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(0, 1)),  // D

				    // Front
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(0, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(1, 1)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(1, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(0, 1)),

				    // Right
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(0, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(1, 1)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(1, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(0, 1)),

				    // Left
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(0, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(1, 1)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(1, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(0, 1)),

				    // Top
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(0, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(1, 1)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, hSize.y, hSize.z), rawrbox::Vector2f(1, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, hSize.y, -hSize.z), rawrbox::Vector2f(0, 1)),

				    // Bottom
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(0, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(1, 1)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(hSize.x, -hSize.y, -hSize.z), rawrbox::Vector2f(1, 0)),
				    rawrbox::VertexData(pos + rawrbox::Vector3f(-hSize.x, -hSize.y, hSize.z), rawrbox::Vector2f(0, 1))};
			}

			std::vector<uint16_t> inds = {};
			constexpr std::array<uint16_t, 6> cubeInd = {
			    0, 1, 2,
			    0, 3, 1};

			for (uint16_t v = 0; v < 24; v += 4) {
				for (const auto& ind : cubeInd) {
					inds.push_back(ind + v);
				}
			}

			mesh.baseVertex = 0;
			mesh.baseIndex = 0;
			mesh.totalVertex = static_cast<uint16_t>(mesh.vertices.size());
			mesh.totalIndex = static_cast<uint16_t>(inds.size());

			// AABB ---
			mesh.bbox._min = -hSize;
			mesh.bbox._max = hSize;
			mesh.bbox._size = mesh.bbox._min.abs() + mesh.bbox._max.abs();
			// -----

			mesh.indices.insert(mesh.indices.end(), inds.begin(), inds.end());
			mesh.setColor(cl);
			return mesh;
		}

		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialBase>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateArrow(float size, const rawrbox::Vector3f& pos, const rawrbox::Colorf& cl = rawrbox::Colors::White()) {
			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};

			float hSize = 0.5F;
			auto offset = rawrbox::Vector3f(0, hSize / 2.F, 0) * size;

			mesh.merge(generateCone<M>(pos + offset, rawrbox::Vector3f(hSize, 1.F, hSize) * size, 6, cl));
			mesh.merge(generateCube<M>(pos - offset, rawrbox::Vector3f(0.15F, hSize, 0.15F) * size, cl));

			// AABB ---
			mesh.bbox._min = rawrbox::Vector3f(-hSize, -hSize, -hSize) * size;
			mesh.bbox._max = rawrbox::Vector3f(hSize, hSize, hSize) * size;
			mesh.bbox._size = mesh.bbox._min.abs() + mesh.bbox._max.abs();
			// -----

			return mesh;
		}

		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialBase>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateCone(const rawrbox::Vector3f& pos, const rawrbox::Vector3f& size, const uint16_t ratio = 11, const rawrbox::Colorf& cl = rawrbox::Colors::White()) {
			if (ratio < 3) throw rawrbox::Logger::err("RawrBox-MeshUtils", "'generateCone' ratio '{}' needs to be at least 3", ratio);

			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};

			const float step = rawrbox::pi<float> * 2.0F / ratio;
			const float radius = size.x / 2.F;
			const float height = size.y / 4.F;

			if constexpr (supportsNormals<typename M::vertexBufferType>) {
				mesh.vertices.push_back(rawrbox::VertexNormData(rawrbox::Vector3f(0, height, 0) + pos, rawrbox::Vector2f(0.5F, 0.5F), rawrbox::Vector3f(0, 1, 0)));
				mesh.vertices.push_back(rawrbox::VertexNormData(rawrbox::Vector3f(0, -height, 0) + pos, rawrbox::Vector2f(0.5F, 0.5F), rawrbox::Vector3f(0, -1, 0)));
			} else {
				mesh.vertices.push_back(rawrbox::VertexData(rawrbox::Vector3f(0, height, 0) + pos, rawrbox::Vector2f(0.5F, 0.5F)));
				mesh.vertices.push_back(rawrbox::VertexData(rawrbox::Vector3f(0, -height, 0) + pos, rawrbox::Vector2f(0.5F, 0.5F)));
			}

			for (uint16_t i = 0; i <= ratio; ++i) {
				const float angle = step * i;
				const float x = std::cos(angle) * radius;
				const float z = std::sin(angle) * radius;

				rawrbox::Vector3f bottomVertex = rawrbox::Vector3f(x, -height, z) + pos;
				rawrbox::Vector3f normal = rawrbox::Vector3f(x, radius, z).normalized();

				// Circular UV mapping for the bottom cap
				float u = 0.5F + std::cos(angle) * 0.5F;
				float v = 0.5F + std::sin(angle) * 0.5F;

				// Seamless UV mapping for the sides
				float sideV = static_cast<float>(i) / ratio;

				mesh.vertices.push_back(rawrbox::VertexNormData(bottomVertex, rawrbox::Vector2f(u, v), normal));
				mesh.vertices.push_back(rawrbox::VertexNormData(bottomVertex, rawrbox::Vector2f(sideV, 0.0F), normal));

				if (i < ratio) {
					// Bottom cap (inverted culling)
					mesh.indices.push_back(1);
					mesh.indices.push_back(i * 2 + 4);
					mesh.indices.push_back(i * 2 + 2);

					// Side faces
					mesh.indices.push_back(0);
					mesh.indices.push_back(i * 2 + 3);
					mesh.indices.push_back(i * 2 + 5);
				}
			}

			mesh.baseVertex = 0;
			mesh.baseIndex = 0;
			mesh.totalVertex = static_cast<uint16_t>(mesh.vertices.size());
			mesh.totalIndex = static_cast<uint16_t>(mesh.indices.size());

			// AABB ---
			mesh.bbox._min = rawrbox::Vector3f(-radius, -height, -radius) + pos;
			mesh.bbox._max = rawrbox::Vector3f(radius, height, radius) + pos;
			mesh.bbox._size = mesh.bbox._max - mesh.bbox._min;
			// -----

			mesh.setColor(cl);
			return mesh;
		}

		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialBase>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateCylinder(const rawrbox::Vector3f& pos, const rawrbox::Vector3f& size, const uint16_t ratio = 11, const rawrbox::Colorf& cl = rawrbox::Colors::White()) {
			if (ratio < 3) throw rawrbox::Logger::err("RawrBox-MeshUtils", "'generateCylinder' ratio '{}' needs to be at least 3", ratio);

			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};

			const float step = rawrbox::pi<float> * 2.0F / ratio;
			const float radius = size.x / 2.F;
			const float halfHeight = size.y / 2.F;

			if constexpr (supportsNormals<typename M::vertexBufferType>) {
				mesh.vertices.push_back(rawrbox::VertexNormData(pos + rawrbox::Vector3f(0, halfHeight, 0), rawrbox::Vector2f(0.5F, 0.5F), rawrbox::Vector3f(0, 1, 0)));
				mesh.vertices.push_back(rawrbox::VertexNormData(pos - rawrbox::Vector3f(0, halfHeight, 0), rawrbox::Vector2f(0.5F, 0.5F), rawrbox::Vector3f(0, -1, 0)));
			} else {
				mesh.vertices.push_back(rawrbox::VertexData(pos + rawrbox::Vector3f(0, halfHeight, 0), rawrbox::Vector2f(0.5F, 0.5F)));
				mesh.vertices.push_back(rawrbox::VertexData(pos - rawrbox::Vector3f(0, halfHeight, 0), rawrbox::Vector2f(0.5F, 0.5F)));
			}

			for (uint16_t i = 0; i <= ratio; ++i) {
				const float angle = step * i;
				const float x = std::cos(angle) * radius;
				const float z = std::sin(angle) * radius;

				rawrbox::Vector3f topVertex = pos + rawrbox::Vector3f(x, halfHeight, z);
				rawrbox::Vector3f bottomVertex = pos + rawrbox::Vector3f(x, -halfHeight, z);
				rawrbox::Vector3f normal = rawrbox::Vector3f(x, 0, z).normalized();

				// Circular UV mapping for top and bottom caps
				float u = 0.5F + std::cos(angle) * 0.5F;
				float v = 0.5F + std::sin(angle) * 0.5F;

				// Linear UV mapping for sides
				float sideU = static_cast<float>(i) / ratio;

				if constexpr (supportsNormals<typename M::vertexBufferType>) {
					mesh.vertices.push_back(rawrbox::VertexNormData(topVertex, rawrbox::Vector2f(u, v), normal));
					mesh.vertices.push_back(rawrbox::VertexNormData(bottomVertex, rawrbox::Vector2f(u, v), normal));
					mesh.vertices.push_back(rawrbox::VertexNormData(topVertex, rawrbox::Vector2f(sideU, 1.0F), normal));
					mesh.vertices.push_back(rawrbox::VertexNormData(bottomVertex, rawrbox::Vector2f(sideU, 0.0F), normal));
				} else {
					mesh.vertices.push_back(rawrbox::VertexData(topVertex, rawrbox::Vector2f(u, v)));
					mesh.vertices.push_back(rawrbox::VertexData(bottomVertex, rawrbox::Vector2f(u, v)));
					mesh.vertices.push_back(rawrbox::VertexData(topVertex, rawrbox::Vector2f(sideU, 1.0F)));
					mesh.vertices.push_back(rawrbox::VertexData(bottomVertex, rawrbox::Vector2f(sideU, 0.0F)));
				}

				if (i < ratio) {
					// Top cap
					mesh.indices.push_back(0);
					mesh.indices.push_back(i * 4 + 2);
					mesh.indices.push_back(i * 4 + 6);

					// Bottom cap
					mesh.indices.push_back(1);
					mesh.indices.push_back(i * 4 + 7);
					mesh.indices.push_back(i * 4 + 3);

					// Side faces
					mesh.indices.push_back(i * 4 + 4);
					mesh.indices.push_back(i * 4 + 5);
					mesh.indices.push_back(i * 4 + 9);

					mesh.indices.push_back(i * 4 + 4);
					mesh.indices.push_back(i * 4 + 9);
					mesh.indices.push_back(i * 4 + 8);
				}
			}

			mesh.baseVertex = 0;
			mesh.baseIndex = 0;
			mesh.totalVertex = static_cast<uint16_t>(mesh.vertices.size());
			mesh.totalIndex = static_cast<uint16_t>(mesh.indices.size());

			// AABB ---
			mesh.bbox._min = pos - rawrbox::Vector3f(radius, halfHeight, radius);
			mesh.bbox._max = pos + rawrbox::Vector3f(radius, halfHeight, radius);
			mesh.bbox._size = mesh.bbox._max - mesh.bbox._min;
			// -----

			mesh.setColor(cl);
			return mesh;
		}

		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialUnlit>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateSphere(const rawrbox::Vector3f& pos, const rawrbox::Vector3f& size, float ratio = 1, const rawrbox::Colorf& cl = rawrbox::Colors::White()) {
			if (ratio <= 0.F) throw rawrbox::Logger::err("RawrBox-MeshUtils", "'generateSphere' ratio '{}' cannot be less than 0", ratio);

			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};

			std::vector<typename M::vertexBufferType> buff = {};
			std::vector<uint16_t> inds = {};

			auto sphereSize = size / 2;

			const auto numSegments = static_cast<uint16_t>(18 * ratio);
			const auto numParallels = static_cast<uint16_t>(9 * ratio);

			for (uint16_t i = 0; i <= numParallels; ++i) {
				float v = static_cast<float>(i) / static_cast<float>(numParallels);
				float phi = v * rawrbox::pi<float>;

				for (uint16_t j = 0; j <= numSegments; ++j) {
					float u = static_cast<float>(j) / static_cast<float>(numSegments);
					float theta = u * rawrbox::pi<float> * 2.0F;

					float x = std::sin(phi) * std::cos(theta) * sphereSize.x;
					float y = std::cos(phi) * sphereSize.y;
					float z = std::sin(phi) * std::sin(theta) * sphereSize.z;

					rawrbox::Vector3f vertexPos = {x, y, z};

					if constexpr (supportsNormals<typename M::vertexBufferType>) {
						buff.push_back(rawrbox::VertexNormData(vertexPos + pos, {u, v}, vertexPos.normalized()));
					} else {
						buff.push_back(rawrbox::VertexData(vertexPos + pos, {u, v}));
					}
				}
			}

			for (uint16_t i = 0; i < numParallels; ++i) {
				for (uint16_t j = 0; j < numSegments; ++j) {
					uint16_t a = i * (numSegments + 1) + j;
					uint16_t b = a + 1;
					uint16_t c = (i + 1) * (numSegments + 1) + j;
					uint16_t d = c + 1;

					inds.push_back(a);
					inds.push_back(d);
					inds.push_back(b);

					inds.push_back(a);
					inds.push_back(c);
					inds.push_back(d);
				}
			}

			mesh.baseVertex = 0;
			mesh.baseIndex = 0;
			mesh.totalVertex = static_cast<uint16_t>(buff.size());
			mesh.totalIndex = static_cast<uint16_t>(inds.size());

			// AABB ---
			mesh.bbox._min = -sphereSize;
			mesh.bbox._max = sphereSize;
			mesh.bbox._size = mesh.bbox._min.abs() + mesh.bbox._max.abs();
			// -----

			mesh.vertices.insert(mesh.vertices.end(), buff.begin(), buff.end());
			mesh.indices.insert(mesh.indices.end(), inds.begin(), inds.end());

			mesh.setColor(cl);
			return mesh;
		}

		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialBase>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateMesh(const rawrbox::Vector3f& pos, const rawrbox::Vector2f& size, uint16_t subDivs = 1, const rawrbox::Colorf& cl = rawrbox::Colors::White()) {
			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};

			auto uvScale = 1.0F / static_cast<float>(subDivs - 1);
			const uint16_t vertSize = subDivs * subDivs;
			mesh.vertices.reserve(vertSize);

			for (uint32_t y = 0; y < subDivs; y++) {
				for (uint32_t x = 0; x < subDivs; x++) {
					auto xF = static_cast<float>(x);
					auto yF = static_cast<float>(y);

					rawrbox::Vector2f posDiv = {xF, yF};
					posDiv /= static_cast<float>(subDivs - 1);
					posDiv *= size;
					posDiv -= size / 2;

					if constexpr (supportsNormals<typename M::vertexBufferType>) {
						mesh.vertices.push_back(rawrbox::VertexNormData(
						    Vector3f(posDiv.x + pos.x, pos.y, posDiv.y + pos.z),
						    rawrbox::Vector2(uvScale * xF,
							uvScale * yF),
						    rawrbox::Vector3f(0, 1, 0), {}));
					} else {
						mesh.vertices.push_back(rawrbox::VertexData(
						    pos + Vector3f(posDiv.x + pos.x, pos.y, posDiv.y + pos.z),
						    rawrbox::Vector2(uvScale * xF,
							uvScale * yF)));
					}
				}
			}

			const uint16_t indcSize = ((subDivs - 1) * (subDivs - 1)) * 6;
			mesh.indices.reserve(indcSize);

			for (size_t y = 0; y < subDivs - 1; y++) {
				auto yOffset = static_cast<uint16_t>(y * subDivs);

				for (size_t x = 0; x < subDivs - 1; x++) {
					uint16_t index = yOffset + static_cast<uint16_t>(x);

					mesh.indices.push_back(index + 1);
					mesh.indices.push_back(index + subDivs);
					mesh.indices.push_back(index);

					mesh.indices.push_back(index + subDivs + 1);
					mesh.indices.push_back(index + subDivs);
					mesh.indices.push_back(index + 1);
				}
			}

			mesh.baseVertex = 0;
			mesh.baseIndex = 0;
			mesh.totalVertex = vertSize;
			mesh.totalIndex = indcSize;

			mesh.setColor(cl);
			return mesh;
		}

		// Adapted from : https://stackoverflow.com/questions/58494179/how-to-create-a-grid-in-opengl-and-drawing-it-with-lines
		template <typename M = rawrbox::MaterialUnlit>
			requires(std::derived_from<M, rawrbox::MaterialUnlit>)
		static rawrbox::Mesh<typename M::vertexBufferType> generateGrid(uint16_t size, const rawrbox::Vector3f& pos, const rawrbox::Colorf& cl = rawrbox::Colors::Gray().strength(0.4F), const rawrbox::Colorf& borderCl = rawrbox::Colors::Transparent()) {
			if constexpr (supportsNormals<typename M::vertexBufferType>) {
				throw rawrbox::Logger::err("RawrBox-MeshUtils", "'generateGrid' does not support normals");
			}

			rawrbox::Mesh<typename M::vertexBufferType> mesh = {};

			const uint16_t vertSize = size * size;
			mesh.vertices.reserve(vertSize);

			float step = 1.F;
			for (uint16_t j = 0; j <= size; ++j) {
				for (uint16_t i = 0; i <= size; ++i) {
					float x = static_cast<float>(i) / static_cast<float>(step);
					float y = 0;
					float z = static_cast<float>(j) / static_cast<float>(step);
					auto col = cl;

					if (j == 0 || i == 0 || j >= size || i >= size) col = borderCl;
					mesh.vertices.push_back(rawrbox::VertexData(pos + rawrbox::Vector3f(pos.x - static_cast<float>(size / 2), pos.y, pos.z - static_cast<float>(size / 2)) + rawrbox::Vector3f(x, y, z), rawrbox::Vector2f(0, 0)));
				}
			}

			const uint16_t indcSize = vertSize * 8;
			mesh.indices.reserve(indcSize);

			for (uint16_t j = 0; j < size; ++j) {
				for (uint16_t i = 0; i < size; ++i) {

					uint16_t row1 = j * (size + 1);
					uint16_t row2 = (j + 1) * (size + 1);

					mesh.indices.push_back(row1 + i);
					mesh.indices.push_back(row1 + i + 1);
					mesh.indices.push_back(row1 + i + 1);
					mesh.indices.push_back(row2 + i + 1);

					mesh.indices.push_back(row2 + i + 1);
					mesh.indices.push_back(row2 + i);
					mesh.indices.push_back(row2 + i);
					mesh.indices.push_back(row1 + i);
				}
			}

			mesh.baseVertex = 0;
			mesh.baseIndex = 0;
			mesh.totalVertex = vertSize;
			mesh.totalIndex = indcSize;
			mesh.lineMode = true;

			mesh.setColor(cl);
			return mesh;
		}
	};
} // namespace rawrbox
