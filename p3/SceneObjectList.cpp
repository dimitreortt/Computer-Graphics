#include "SceneObjectList.h"
void SceneObjectList::add(SceneObject* newSceneObject) {
	//Reference<SceneObject> soRef{ newSceneObject };
	Node* newNode = new Node(newSceneObject);
	
	if (this->_tail == nullptr) {
		this->_head = this->_tail = newNode;
		newNode->setNext(nullptr);
		newNode->setPrev(nullptr);
	}
	else {
		newNode->_prev = _tail;
		_tail->_next = newNode;
		_tail = newNode;
	}
}

void SceneObjectList::remove(SceneObject* trashNode)
{	
	Node* ptr;
	for (ptr = _head; ptr != _tail->_next; ptr = ptr->_next) {
		std::cout << "d,";
		SceneObject* c = ptr->_content;
		if (c == trashNode)//found node to be removed
			break;			
	}
	if (ptr == nullptr)
		std::cout << "No node found to be removed";
	else 
	{
		//
		if (ptr->_prev != nullptr) {
			ptr->_prev->_next = ptr->_next;

			if (ptr->_next != nullptr) {
				ptr->_next->_prev = ptr->_prev;
			}
			else {
				_tail = ptr->_prev;
				//_tail->_prev = ptr->_prev->_prev;
				_tail->_next = nullptr;
			}
		}
		else { //_prev is nullptr
			if (ptr->_next != nullptr) {
				_head = ptr->_next;
				ptr->_next->_prev = nullptr;
			}
			else //ptr is the only element
				_head = _tail = nullptr;
		}
		delete(ptr);
		return;
	}
}
