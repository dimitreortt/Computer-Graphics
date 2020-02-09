#include "ComponentCollection.h"
#include <iostream>

void ComponentCollection::add(Component* newComponent)
{
	Nodec* newNode = new Nodec(newComponent);
	if (this->_tail == nullptr) {
		this->_head = this->_tail = newNode;
		newNode->setNext(nullptr);
		newNode->setPrev(nullptr);
		add(new LastComponent());
	}
	else {
		if (_tail->_prev == nullptr)
		{ //add helpful LastComponent component
			_tail = newNode;
			_tail->_prev = _head;
			_tail->_next = nullptr;
		}
		else {
			newNode->_prev = _tail->_prev;
			newNode->setNext(_tail);
			_tail->_prev->_next = newNode;
			_tail->_prev = newNode;
		}
	}
}

ComponentCollection::iterator& ComponentCollection::remove(ComponentCollection::iterator& it)
{
	if (it == this->begin())
	{
		std::cout << "Impossible to remove first element, it is the Transform";
		exit(1);
	}
	else if (it == this->end())
	{
		std::cout << "Impossible to remove element after the last, it is null";
		exit(1);
	}
	else
	{
		auto node = it.itNode();
		if (node->_prev != nullptr)
			node->_prev->setNext(node->_next);
		if (node->_next != nullptr)
			node->_next->setPrev(node->_prev);
		else {
			_tail = node->_prev;
			_tail->setNext(nullptr);
			it = iterator(_tail);
			return it;
		}
		it = iterator(node->_prev);
		return it;
	}
}

bool ComponentCollection::has(const char* const cName)
{
	for (auto c : *this)
	{
		if (!strcmp(cName, c->typeName()))
			return true;
	}
	return false;
}