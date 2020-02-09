//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Orthrus Group.                               |
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
// OVERVIEW: Primitive.cpp
// ========
// Source file for primitive.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 30/10/2018

#include "Primitive.h"
#include "Transform.h"

namespace cg
{ // begin namespace cg


bool
Primitive::intersect(const Ray& ray, Intersection& hit) const
{
	if (_mesh == nullptr)
		return false;

	auto t = const_cast<Primitive*>(this)->transform();
	Ray localRay{ ray, t->worldToLocalMatrix() }; 
	auto d = math::inverse(localRay.direction.length());
	float tMin;
	float tMax;

	localRay.direction *= d;
	
	mat4f Ml = t->localToWorldMatrix();

	// testa com a BVH
	/*Inicializa a pilha com o push da raíz da BVH.
		Enquanto a pilha não está vazia :
			Retira do topo
			Calcula a interseção com o raio
			Caso intercepte, checar se o nó é folha.
			Caso não seja folha, adicionar os dois nós na pilha.
			Caso seja folha, calcular a interseção com cada triângulo e manter o mais próximo.*/
	/*auto bvh = bvhMap[_mesh];

	if (bvh == nullptr)
		bvhMap[_mesh] = bvh = new BVH{ *_mesh, 16 };

	if (bvh->intersect(ray, hit, d))
	{
		hit.object = this;
		return true;
	}
	return false;*/

	if (_mesh->bounds().intersect(localRay, tMin, tMax))
	{
		auto data = _mesh->data();
		bool intersected = false;
		for (int i = 0; i < data.numberOfTriangles; i++)
		{
			auto t = data.triangles[i];
			auto p0 = data.vertices[t.v[0]];
			auto p1 = data.vertices[t.v[1]];
			auto p2 = data.vertices[t.v[2]];

			vec3f e1 = p1 - p0;					// Passo 1.

			vec3f e2 = p2 - p0;					// Passo 2.

			vec3f D = localRay.direction;		// Passo 3.
			vec3f S1 = D.cross(e2);

			float S1e1 = S1.dot(e1);			// Passo 4.
			if (-0.001 < S1e1 < 0.001) continue;
			float invS1e1 = math::inverse(S1e1);

			vec3f Or = localRay.origin;			// Passo 5.
			vec3f S = Or - p0;

			vec3f S2 = S.cross(e1);				// Passo 6

			float s2e2 = S2.dot(e2);			// Passo 7
			float tl = s2e2 * invS1e1;
			if (tl < 0) continue;

			float s1S = S1.dot(S);				// Passo 8
			float b1 = (s1S) * invS1e1;
			if (b1 < 0)	continue;

			float s2D = S2.dot(D);				// Passo 9
			float b2 = s2D * invS1e1;
			if (b2 < 0)	continue;

			if (b1 + b2 > 1) continue;			// Passo 10
			auto b0 = 1 - b1 - b2;
			
			vec3f Dl = localRay.direction.versor();
			auto D2 = Ml.transformVector(Dl);
			float dist = tl * D2.length();
			if (dist < ray.tMax && dist < hit.distance)
			{
				hit.distance = dist;
				hit.p = vec3f{ b0,b1,b2 };
				hit.object = this;
				hit.triangleIndex = i;
				intersected = true;
			}					
		}
		return intersected;
	}
	return false;
}

} // end namespace cg
