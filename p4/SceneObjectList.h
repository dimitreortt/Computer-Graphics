#ifndef __OBJECTLIST_h
#define __OBJECTLIST_h

#include "Primitive.h"
#include <iostream>

using namespace cg;

//forward definition
class cg::SceneObject;

class Node {
public:
	//Reference<SceneObject> _content; //conteudo
	SceneObject* _content;
	Node* _next; //next
	Node* _prev; //previous

	//Node(Reference<SceneObject> content) : _content(content){ _next = _prev = nullptr;}
	Node(SceneObject* content) : _content(content) { _next = _prev = nullptr; }
	void setNext(Node* next) { _next = next; }
	void setPrev(Node* prev) { _prev = prev; }
};

class SceneObjectList : public SharedObject
{
public:
	SceneObjectList() {
		_head = _tail = nullptr;
	};

	class iterator
	{
	public:
		typedef iterator self_type;
		typedef Node& reference;
		typedef Node* pointer;
		typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;
	
		iterator() { _ptr = nullptr; }
		iterator(pointer ptr):_ptr(ptr) {}

		self_type operator++(void){
			self_type i = _ptr->_next; 
			_ptr = _ptr->_next; 
			return i; 
		}
		self_type operator++(int off) { _ptr = _ptr->_next; return *this; }

		SceneObject* operator*() { return _ptr->_content;}
		SceneObject* operator->(){ return (_ptr->_content);}

		bool operator==(const self_type& rhs) { return _ptr == rhs._ptr; }
		bool operator!=(const self_type& rhs) { return _ptr != rhs._ptr; }
	private:
		pointer _ptr;
	};

	void add(SceneObject* newSceneObject); //add um SceneObject
	void remove(SceneObject* trashSceneObject); //SceneObject descartado
	iterator begin() { return iterator(_head); }
	iterator end() { return _tail != nullptr ? iterator(_tail->_next) : _tail;}
	bool isEmpty() { return _head == nullptr; }
private:
	Node* _head;
	Node* _tail;

}; // SceneObjectList end

#endif // __OBJECTLIST_h