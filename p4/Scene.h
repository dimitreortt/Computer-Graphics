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
// OVERVIEW: Scene.h
// ========
// Class definition for scene.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 21/09/2019

#ifndef __Scene_h
#define __Scene_h

#include "SceneObject.h"
#include "graphics/Color.h"
#include "graphics/GLImage.h"
#include "Camera.h"
#include "Light.h"
#include <list>

namespace cg
{ // begin namespace cg

	class SceneEditor;
	class GLRenderer;
	class RayTracer;
	//class GLImage;

	enum ViewMode
	{
		Editor = 0,
		Renderer = 1
	};
/////////////////////////////////////////////////////////////////////
//
// Scene: scene class
// =====
class Scene: public SceneNode
{
public:
	Color backgroundColor{ Color::gray };
	Color ambientLight{ 213, 161, 216 };

  /// Constructs an empty scene.
  Scene(const char* name):
    SceneNode{name},
    _root{"Root", *this}
  {
    SceneObject::makeUse(&_root);
  }

  /// Returns the root scene object of this scene.
  auto root() const
  {
    return &_root;
  }

  auto root()
  {
    return &_root;
  }

  std::list<Reference<Light>> lights()
  {
	  return _lights;
  }

  void addLight(Light& l)
  {
	  _lights.push_back(Scene::makeUse(&l));
	  for (auto li : _lights)
	  {
		  std::cout << "added light from: " << li->sceneObject()->name() << "\n";
	  }
  }

  void removeLight(Light* l)
  {
	  _lights.remove(l);
	  Scene::release(l);
  }

  void removeLight(SceneObject* obj)
  {
	  for (auto c : obj->components())
	  {
		  if (auto l = dynamic_cast<Light*>(c))
		  {
			  _lights.remove(l);
			  Scene::release(l);
		  }
	  }
  }

  Camera*
  currentCamera()
  {
	  return _currentCamera;
  }

  void
  setCurrentCamera(Camera* camera)
  {
	  if (camera != _currentCamera)
	  {
		  if (camera != nullptr && camera->sceneObject() == nullptr)
			  return; // TODO: throw an exception
		  _currentCamera = camera;
	  }	  
  }

  //static Scene* current()
  //{
	 // return _current;
  //}
  
  //static void
  //setCurrent(Scene* scene)
  //{
	 // if (scene != _current)
	 // {
		//  if (scene != nullptr /*&& scene->sceneObject() == nullptr*/)
		//	  return; // TODO: throw an exception
		//  _current = scene;
	 // }
  //}

  ViewMode _viewMode{ ViewMode::Editor };

  bool _showAssets{ true };
  bool _showEditorView{ true };
  SceneEditor* _editor;
  GLRenderer* _renderer;
  RayTracer* _rayTracer;
  GLImage* _image;

private:	

  SceneObject _root;
  std::list<Reference<Light>> _lights;
  Camera* _currentCamera;
  //static Scene* _current;

}; // Scene

} // end namespace cg

#endif // __Scene_h
