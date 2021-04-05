#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"
#include <iostream>

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct Node {
        T data;
        int dis;
        Node *leftSon, *rightSon;
        explicit Node(const T& _data) : data(_data), dis(0), leftSon(nullptr), rightSon(nullptr){}
        ~Node() {
            if (leftSon) delete leftSon, leftSon = nullptr;
            if (rightSon) delete rightSon, rightSon = nullptr;
        }
    };
    Node *root;
    size_t treeSize;
    static void printNode(Node* now) {
        std::cout << "(" << now->data << "," << now->dis << ")" << ' ';
        if (now->leftSon) std::cout << "leftson:" << now->leftSon->data << "," << now->leftSon->dis << ' ';
        if (now->rightSon) std::cout << "rightson:" << now->rightSon->data << "," << now->rightSon->dis << ' ';
        std::cout << '\n';
    }
    static void copyBuilder(Node*& now, Node* obj) {
        now->data = obj->data, now->dis = obj->dis;
        if (obj->leftSon) {
            now->leftSon = new Node (obj->leftSon->data);
            copyBuilder(now->leftSon, obj->leftSon);
        }
        if (obj->rightSon) {
            now->rightSon = new Node (obj->rightSon->data);
            copyBuilder(now->rightSon, obj->rightSon);
        }
    }
    static void dfs(Node* now, void (*f) (Node*)) {
        f(now);
        if (now->leftSon) dfs(now->leftSon, f);
        if (now->rightSon) dfs(now->rightSon, f);
    }
    static void leftistMerge(Node*& A, Node*& B) {
        if (Compare()(A->data, B->data)) std::swap(A, B);
        if (A->rightSon == nullptr) A->rightSon = B;
        else leftistMerge(A->rightSon, B);
        A->dis = A->rightSon->dis + 1;
        if (A->leftSon) {
            if (A->rightSon->dis > A->leftSon->dis) std::swap(A->leftSon, A->rightSon);
        }
        else std::swap(A->leftSon, A->rightSon);
    }

public:
	/**
	 * TODO constructors
	 */
	priority_queue() {
	    treeSize = 0;
	    root = nullptr;
	}
    explicit priority_queue(const T& e) {
        treeSize = 1;
        root = new Node(e);
    }
	priority_queue(const priority_queue &other) {
        if (other.root) {
            root = new Node(other.root->data);
            root->dis = other.root->dis;
            copyBuilder(root, other.root);
        }
        treeSize = other.treeSize;
	}
	/**
	 * TODO deconstructor
	 */
	~priority_queue() {
        if (root) delete root, root = nullptr;
	}
	/**
	 * TODO Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
	    if (this == &other) return *this;
        if (root) delete root, root = nullptr;
        if (other.root) {
            root = new Node(other.root->data);
            root->dis = other.root->dis;
            copyBuilder(root, other.root);
        }
        treeSize = other.treeSize;
        return *this;
	}
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
        if (empty()) throw container_is_empty();
        return root->data;
	}
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
        treeSize++;
        if (empty()) root = new Node(e);
	    else {
	        Node* newNode = new Node(e);
	        leftistMerge(root, newNode);
	    }
	}
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
	    if (empty()) throw container_is_empty();
	    treeSize--;
        Node* tmp = root;
        if (root->leftSon == nullptr) root = root->rightSon;
        else if (root->rightSon == nullptr) root = root->leftSon;
        else {
            leftistMerge(root->leftSon, root->rightSon);
            root = (Compare()(tmp->leftSon->data, tmp->rightSon->data)) ? tmp->rightSon : tmp->leftSon;
        }
        if (tmp) {
            tmp->leftSon = tmp->rightSon = nullptr;
            delete tmp;
        }
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const {
        return treeSize;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
        return root == nullptr;
	}
	/**
	 * return a merged priority_queue with at least O(logn) complexity.
	 */
	void merge(priority_queue &other) {
	    treeSize += other.treeSize;
	    leftistMerge(root, other.root);
	    other.root = nullptr;
	}

	void printTree() const {
        if (empty()) throw container_is_empty();
        std::cout << "*-------------- Tree --------------*" << '\n';
	    dfs(root, printNode);
	}
};

}

#endif
