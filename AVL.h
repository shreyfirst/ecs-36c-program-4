#include <memory>
#include <string>

class AVL;

class AVLNode {
 public:
 	AVLNode(int key);
 	AVLNode(int key, std::weak_ptr<AVLNode> parent);
 	bool IsLeaf() const;
 	bool IsMissingChild() const;
 	bool HasLeftChild() const;
 	bool HasRightChild() const;
	int getBalanceFactor(std::shared_ptr<AVLNode> node);
	int getHeight(std::shared_ptr<AVLNode> node);

 	void DeleteChild(std::shared_ptr<AVLNode> v);
 	void ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u);
	std::shared_ptr<AVLNode> left_;
  	std::shared_ptr<AVLNode> right_;

 private:
  int key_;
  int height_;
  int balance_factor_;
  std::weak_ptr<AVLNode> parent_;
  

  friend AVL;
}; // class AVLNode

class AVL {
 public:
 	AVL();

 	void Insert(int key);
 	bool Delete(int key);
 	bool Find(int key) const;
 	std::string JSON() const;
 	size_t size() const;
 	bool empty() const;
 	int DeleteMin();
	std::shared_ptr<AVLNode> rotateRight(std::shared_ptr<AVLNode> node);
	std::shared_ptr<AVLNode> rotateLeft(std::shared_ptr<AVLNode> node);

 private:
	void DeleteLeaf(std::shared_ptr<AVLNode> currentNode);
	int DeleteMin(std::shared_ptr<AVLNode> currentNode);

 	std::shared_ptr<AVLNode> root_;
 	size_t size_;
}; // class AVL