#pragma once

#include <rawrbox/render/materials/base.hpp>

#ifdef RAWRBOX_SCRIPTING
	// #include <rawrbox/render/scripting/wrapper/model_wrapper.hpp>
	#include <sol/sol.hpp>
#endif
namespace rawrbox {

	template <typename M = rawrbox::MaterialBase>
#ifdef RAWRBOX_SCRIPTING
	class ModelBase : public std::enable_shared_from_this<rawrbox::ModelBase<M>> {
#else
	class ModelBase {
#endif

	protected:
		bgfx::DynamicVertexBufferHandle _vbdh = BGFX_INVALID_HANDLE; // Vertices - Dynamic
		bgfx::VertexBufferHandle _vbh = BGFX_INVALID_HANDLE;         // Vertices - Static

		bgfx::DynamicIndexBufferHandle _ibdh = BGFX_INVALID_HANDLE; // Indices - Dynamic
		bgfx::IndexBufferHandle _ibh = BGFX_INVALID_HANDLE;         // Indices - Static

		std::unique_ptr<rawrbox::Mesh> _mesh = std::make_unique<rawrbox::Mesh>();
		std::unique_ptr<M> _material = std::make_unique<M>();

#ifdef RAWRBOX_SCRIPTING
		sol::table _table;
		sol::object _luaWrapper;
#endif

		// BGFX DYNAMIC SUPPORT ---
		bool _isDynamic = false;
		// ----

		virtual void updateBuffers() {
			if (!this->isDynamic() || !this->isUploaded()) return;

			const bgfx::Memory* vertMem = bgfx::makeRef(this->_mesh->vertices.data(), static_cast<uint32_t>(this->_mesh->vertices.size()) * M::vLayout().getStride());
			const bgfx::Memory* indexMem = bgfx::makeRef(this->_mesh->indices.data(), static_cast<uint32_t>(this->_mesh->indices.size()) * sizeof(uint16_t));

			bgfx::update(this->_vbdh, 0, vertMem);
			bgfx::update(this->_ibdh, 0, indexMem);
		}

	public:
		ModelBase() = default;
		ModelBase(ModelBase&&) = delete;
		ModelBase& operator=(ModelBase&&) = delete;
		ModelBase(const ModelBase&) = delete;
		ModelBase& operator=(const ModelBase&) = delete;

		virtual ~ModelBase() {
			RAWRBOX_DESTROY(this->_vbh);
			RAWRBOX_DESTROY(this->_ibh);
			RAWRBOX_DESTROY(this->_vbdh);
			RAWRBOX_DESTROY(this->_ibdh);

			this->_mesh.reset();

#ifdef RAWRBOX_SCRIPTING
			if (this->_luaWrapper.valid()) this->_luaWrapper.abandon();
			this->_table = {};
#endif
		}

		// UTIL ---
		[[nodiscard]] virtual const rawrbox::Vector3f& getPos() const { return this->_mesh->getPos(); }
		virtual void setPos(const rawrbox::Vector3f& pos) {
			this->_mesh->setPos(pos);
		}

		[[nodiscard]] virtual const rawrbox::Vector3f& getScale() const { return this->_mesh->getScale(); }
		virtual void setScale(const rawrbox::Vector3f& scale) {
			this->_mesh->setScale(scale);
		}

		[[nodiscard]] virtual const rawrbox::Vector4f& getAngle() const { return this->_mesh->getAngle(); }
		virtual void setAngle(const rawrbox::Vector4f& ang) {
			this->_mesh->setAngle(ang);
		}

		virtual void setEulerAngle(const rawrbox::Vector3f& ang) {
			this->_mesh->setEulerAngle(ang);
		}

		[[nodiscard]] virtual const rawrbox::Matrix4x4& getMatrix() const {
			return this->_mesh->matrix;
		}

		[[nodiscard]] virtual bool isDynamic() const {
			return this->_isDynamic;
		}

		[[nodiscard]] virtual bool isUploaded() const {
			if (this->isDynamic()) return bgfx::isValid(this->_ibdh) && bgfx::isValid(this->_vbdh);
			return bgfx::isValid(this->_ibh) && bgfx::isValid(this->_vbh);
		}

		// ----
		virtual void upload(bool dynamic = false) {
			if (this->isUploaded()) throw std::runtime_error("[RawrBox-ModelBase] Upload called twice");

			// Generate buffers ----
			this->_isDynamic = dynamic;

			const auto layout = M::vLayout();
			const bgfx::Memory* vertMem = bgfx::makeRef(this->_mesh->vertices.data(), static_cast<uint32_t>(this->_mesh->vertices.size()) * layout.getStride());
			const bgfx::Memory* indexMem = bgfx::makeRef(this->_mesh->indices.data(), static_cast<uint32_t>(this->_mesh->indices.size()) * sizeof(uint16_t));

			if (dynamic) {
				if (this->_mesh->empty()) {
					this->_vbdh = bgfx::createDynamicVertexBuffer(1, layout, 0 | BGFX_BUFFER_ALLOW_RESIZE);
					this->_ibdh = bgfx::createDynamicIndexBuffer(1, 0 | BGFX_BUFFER_ALLOW_RESIZE);
				} else {
					this->_vbdh = bgfx::createDynamicVertexBuffer(vertMem, layout, 0 | BGFX_BUFFER_ALLOW_RESIZE);
					this->_ibdh = bgfx::createDynamicIndexBuffer(indexMem, 0 | BGFX_BUFFER_ALLOW_RESIZE);
				}
			} else {
				if (this->_mesh->empty()) throw std::runtime_error("[RawrBox-ModelBase] Static buffer cannot contain empty vertices / indices. Use dynamic buffer instead!");

				this->_vbh = bgfx::createVertexBuffer(vertMem, layout);
				this->_ibh = bgfx::createIndexBuffer(indexMem);
			}
			// -----------------

			this->_material->upload();
			this->_material->registerUniforms();
		}

		virtual void draw() {
			if (!this->isUploaded()) throw std::runtime_error("[RawrBox-Model] Failed to render model, vertex / index buffer is not uploaded");
		}

#ifdef RAWRBOX_SCRIPTING
		virtual void initializeLua(sol::state_view lua) {
			this->_table = lua.create_table();

			if (this->_luaWrapper.valid()) this->_luaWrapper.abandon();
			// this->_luaWrapper = sol::make_object(lua, rawrbox::ModelWrapper<M>(this));
		}

		sol::object& getScriptingWrapper() {
			return this->_luaWrapper;
		}

		sol::table& getScriptingTable() {
			return this->_table;
		}
#endif
	};
} // namespace rawrbox
