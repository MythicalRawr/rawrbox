
#include <rawrbox/ui/base.hpp>
#include <rawrbox/ui/container.hpp>

namespace rawrbox {

	// UTILS ---
	void UIContainer::setPos(const rawrbox::Vector2f& pos) { this->_aabb.pos = pos; }
	const rawrbox::Vector2f& UIContainer::getPos() const { return this->_aabb.pos; }
	void UIContainer::setSize(const rawrbox::Vector2f& size) { this->_aabb.size = size; }
	const rawrbox::Vector2f& UIContainer::getSize() const { return this->_aabb.size; }

	void UIContainer::removeChildren() {
		while (!this->_children.empty())
			this->_children.front()->remove();
	}

	void UIContainer::remove() {
		this->removeChildren();

		auto p = this->_parent.lock();
		auto& parentChilds = p->getChildren();
		auto sharedPtr = getRef<UIBase>();
		parentChilds.erase(std::find(parentChilds.begin(), parentChilds.end(), sharedPtr));

		this->_parent.reset();
	}
	// ---

	// REFERENCE HANDLING --
	void UIContainer::setRef(const std::shared_ptr<UIContainer>& ref) { this->_ref = ref; }
	// ---

	// PARENTING ---
	std::vector<std::shared_ptr<UIBase>>& UIContainer::getChildren() { return this->_children; }
	const std::vector<std::shared_ptr<UIBase>>& UIContainer::getChildren() const { return this->_children; }

	bool UIContainer::hasParent() const { return !this->_parent.expired(); }
	void UIContainer::addChild(const std::shared_ptr<UIBase>& elm) { elm->setParent(getRef<UIBase>()); }
	void UIContainer::setParent(const std::shared_ptr<UIContainer>& elm) {
		auto sharedPtr = getRef<UIBase>();

		if (hasParent()) {
			auto& childs = this->_parent.lock()->getChildren();
			childs.erase(std::find(childs.begin(), childs.end(), sharedPtr));
		}

		auto& childn = elm->getChildren();
		if (sharedPtr->alwaysOnTop()) {
			childn.insert(childn.begin(), sharedPtr);
		} else {
			childn.push_back(sharedPtr);
		}

		this->_parent = elm;
	}
	// --------------

	// RENDERING -----
	void UIContainer::update() {
		for (auto elm : this->_children) {
			elm->update();
		}
	}

	void UIContainer::draw(rawrbox::Stencil& stencil) {
		for (auto elm : this->_children) {
			if (!elm->visible()) continue;

			stencil.pushOffset(elm->getPos());
			stencil.pushClipping({{0, 0}, elm->getSize()});

			elm->beforeDraw(stencil);
			elm->draw(stencil);
			elm->afterDraw(stencil);

			stencil.popClipping();
			stencil.popOffset();
		}
	}
	// ----
} // namespace rawrbox
