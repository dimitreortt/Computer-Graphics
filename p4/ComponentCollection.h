#ifndef __Component_Collection_h
#define __Component_Collection_h

#include "graphics/Application.h"
#include "Component.h"
#include <iostream>


using namespace cg;
class cg::SceneObject;

class Nodec {
public:
	//Reference<Component> _content; //conteudo
	Component* _content;
	Nodec* _next; //next
	Nodec* _prev; //previous

	Nodec(Component* content) : _content(content) { _next = _prev = nullptr; }
	void setNext(Nodec* next) { _next = next; }
	void setPrev(Nodec* prev) { _prev = prev; }
};

class ComponentCollection : public SharedObject
{
public:
	ComponentCollection(/*const char* sceneName*/) {
		/*_sceneName = sceneName;*/
		_head = _tail = nullptr;
	};
	
	class iterator
	{
	public:
		typedef iterator self_type;
		typedef Nodec& reference;
		typedef Nodec* pointer;
		typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;

		iterator() { _ptr = nullptr; }
		iterator(pointer ptr) :_ptr(ptr) {}

		Nodec* itNode() { return _ptr; }
		self_type operator++()
		{
			self_type i = _ptr->_next;
			_ptr = _ptr->_next;
			return i;
		}
		self_type operator--()
		{
			if (_ptr->_prev == nullptr) { std::cout << "prev is nullptr! Exiting cc.h"; exit(1); }
			self_type i = _ptr->_prev;
			_ptr = _ptr->_prev;
			return i;
		}
		self_type operator++(int off) { _ptr = _ptr->_next; return *this; }
		self_type operator--(int off) { _ptr = _ptr->_prev; return *this; }
		Component* operator*() 
		{ 
			return _ptr->_content; }
		Component* operator->() { return _ptr->_content; }
		bool operator==(const self_type& rhs) { return _ptr == rhs._ptr; }
		bool operator!=(const self_type& rhs) { 
			/*if (_ptr == nullptr)
				std::cout << " _ptr is null,";
			if (rhs._ptr == nullptr)
				std::cout << "rhs._ptr is null,";*/
			return _ptr != rhs._ptr; 
		}				
	private:
		pointer _ptr;
	};

	iterator begin() { return iterator(_head); }
	iterator end() {
		return iterator(_tail->_next);
		//if (_tail != nullptr)
		//{
		//	//std::cout << "\n(_tail != null, return it(_tail-next))";
		//	//if (_tail->_next == nullptr)
		//	//	std::cout << "\n(_tail->next is nullptr)";
		//	//else
		//	//	_tail->_next = _tail->_next;
		//	//	//_tail->_next = nullptr;

		//	return iterator(_tail->_next);
		//}
		//else
		//{
		//	//td::cout << "\n(_tail == null, return it(_tail))";
		//	return iterator(_tail);
		//}
		////return _tail != nullptr ? iterator(_tail->_next) : iterator(_tail);
	}
	void add(Component* newComponent); //add um Component que pode ser um 'SceneObject' ou 'Scene'
	void remove(Component* trashComponent); //Component descartavel
	void remove(const char* const trashNodeTypeName);
	ComponentCollection::iterator& remove(ComponentCollection::iterator& it);
	bool isTailNextNull();
	Nodec* head() { return _head; }
	Nodec* tail() { return _tail; }
	bool has(const char* const);

private:
	/*const char* _sceneName;*/
	Nodec* _head;
	Nodec* _tail;

}; // Component Collection end

#endif // __Component_Collection_h