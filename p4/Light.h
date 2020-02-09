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
// OVERVIEW: Light.h
// ========
// Class definition for light.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 14/10/2019

#ifndef __Light_h
#define __Light_h

//#include "Component.h"
//#include "Scene.h"
#include "graphics/Color.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Light: light class
// =====
class Light: public Component
{
public:
  enum Type
  {
    Directional,
    Point,
    Spot
  };

  Color color{Color::white};
  bool on{ true }; // luz esta ligada ou nao

  Light():
    Component{"Light"},
    _type{Directional},
	  _falloff{ 1 },
	  _fallExponent{ 0.2f },
	  _ghama{ 30 }
  {
    // do nothing
  }

  auto type() const
  {
    return _type;
  }

  void setType(Type type)
  {
    _type = type;
  }

  int falloff()
  {
	  return _falloff;
  }

  void setFalloff(int f)
  {
	  _falloff = f;
  }

  float fallExponent()
  {
	  return _fallExponent;
  }

  void setFallExponent(float fe) 
  {
	  _fallExponent = fe;
  }

  vec3f position()
  {
	  //return _position;
	  return this->sceneObject()->transform()->localPosition();
  }

  vec3f direction()
  {
	  //return _direction;
	  return this->sceneObject()->transform()->localRotation() * vec3f { 0, 0, -1 };
  }

  float ghama()
  {
	  return _ghama;
  }

  void setGhama(float g)
  {
	  _ghama = g;
  }

private:
  Type _type;
  int _falloff;
  vec4f _position;
  vec3f _direction; //passivel de mudança (_position pode ser interpretado como _direction)
  float _ghama; // notacao do capitulo 4 para luz spot, angulo de abertura
  float _fallExponent; // expoente de decaimento
}; // Light

} // end namespace cg

#endif // __Light_h
