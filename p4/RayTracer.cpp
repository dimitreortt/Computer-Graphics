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
// OVERVIEW: RayTracer.cpp
// ========
// Source file for simple ray tracer.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 16/10/2019

#include "Camera.h"
#include "RayTracer.h"
#include <time.h>

using namespace std;

namespace cg
{ // begin namespace cg

void
printElapsedTime(const char* s, clock_t time)
{
  printf("%sElapsed time: %.4f s\n", s, (float)time / CLOCKS_PER_SEC);
}


/////////////////////////////////////////////////////////////////////
//
// RayTracer implementation
// =========
RayTracer::RayTracer(Scene& scene, Camera* camera):
  Renderer{scene, camera},
  _maxRecursionLevel{6},
  _minWeight{MIN_WEIGHT}
{
  // TODO: BVH
}

void
RayTracer::render()
{
  throw std::runtime_error("RayTracer::render() invoked");
}

inline float
windowHeight(Camera* c)
{
  if (c->projectionType() == Camera::Parallel)
    return c->height();
  return c->nearPlane() * tan(math::toRadians(c->viewAngle() * 0.5f)) * 2;

}
void
RayTracer::renderImage(Image& image)
{
  auto t = clock();
  const auto& m = _camera->cameraToWorldMatrix();

  // VRC axes
  _vrc.u = m[0];
  _vrc.v = m[1];
  _vrc.n = m[2];
  // init auxiliary mapping variables
  _W = image.width();
  _H = image.height();
  _Iw = math::inverse(float(_W));
  _Ih = math::inverse(float(_H));

  auto height = windowHeight(_camera);

  _W >= _H ? _Vw = (_Vh = height) * _W * _Ih : _Vh = (_Vw = height) * _H * _Iw;
  // init pixel ray
  _pixelRay.origin = _camera->transform()->position();
  _pixelRay.direction = -_vrc.n;
  _camera->clippingPlanes(_pixelRay.tMin, _pixelRay.tMax);
  _numberOfRays = _numberOfHits = 0;
  scan(image);
  printf("\nNumber of rays: %llu", _numberOfRays);
  printf("\nNumber of hits: %llu", _numberOfHits);
  printElapsedTime("\nDONE! ", clock() - t);
}

void
RayTracer::setPixelRay(float x, float y)
//[]---------------------------------------------------[]
//|  Set pixel ray                                      |
//|  @param x coordinate of the pixel                   |
//|  @param y cordinates of the pixel                   |
//[]---------------------------------------------------[]
{
  auto p = imageToWindow(x, y);

  switch (_camera->projectionType())
  {
    case Camera::Perspective:
      _pixelRay.direction = (p - _camera->nearPlane() * _vrc.n).versor();
      break;

    case Camera::Parallel:
      _pixelRay.origin = _camera->transform()->position() + p;
      break;
  }
}

void
RayTracer::scan(Image& image)
{
  ImageBuffer scanLine{_W, 1};

  for (int j = 0; j < _H; j++)
  {
    auto y = (float)j + 0.5f;

    printf("Scanning line %d of %d\r", j + 1, _H);
    for (int i = 0; i < _W; i++)
      scanLine[i] = shoot((float)i + 0.5f, y);
    image.setData(0, j, scanLine);
  }
}

Color
RayTracer::shoot(float x, float y)
//[]---------------------------------------------------[]
//|  Shoot a pixel ray                                  |
//|  @param x coordinate of the pixel                   |
//|  @param y cordinates of the pixel                   |
//|  @return RGB color of the pixel                     |
//[]---------------------------------------------------[]
{
  // set pixel ray
  setPixelRay(x, y);

  // trace pixel ray
  Color color = trace(_pixelRay, 0, 1.0f);
  
  // adjust RGB color
  if (color.r > 1.0f)
    color.r = 1.0f;
  if (color.g > 1.0f)
    color.g = 1.0f;
  if (color.b > 1.0f)
    color.b = 1.0f;
  
  return color;
}

Color
RayTracer::trace(const Ray& ray, uint32_t level, float weight)
//[]---------------------------------------------------[]
//|  Trace a ray                                        |
//|  @param the ray                                     |
//|  @param recursion level                             |
//|  @param ray weight                                  |
//|  @return color of the ray                           |
//[]---------------------------------------------------[]
{
  if (level > _maxRecursionLevel)
    return Color::black;  

  _numberOfRays++;

  Intersection hit;

  return intersect(ray, hit) ? shade(ray, hit, level, weight) : background();
}

inline constexpr auto
rt_eps()
{
  return 1e-4f;
}

bool
RayTracer::intersect(const Ray& ray, Intersection& hit)
//[]---------------------------------------------------[]
//|  Ray/object intersection                            |
//|  @param the ray (input)                             |
//|  @param information on intersection (output)        |
//|  @return true if the ray intersects an object       |
//[]---------------------------------------------------[]
{
  hit.object = nullptr;
  hit.distance = ray.tMax;
  
  // aplica primitive.intersect para cada objeto da hierarquia de objetos
  hierarchyIntersection(ray, hit, scene()->root());

  return hit.object != nullptr;
}

// define primitivo da hierarquia mais proximo a ser interceptado por ray (se houver)
void RayTracer::hierarchyIntersection(const Ray& ray, Intersection& hit, SceneObject* obj)
{
	for (auto child : obj->children())
	{
		// testa a intersecção para filhos de o
		for (auto c : child->components())
			if (auto p = dynamic_cast<Primitive*>(c))
				p->intersect(ray, hit);
		// testa para filhos
		hierarchyIntersection(ray, hit, child);
	}
}

Color
RayTracer::phong(const vec3f& rayOrigin, Light& l, vec3f& p, vec3f& pNormal, vec3f& lPos, vec3f& lDir, Material& m, Color& ambientLight)
{
	Color Il;
	vec3f Ll;

	if (l.type() == Light::Directional) // Directional light
	{
		Ll = lDir.versor();
		Il = l.color;
	}

	else if (l.type() == Light::Point)	// Point light
	{
		// PROBLEMA: p está em coordenadas globais, não acompanhou alteraçoes no transform da caixa
		auto temp = (p - lPos); // essa linha pode ter problemas
		float Dl = temp.length(); 
		Ll = temp * math::inverse(Dl);
		Il = l.color * math::inverse(pow(Dl, l.falloff()));
	}

	else if (l.type() == Light::Spot)	// Spot light
	{
		auto temp =  p - lPos;
		auto Dl = temp.length(); // essa linha pode ter problemas
		Ll = temp * math::inverse(Dl);

		// capitulo 4 sobre luz spot
		vec3f dir = lDir.versor();
		float fi = acos(dir.dot(Ll));

		if (fi < l.ghama())	// se o ponto esta no alcance
			Il = (l.color * math::inverse(pow(Dl, l.falloff())) * pow(cos(fi), l.fallExponent()));
		else
			Il = Color::black;
	}

	// ray.origin contains the position of the "observer"
	auto V = (p - rayOrigin).versor();

	auto Rl = Ll - 2 * pNormal.dot(Ll) * pNormal;

	// phong formula
	auto pColor = 
		m.ambient * ambientLight + 
		(m.diffuse * Il * std::max(-pNormal.dot(Ll), 0.0f)) + 
		(m.spot * Il * std::pow(std::max(-Rl.dot(V), 0.0f), m.shine));

	return pColor;
}

Color
RayTracer::shade(const Ray& ray, Intersection& hit, int level, float weight)
//[]---------------------------------------------------[]
//|  Shade a point P                                    |
//|  @param the ray (input)                             |
//|  @param information on intersection (input)         |
//|  @param recursion level                             |
//|  @param ray weight                                  |
//|  @return color at point P                           |
//[]---------------------------------------------------[]
{
	auto color = Color::black;

	const auto& data = hit.object->mesh()->data();
	auto triangle = data.triangles[hit.triangleIndex];

	// get vertices
	vec3f v0 = data.vertices[triangle.v[0]];
	vec3f v1 = data.vertices[triangle.v[1]];
	vec3f v2 = data.vertices[triangle.v[2]];

	// get normals
	vec3f N0 = data.vertexNormals[triangle.v[0]];
	vec3f N1 = data.vertexNormals[triangle.v[1]];
	vec3f N2 = data.vertexNormals[triangle.v[2]];

	auto hitTransform = hit.object->sceneObject()->transform();

	// transformation matrices
	auto m = hitTransform->localToWorldMatrix();
	auto mT = hitTransform->worldToLocalMatrix();

	auto p = triangle::interpolate(hit.p, v0, v1, v2);	
	auto pGlobal = m.transform(p);
	
	auto N = triangle::interpolate(hit.p, N0, N1, N2);
	auto NGlobal = m.transformVector(N);

	// escape intercepting own origin of ray
	p += N * rt_eps();
	pGlobal += NGlobal * rt_eps();	

	auto material = hit.object->material;

	for (auto l : _scene->lights())
	{
		if (!l->on)
			continue;
		
		auto lGlobalPos = l->sceneObject()->transform()->position();		
		auto lrGlobalDir = (lGlobalPos - pGlobal).versor();

		Ray lightRay{ pGlobal, lrGlobalDir };
		lightRay.tMax = (lGlobalPos - p).length();

		Intersection lrHit;

		// gets global origin of ray
		if (!intersect(lightRay, lrHit))
		{
			// light ray direction
			auto lPos = mT.transform(lGlobalPos);
			auto lrDir = (lPos - p).versor();
			auto rayOrigin = m.transform(ray.origin);
			color += phong(rayOrigin, *l, p, N, lPos, lrDir, material, _scene->ambientLight);
		}
	}
	
	// reflection color
	if (level < _maxRecursionLevel)
	{
		auto Or = material.specular;
		if (Or.r > 0 || Or.g > 0 || Or.b > 0)
		{
			float w = weight * std::max(Or.r,std::max(Or.g,Or.b));
			if (w > _minWeight)
			{
				auto Ll = ray.direction.versor();
				auto Rl = Ll - 2 * NGlobal.dot(Ll) * NGlobal;
				Ray reflectRay{ pGlobal, Rl };
				color += Or * trace(reflectRay, level + 1, w);
			}
		}
	}
	return color;
}

Color
RayTracer::background() const
//[]---------------------------------------------------[]
//|  Background                                         |
//|  @return background color                           |
//[]---------------------------------------------------[]
{
  return _scene->backgroundColor;
}

bool
RayTracer::shadow(const Ray& ray)
//[]---------------------------------------------------[]
//|  Verifiy if ray is a shadow ray                     |
//|  @param the ray (input)                             |
//|  @return true if the ray intersects an object       |
//[]---------------------------------------------------[]
{
  Intersection hit;
  return intersect(ray, hit);
}

} // end namespace cg
