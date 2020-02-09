//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: SceneObject.h
// ========
// Class definition for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 23/09/2019

#ifndef __SceneObject_h
#define __SceneObject_h

#include "SceneNode.h"
#include "Transform.h"
#include "SceneObjectList.h"
#include "ComponentCollection.h"

namespace cg
{ // begin namespace cg

// Forward definition
class Scene;

/////////////////////////////////////////////////////////////////////
//
// SceneObject: scene object class
// ===========
class SceneObject: public SceneNode
{
public:
  enum Type
  {
	ThreeDObject,
	Light,
	Camera,
	EmptyObject
  };

  bool visible{true};

  /// Constructs an empty scene object.
  SceneObject(const char* name, Scene& scene):
    SceneNode{name},
    _scene{&scene},
    _parent{}
  {
    addComponent(makeUse(&_transform));    
  }

  /// Returns the scene which this scene object belong to.
  auto scene() const
  {
    return _scene;
  }

  /// Returns the parent of this scene object.
  auto parent() const
  {
    return _parent;
  }

  /// Sets the parent of this scene object.
  void setParent(SceneObject* parent);

  /// Returns the transform of this scene object.
  auto transform() const
  {
    return &_transform;
  }

  auto transform()
  {
    return &_transform;
  }

  void addComponent(Component* component)
  {
    component->_sceneObject = this;
	_components.add(Component::makeUse(component));
  }

  void deleteComponent(Component* c)
  {
	  _components.remove(c);
  }

  void deleteComponent(const char* const cName)
  {
	  _components.remove(cName);
  }

  void addChild(SceneObject* child)
  {
	  // child->setParent(SceneObject::makeUse(this));
	  _children.add(SceneObject::makeUse(child));
  }

  // delete all children and all trees from them
  void deleteChildren()
  {
	  for(auto child : _children)
	  {	
		  child->deleteChildren();
		  _children.remove(child);
	  }	  
  }

  // delete one child and entire tree from it
  void deleteChild(SceneObject* child)
  {
	  for (auto o : child->children())
	  {
		  o->deleteChildren();
	  }
	  _children.remove(child);
  }

  // removes one child from children but keeps its children 
  // (used in change of paternity)
  void removeChild(SceneObject* child)
  {
	  _children.remove(child);
  }

  SceneObjectList children()
  {
	  return _children;
  }

  //prints kids's names
  bool hasKids()
  {	
	  return !_children.isEmpty();
  }

  ComponentCollection components()
  {
	  return _components;
  }

  Type type()
  {
	  return _type;
  }

  void setType(Type type)
  {
	  _type = type;
  }

private:
  Type _type;
  Scene* _scene;
  SceneObject* _parent;
  Transform _transform;
  SceneObjectList _children;
  ComponentCollection _components;

  friend class Scene;

}; // SceneObject

/// Returns the transform of a component.
inline Transform*
Component::transform() // declared in Component.h
{
  return sceneObject()->transform();
}

/// Returns the parent of a transform.
inline Transform*
Transform::parent() const // declared in Transform.h
{
  if (auto p = sceneObject()->parent())
    return p->transform();
   return nullptr;
}

} // end namespace cg

#endif // __SceneObject_h
