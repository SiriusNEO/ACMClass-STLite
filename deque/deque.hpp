#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include <iostream>
#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {
template<class T>
class deque;

template<class T>
class SquareLinks {
private:
    static const int BLOCK_SIZE = 350;
    struct Node {
        T data;
        int size;
        Node *pre, *nxt, *preHeader, *nxtHeader;
        explicit Node(T _data):data(_data),size(1),pre(nullptr),nxt(nullptr),preHeader(nullptr),nxtHeader(nullptr){}
    };
    typedef Node* NodeCur;
    NodeCur firstHeader, lastHeader;
    size_t size;
    friend deque<T>;

public:
    SquareLinks():firstHeader(nullptr), lastHeader(nullptr), size(0){}
    SquareLinks(const SquareLinks& obj):firstHeader(nullptr), lastHeader(nullptr), size(0) {
        NodeCur nowHeader = obj.firstHeader;
        while (nowHeader != nullptr) {
            NodeCur ptr = nowHeader;
            push_back(ptr->data), ptr = ptr->nxt;
            while (ptr != nowHeader) push_back(ptr->data), ptr = ptr->nxt;
            nowHeader = nowHeader->nxtHeader;
        }
    }
    SquareLinks& operator = (const SquareLinks& obj) {
        if (this == &obj) return *this;
        while (size) pop_back();
        NodeCur nowHeader = obj.firstHeader;
        while (nowHeader != nullptr) {
            NodeCur ptr = nowHeader;
            push_back(ptr->data), ptr = ptr->nxt;
            while (ptr != nowHeader) push_back(ptr->data), ptr = ptr->nxt;
            nowHeader = nowHeader->nxtHeader;
        }
        return *this;
    }
    ~SquareLinks() {
        NodeCur nowHeader = firstHeader;
        while (nowHeader != nullptr) {
            NodeCur ptr = nowHeader->pre;
            while (ptr != nowHeader) {
                NodeCur tmpPtr = ptr;
                ptr = ptr->pre;
                delete tmpPtr;
            }
            NodeCur tmpPtr = nowHeader;
            nowHeader = nowHeader->nxtHeader;
            delete tmpPtr;
        }
    }
    void split(NodeCur nowHeader) {
        NodeCur ptr = nowHeader;
        int nowPos = 0;
        while (nowPos < BLOCK_SIZE/2) nowPos++, ptr = ptr->nxt; //locate the split point
        //turn the split point to the last point
        ptr->pre->nxt = nowHeader;
        NodeCur lastPtr = nowHeader->pre;
        nowHeader->pre = ptr->pre;
        //insert the new header
        ptr->nxtHeader = nowHeader->nxtHeader;
        if (nowHeader->nxtHeader) nowHeader->nxtHeader->preHeader = ptr;
        nowHeader->nxtHeader = lastPtr->nxt = ptr;
        ptr->preHeader = nowHeader;
        //new header last node
        ptr->pre = lastPtr;
        //size
        ptr->size = nowHeader->size - BLOCK_SIZE/2; //half of the size
        nowHeader->size = BLOCK_SIZE/2;
        //update lastHeader
        if (ptr->nxtHeader == nullptr) lastHeader = ptr;
    }
    NodeCur insert(size_t pos, const T& val) { //insert behind pos
        if (!pos) return push_front(val);
        if (pos == size) return push_back(val);
        size++;
        if (firstHeader == nullptr) {
            lastHeader = firstHeader = new Node(val); //the data structure is empty, create a new node
            firstHeader->pre = firstHeader->nxt = firstHeader;
            return firstHeader;
        }
        NodeCur ptr = firstHeader, nowHeader;
        int nowPos = 0;
        while (ptr != nullptr && nowPos + ptr->size < pos) {
            nowPos += ptr->size;
            ptr = ptr->nxtHeader;
        }
        nowHeader = ptr; //locate the row
        nowHeader->size++;
        while (nowPos+1 < pos) nowPos++, ptr = ptr->nxt; //locate the column
        NodeCur newNode = new Node(val);
        newNode->nxt = ptr->nxt; //insert
        ptr->nxt->pre = newNode;
        ptr->nxt = newNode;
        newNode->pre = ptr;
        if (nowHeader->size >= BLOCK_SIZE) split(nowHeader);
        return newNode;
    }
    NodeCur del(size_t pos) {
        size--;
        NodeCur ptr = firstHeader, nowHeader;
        int nowPos = 0;
        while (ptr != nullptr && nowPos + ptr->size < pos) {
            nowPos += ptr->size;
            ptr = ptr->nxtHeader;
        }
        nowHeader = ptr; //locate the row
        while (nowPos+1 < pos) nowPos++, ptr = ptr->nxt;
        if (ptr == nowHeader) { //delete header
            if (ptr->size == 1) {
                if (nowHeader->preHeader) nowHeader->preHeader->nxtHeader = nowHeader->nxtHeader;
                if (nowHeader->nxtHeader) nowHeader->nxtHeader->preHeader = nowHeader->preHeader;
                if (nowHeader == firstHeader) {
                    if (nowHeader == lastHeader) firstHeader = lastHeader = nullptr;
                    else firstHeader = firstHeader->nxtHeader;
                } else if (nowHeader == lastHeader) lastHeader = lastHeader->preHeader;
                NodeCur ret = nullptr;
                if (nowHeader->nxtHeader) ret = nowHeader->nxtHeader;
                else if (nowHeader->preHeader) ret = nowHeader->preHeader->pre;
                delete nowHeader;
                return ret;
            }
            else {
                ptr->size--;
                ptr->pre->nxt = ptr->nxt;
                ptr->nxt->size = ptr->size;
                ptr->nxt->pre = ptr->pre;
                if (ptr->nxtHeader) ptr->nxtHeader->preHeader = ptr->nxt;
                if (ptr->preHeader) ptr->preHeader->nxtHeader = ptr->nxt;
                ptr->nxt->nxtHeader = ptr->nxtHeader;
                ptr->nxt->preHeader = ptr->preHeader;
                if (firstHeader == ptr) firstHeader = ptr->nxt;
                if (lastHeader == ptr) lastHeader = ptr->nxt;
                NodeCur ret = ptr->nxt;
                delete ptr;
                return ret;
            }
        }
        ptr->pre->nxt = ptr->nxt;
        ptr->nxt->pre = ptr->pre;
        nowHeader->size--;
        NodeCur ret = ptr->nxt;
        if (ret == nowHeader) {
            if (nowHeader->nxtHeader) ret = nowHeader->nxtHeader;
            else ret = nowHeader->pre;
        }
        delete ptr;
        return ret;
    }
    NodeCur push_front(const T& val) {
        size++;
        if (firstHeader == nullptr) {
            lastHeader = firstHeader = new Node(val);
            firstHeader->pre = firstHeader->nxt = firstHeader;
            return firstHeader;
        }
        NodeCur newNode = new Node(val);
        firstHeader->size++;
        //insert newNode
        newNode->nxt = firstHeader;
        newNode->pre = firstHeader->pre;
        newNode->pre->nxt = newNode;
        firstHeader->pre = newNode;
        newNode->size = firstHeader->size, firstHeader->size = 1;
        //insert header
        newNode->nxtHeader = firstHeader->nxtHeader, firstHeader->nxtHeader = nullptr;
        if (newNode->nxtHeader) newNode->nxtHeader->preHeader = newNode;
        if (firstHeader == lastHeader) lastHeader = newNode;
        firstHeader = newNode;
        if (firstHeader->size >= BLOCK_SIZE) split(firstHeader);
        return newNode;
    }
    void pop_front() {
        size--;
        if (firstHeader->size == 1) { //delete the first row
            NodeCur tmpPtr = firstHeader;
            firstHeader = firstHeader->nxtHeader;
            if (firstHeader == nullptr) lastHeader = nullptr;
            delete tmpPtr;
            return ;
        }
        firstHeader->size--;
        firstHeader->pre->nxt = firstHeader->nxt;
        firstHeader->nxt->size = firstHeader->size;
        firstHeader->nxt->pre = firstHeader->pre;
        if (firstHeader->nxtHeader) firstHeader->nxtHeader->preHeader = firstHeader->nxt;
        firstHeader->nxt->nxtHeader = firstHeader->nxtHeader;
        NodeCur tmpPtr = firstHeader;
        if (firstHeader == lastHeader) lastHeader = firstHeader->nxt;
        firstHeader = firstHeader->nxt;
        delete tmpPtr;
    }
    NodeCur push_back(const T& val) {
        size++;
        if (firstHeader == nullptr) {
            lastHeader = firstHeader = new Node(val);
            firstHeader->pre = firstHeader->nxt = firstHeader;
            return firstHeader;
        }
        NodeCur newNode = new Node(val);
        lastHeader->size++;
        lastHeader->pre->nxt = newNode;
        newNode->pre = lastHeader->pre;
        newNode->nxt = lastHeader;
        lastHeader->pre = newNode;
        if (lastHeader->size >= BLOCK_SIZE) split(lastHeader);
        return newNode;
    }
    void pop_back() {
        size--;
        if (lastHeader->size == 1) {
            NodeCur tmpPtr = lastHeader;
            if (firstHeader == lastHeader) firstHeader = nullptr;
            lastHeader = lastHeader->preHeader;
            if (lastHeader) lastHeader->nxtHeader = nullptr;
            delete tmpPtr;
            return ;
        }
        lastHeader->size--;
        lastHeader->pre->pre->nxt = lastHeader;
        NodeCur tmpPtr = lastHeader->pre;
        lastHeader->pre = lastHeader->pre->pre;
        delete tmpPtr;
    }
    NodeCur find(size_t pos) const {
        NodeCur ptr = firstHeader;
        int nowPos = 0;
        while (ptr != nullptr && nowPos + ptr->size < pos) nowPos += ptr->size, ptr = ptr->nxtHeader;
        while (nowPos+1 < pos) nowPos++, ptr = ptr->nxt;
        return ptr;
    }
};

template<class T>
class deque {
private:
    SquareLinks<T> squareLinks;
public:
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
        size_t pos;
        const SquareLinks<T>* dataStructureCur;
        typename SquareLinks<T>::NodeCur elementCur;
        friend deque<T>;
	public:
	    iterator():pos(0), dataStructureCur(nullptr), elementCur(nullptr){}
	    iterator(size_t _pos, const SquareLinks<T>* _dataStructureCur, typename SquareLinks<T>::NodeCur _elementCur):
        pos(_pos), dataStructureCur(_dataStructureCur), elementCur(_elementCur){}
		~iterator(){}
		/**
		 * return a new iterator which pointer n-next elements
		 *   if there are not enough elements, iterator becomes invalid
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			iterator ret = *this;
			ret += n;
			return ret;
		}
		iterator operator-(const int &n) const {
			iterator ret = *this;
            ret -= n;
			return ret;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
            if (dataStructureCur != rhs.dataStructureCur) throw invalid_iterator();
		    return pos-rhs.pos;
		}
		iterator& operator+=(const int &n) {
            pos += n;
            if (pos < 1) elementCur = dataStructureCur->firstHeader;
            else if (pos > dataStructureCur->size && dataStructureCur->lastHeader) elementCur = dataStructureCur->lastHeader->pre;
            else elementCur = dataStructureCur->find(pos);
		    return *this;
		}
		iterator& operator-=(const int &n) {
            return (*this)+=(-n);
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
		    iterator ret = *this;
            ++(*this);
		    return ret;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
		    pos++;
		    if (elementCur && pos <= dataStructureCur->size && pos > 1) {
                elementCur = elementCur->nxt;
                if (elementCur->size > 1 || elementCur->nxt == elementCur) elementCur = elementCur->nxtHeader;
            }
		    return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
		    iterator ret = *this;
            --(*this);
		    return ret;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
		    pos--;
            if (elementCur && pos < dataStructureCur->size && pos > 0) {
                if (elementCur->size > 1 || elementCur->nxt == elementCur) {
                    elementCur = elementCur->preHeader;
                    if (elementCur) elementCur = elementCur->pre;
                }
                else elementCur = elementCur->pre;
            }
            return *this;
		}
		/**
		 * TODO *it
		 * 		throw if iterator is invalid
		 */
		T& operator*() const {
            if (invalid() || elementCur == nullptr || pos == dataStructureCur->size + 1) throw invalid_iterator();
		    return elementCur->data;
		}
		/**
		 * TODO it->field
		 * 		throw if iterator is invalid
		 */
		T* operator->() const noexcept {
            if (invalid() || elementCur == nullptr || pos == dataStructureCur->size + 1) throw invalid_iterator();
		    return &elementCur->data;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {return pos == rhs.pos && elementCur == rhs.elementCur;}
		bool operator==(const const_iterator &rhs) const {return pos == rhs.pos && elementCur == rhs.elementCur;}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {return pos != rhs.pos || elementCur != rhs.elementCur;}
		bool operator!=(const const_iterator &rhs) const {return pos != rhs.pos || elementCur != rhs.elementCur;}
        bool invalid() const {return pos <= 0 || pos > dataStructureCur->size+1;}
	};
	class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
    private:
        // data members.
        size_t pos;
        const SquareLinks<T> *dataStructureCur;
        typename SquareLinks<T>::NodeCur elementCur;
        friend deque<T>;
    public:
        const_iterator():pos(0), dataStructureCur(nullptr), elementCur(nullptr) {}
        const_iterator(size_t _pos, const SquareLinks<T>* _dataStructureCur, typename SquareLinks<T>::NodeCur _elementCur):
        pos(_pos), dataStructureCur(_dataStructureCur), elementCur(_elementCur){}
        const_iterator(const const_iterator &other):pos(other.pos), dataStructureCur(other.dataStructureCur), elementCur(other.elementCur) {}
        explicit const_iterator(const iterator &other):pos(other.pos), dataStructureCur(other.dataStructureCur), elementCur(other.elementCur) {}
        ~const_iterator(){}
        const_iterator operator+(const int &n) const {
            const_iterator ret = *this;
            ret += n;
            return ret;
        }
        const_iterator operator-(const int &n) const {
            const_iterator ret = *this;
            ret -= n;
            return ret;
        }
        // return th distance between two iterator,
        // if these two iterators points to different vectors, throw invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            if (dataStructureCur != rhs.dataStructureCur) throw invalid_iterator();
            return pos-rhs.pos;
        }
        const_iterator& operator+=(const int &n) {
            pos += n;
            if (pos < 1) elementCur = dataStructureCur->firstHeader;
            else if (pos > dataStructureCur->size && dataStructureCur->lastHeader) elementCur = dataStructureCur->lastHeader->pre;
            else elementCur = dataStructureCur->find(pos);
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            return (*this)+=(-n);
        }
        const_iterator operator++(int) {
            const_iterator ret = *this;
            ++(*this);
            return ret;
        }
        const_iterator& operator++() {
            pos++;
            if (elementCur && pos <= dataStructureCur->size && pos > 1) {
                elementCur = elementCur->nxt;
                if (elementCur->size > 1 || elementCur->nxt == elementCur) elementCur = elementCur->nxtHeader;
            }
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator ret = *this;
            --(*this);
            return ret;
        }
        const_iterator& operator--() {
            pos--;
            if (elementCur && pos < dataStructureCur->size && pos > 0) {
                if (elementCur->size > 1 || elementCur->nxt == elementCur) {
                    elementCur = elementCur->preHeader;
                    if (elementCur) elementCur = elementCur->pre;
                }
                else elementCur = elementCur->pre;
            }
            return *this;
        }
        const T& operator*() const {
            if (invalid() || elementCur == nullptr || pos == dataStructureCur->size + 1) throw invalid_iterator();
            return elementCur->data;
        }
        const T* operator->() const noexcept {
            if (invalid() || elementCur == nullptr || pos == dataStructureCur->size + 1) throw invalid_iterator();
            return &elementCur->data;
        }
        bool operator==(const iterator &rhs) const {return pos == rhs.pos && elementCur == rhs.elementCur;}
        bool operator==(const const_iterator &rhs) const {return pos == rhs.pos && elementCur == rhs.elementCur;}
        bool operator!=(const iterator &rhs) const {return pos != rhs.pos || elementCur != rhs.elementCur;}
        bool operator!=(const const_iterator &rhs) const {return pos != rhs.pos || elementCur != rhs.elementCur;}
        bool invalid() const {return pos < 0 || pos > dataStructureCur->size+1;}
	};
	/**
	 * TODO Constructors
	 */
	deque():squareLinks() {}
	deque(const deque &other):squareLinks(other.squareLinks) {}
	/**
	 * TODO Deconstructor
	 */
	~deque() {}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {
	    if (this == &other) return *this;
	    squareLinks = other.squareLinks;
	    return *this;
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
    T & at(const size_t &pos) {
        if (pos < 0 || pos >= squareLinks.size) throw index_out_of_bound();
        return squareLinks.find(pos+1)->data;
    }
	const T & at(const size_t &pos) const {
        if (pos < 0 || pos >= squareLinks.size) throw index_out_of_bound();
        return squareLinks.find(pos+1)->data;
    }
	T & operator[](const size_t &pos) {
        if (pos < 0 || pos >= squareLinks.size) throw index_out_of_bound();
        return squareLinks.find(pos+1)->data;
    }
	const T & operator[](const size_t &pos) const {
        if (pos < 0 || pos >= squareLinks.size) throw index_out_of_bound();
        return squareLinks.find(pos+1)->data;
    }
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
        if (empty()) throw container_is_empty();
        return squareLinks.firstHeader->data;
    }
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
        if (empty()) throw container_is_empty();
        return squareLinks.lastHeader->pre->data;
    }
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {return iterator(1, &squareLinks, squareLinks.firstHeader);}
	const_iterator cbegin() const {return const_iterator(1, &squareLinks, squareLinks.firstHeader);}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
	    if (empty()) return iterator(squareLinks.size+1, &squareLinks, nullptr);
        return iterator(squareLinks.size+1, &squareLinks, squareLinks.lastHeader->pre);
	}
	const_iterator cend() const {
        if (empty()) return const_iterator(iterator(squareLinks.size+1, &squareLinks, nullptr));
	    return const_iterator(squareLinks.size+1, &squareLinks, squareLinks.lastHeader->pre);
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
	    return squareLinks.size <= 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
	    return squareLinks.size;
	}
	/**
	 * clears the contents
	 */
	void clear() {
	    while (!empty()) squareLinks.pop_back();
	}
	/**
	 * inserts elements at the specified locate on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
        if (pos.invalid() || pos.dataStructureCur != &squareLinks) throw invalid_iterator();
	    return iterator(pos.pos, &squareLinks, squareLinks.insert(pos.pos-1, value));
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
        if (empty()) throw container_is_empty();
        if (pos.invalid() || pos.dataStructureCur != &squareLinks) throw invalid_iterator();
        return iterator(pos.pos, &squareLinks, squareLinks.del(pos.pos));
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {squareLinks.push_back(value);}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
        if (empty()) throw container_is_empty();
	    squareLinks.pop_back();
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {squareLinks.push_front(value);}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
        if (empty()) throw container_is_empty();
	    squareLinks.pop_front();
	}

	void print() {squareLinks.print();}
};

}

#endif
