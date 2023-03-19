#include "AVL.h"

#include <cassert>
#include <iostream>
#include <string>
#include <queue>

#include "json.hpp"


AVLNode::AVLNode(int key) :
	key_(key),
	parent_(std::weak_ptr<AVLNode>()),
	left_(nullptr),
	right_(nullptr),
	height_(0),
	balance_factor_() {} 

AVLNode::AVLNode(int key, std::weak_ptr<AVLNode> parent) :
	key_(key),
	left_(nullptr),
	right_(nullptr),
	height_(0),
	balance_factor_(),
	parent_(parent) {}

bool AVLNode::IsLeaf() const {
	return left_ == nullptr && right_ == nullptr;
}

bool AVLNode::HasLeftChild() const {
	return left_ != nullptr;
}

bool AVLNode::HasRightChild() const {
	return right_ != nullptr;
}

// int AVLNode::getHeight() {
//     int leftHeight = (left_ != nullptr) ? left_->getHeight() : -1;
//     int rightHeight = (right_ != nullptr) ? right_->getHeight() : -1;
//     return 1 + std::max(leftHeight, rightHeight);
// }
int getHeight(const std::shared_ptr<AVLNode>& node) {
    if (node == nullptr) {
        return -1;
    } else {
        int leftHeight = getHeight(node->left_);
        int rightHeight = getHeight(node->right_);
        return std::max(leftHeight, rightHeight) + 1;
    }
}

// int AVLNode::getBalanceFactor() {
//     int leftHeight = (left_ != nullptr) ? left_->getHeight() : -1;
//     int rightHeight = (right_ != nullptr) ? right_->getHeight() : -1;
//     return rightHeight - leftHeight;
// }
int getBalanceFactor(std::shared_ptr<AVLNode> node) {
    if (node == nullptr) {
        return 0;
    }
    return getHeight(node->left_) - getHeight(node->right_);
}


void AVLNode::DeleteChild(std::shared_ptr<AVLNode> v) {
	if (left_ == v) {
		left_ = nullptr;
	} else if (right_ == v) {
		right_ = nullptr;
	} else {
		std::cerr << "AVLNode::DeleteChild Error: non-child passed as argument\n";
		exit(EXIT_FAILURE);
	}
}

void AVLNode::ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u) {
	if (left_ == u || right_ == u) {
		std::cerr << "AVLNode::ReplaceChild Error: child passed as replacement\n";
	}
	if (left_ == v) {
		left_ = u;
		u->parent_ = v->parent_;
	} else if (right_ == v) {
		right_ = u;
		u->parent_ = v->parent_;
	} else {
		std::cerr << "AVLNode::ReplaceChild Error: non-child passed as argument\n";
		exit(EXIT_FAILURE);
	}
}



AVL::AVL() : root_(nullptr), size_(0) {}

std::shared_ptr<AVLNode> AVL::rotateLeft(std::shared_ptr<AVLNode> node) {
    std::shared_ptr<AVLNode> rightChild = node->right_;
    node->right_ = rightChild->left_;
    if (node->right_ != nullptr) {
        node->right_->parent_ = node;
    }
    rightChild->left_ = node;
    rightChild->parent_ = node->parent_;
    node->parent_ = rightChild;

    // Update parent's child pointer
    if (rightChild->parent_.lock() != nullptr) {
        if (rightChild->parent_.lock()->left_ == node) {
            rightChild->parent_.lock()->left_ = rightChild;
        } else {
            rightChild->parent_.lock()->right_ = rightChild;
        }
    }

    node->height_ = 1 + std::max(getHeight(node->left_), getHeight(node->right_));
    rightChild->height_ = 1 + std::max(getHeight(rightChild->left_), getHeight(rightChild->right_));
    return rightChild;
}

std::shared_ptr<AVLNode> AVL::rotateRight(std::shared_ptr<AVLNode> node) {
    std::shared_ptr<AVLNode> leftChild = node->left_;
    node->left_ = leftChild->right_;
    if (node->left_ != nullptr) {
        node->left_->parent_ = node;
    }
    leftChild->right_ = node;
    leftChild->parent_ = node->parent_;
    node->parent_ = leftChild;

    // Update parent's child pointer
    if (leftChild->parent_.lock() != nullptr) {
        if (leftChild->parent_.lock()->left_ == node) {
            leftChild->parent_.lock()->left_ = leftChild;
        } else {
            leftChild->parent_.lock()->right_ = leftChild;
        }
    }

    node->height_ = 1 + std::max(getHeight(node->left_), getHeight(node->right_));
    leftChild->height_ = 1 + std::max(getHeight(leftChild->left_), getHeight(leftChild->right_));
    return leftChild;
}


void AVL::Insert(int key) {
	//if root is null
	if (root_ == nullptr) {
		root_ = std::make_shared<AVLNode>(key);
		size_++;
		return;
	}
	std::shared_ptr<AVLNode> currentNode = root_, lastNode = nullptr;

	while (currentNode != nullptr) {
		lastNode = currentNode;
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	if (key < lastNode->key_) {
		lastNode->left_ = std::make_shared<AVLNode>(key, lastNode);
	} else {
		lastNode->right_ = std::make_shared<AVLNode>(key, lastNode);
	}
	size_++;

	//rebalance
 std::shared_ptr<AVLNode> currentNode2 = (lastNode->left_ != nullptr) ? lastNode->left_ : lastNode->right_;
while (currentNode2 != nullptr) {
    currentNode2->height_ = 1 + std::max(getHeight(currentNode2->left_), getHeight(currentNode2->right_));
    int balanceFactor = getBalanceFactor(currentNode2);

    if (balanceFactor > 1) {
        if (getBalanceFactor(currentNode2->left_) >= 0) {
            currentNode2 = rotateRight(currentNode2);
        } else {
            currentNode2->left_ = rotateLeft(currentNode2->left_);
            currentNode2 = rotateRight(currentNode2);
        }
    } else if (balanceFactor < -1) {
        if (getBalanceFactor(currentNode2->right_) <= 0) {
            currentNode2 = rotateLeft(currentNode2);
        } else {
            currentNode2->right_ = rotateRight(currentNode2->right_);
            currentNode2 = rotateLeft(currentNode2);
        }
    }

    if (currentNode2->parent_.lock() == nullptr) {
        root_ = currentNode2;
    }

    currentNode2 = currentNode2->parent_.lock();
}


}

bool AVL::Delete(int key) {
	std::shared_ptr<AVLNode> currentNode = root_;
	while (currentNode != nullptr) {
		if (currentNode->key_ == key) {
			if (currentNode->IsLeaf()) {
				DeleteLeaf(currentNode);
			} else if (currentNode->left_ == nullptr) {
				assert(currentNode->right_ != nullptr);
				std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				parent->ReplaceChild(currentNode, currentNode->right_);
				size_--; assert(size_ >= 0);
			} else if (currentNode->right_ == nullptr) {
				assert(currentNode->left_ != nullptr);
				std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				parent->ReplaceChild(currentNode, currentNode->left_);
				size_--; assert(size_ >= 0);
			} else {
				currentNode->key_ = DeleteMin(currentNode);
			}
		}
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	return false;
}

int AVL::DeleteMin() {
	return DeleteMin(root_);
}


void AVL::DeleteLeaf(std::shared_ptr<AVLNode> currentNode) {
	std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
	if (parent == nullptr) {
		// Delete root
		root_ = nullptr;
		size_--; assert(size_ == 0);
	} else {
		if (parent->right_ == currentNode) {
			parent->right_ = nullptr;
		} else if (parent->left_ == currentNode) {
			parent->left_ = nullptr;
		} else {
			std::cerr << "AVL::DeleteLeaf Error: inconsistent state\n";
		}
		size_--; assert(size_ >= 0);
	}
}

int AVL::DeleteMin(std::shared_ptr<AVLNode> currentNode) {
	std::shared_ptr<AVLNode> lastNode = nullptr;
	while (currentNode != nullptr) {
		lastNode = currentNode;
		currentNode = currentNode->left_;
	}
	int result = lastNode->key_;
	std::shared_ptr<AVLNode> parent = lastNode->parent_.lock();
	if (parent == nullptr) {
		// lastNode is root
		if (lastNode->right_ != nullptr) {
			root_ = lastNode->right_;
			lastNode->right_->parent_.reset();
		} else {
			root_ = nullptr;
		}
	} else {
		// lastNode under the root
		if (lastNode->right_ != nullptr) {
			parent->left_ = lastNode->right_;
			lastNode->right_->parent_ = parent;
		} else {
			parent->left_ = nullptr;
		}
  }
	size_--; assert(size_ >= 0);
	return result;
}

size_t AVL::size() const {
	return size_;
}

bool AVL::empty() const {
	return size_ == 0;
}

bool AVL::Find(int key) const {
	std::shared_ptr<AVLNode> currentNode = root_;
	while (currentNode != nullptr) {
		if (currentNode->key_ == key) {
			return true;
		}
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	return false;
}

std::string AVL::JSON() const {
    nlohmann::json result;
    std::queue< std::shared_ptr<AVLNode> > nodes;
    if (root_ != nullptr) {
        result["root"] = root_->key_;
        result["height"] = getHeight(root_);
        nodes.push(root_);
        while (!nodes.empty()) {
            auto v = nodes.front();
            nodes.pop();
            std::string key = std::to_string(v->key_);
            result[key]["balance factor"] = -(getBalanceFactor(v));
            result[key]["height"] = getHeight(v);
            if (v->left_ != nullptr) {
                result[key]["left"] = v->left_->key_;
                nodes.push(v->left_);
            }
            if (v->right_ != nullptr) {
                result[key]["right"] = v->right_->key_;
                nodes.push(v->right_);
            }
            if (v->parent_.lock() != nullptr) {
                result[key]["parent"] = v->parent_.lock()->key_;
            } else {
                result[key]["root"] = true;
            }
        }
    }
    result["size"] = size_;
    return result.dump(2) + "\n";
}
