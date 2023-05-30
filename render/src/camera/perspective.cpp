

#include <rawrbox/render/camera/perspective.hpp>

#include <bx/math.h>

#include <stdexcept>

namespace rawrbox {
	// NOLINTBEGIN(clang-analyzer-optin.cplusplus.VirtualCall)
	CameraPerspective::CameraPerspective(const rawrbox::Vector2i& _wsize, float FOV, float near, float far, bool homogeneousDepth) : _winSize(_wsize), _FOV(FOV), _near(near), _far(far), _homogeneousDepth(homogeneousDepth) {
		bx::mtxProj(this->_projection.data(), FOV, static_cast<float>(_wsize.x) / static_cast<float>(_wsize.y), near, far, homogeneousDepth);
		this->updateMtx();
	}
	// NOLINTEND(clang-analyzer-optin.cplusplus.VirtualCall)

	void CameraPerspective::updateMtx() {
		auto dir = this->getForward();

		auto m_at = this->_pos + dir;
		auto m_up = this->getUp();

		this->_view.lookAt(this->_pos, m_at, m_up);
	}

	const rawrbox::Vector3f CameraPerspective::worldToScreen(const rawrbox::Vector3f& pos) const {
		return rawrbox::Matrix4x4::project(pos, this->_view, this->_projection, {0, 0, this->_winSize.x, this->_winSize.y});
	}

	const rawrbox::Vector3f CameraPerspective::screenToWorld(const rawrbox::Vector2f& screen_pos) const {
		rawrbox::Vector3f plane_origin{0, 0, 0};
		rawrbox::Vector3f plane_normal{0, 1, 0};

		auto screenPos = screen_pos.cast<float>();

		// get our pos and force aim downwards, the getForward() seems to behave odd when aiming full down
		auto campos = this->getPos();

		rawrbox::Matrix4x4 viewproj_inv = this->_projection * this->_view;
		viewproj_inv.inverse();

		float screenx_clip = 2 * (screenPos.x / this->_winSize.x) - 1;
		float screeny_clip = 1 - 2 * (screenPos.y) / this->_winSize.y;

		rawrbox::Vector4f screen_clip = {screenx_clip, screeny_clip, -1, 1};
		rawrbox::Vector4f world_pos = viewproj_inv.mulVec(screen_clip);

		// divide by the weigth of the universe to resolve black mater offsets
		world_pos /= world_pos.w;

		// convert the object back to the real universe
		rawrbox::Vector3f mouse_point_world = {world_pos.x, world_pos.y, world_pos.z};
		rawrbox::Vector3f camera_forward_world = mouse_point_world - campos;

		float numerator = (plane_origin - campos).dot(plane_normal);
		float denumerator = camera_forward_world.dot(plane_normal);

		float delta = numerator / denumerator;
		return camera_forward_world * delta + campos;
	}

} // namespace rawrbox
