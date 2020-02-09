#include "geometry/MeshSweeper.h"
#include "P4.h"
#include <iostream>
#include <iterator>

MeshMap P4::_defaultMeshes;
MaterialMap P4::_defaultMaterials;

// transforma de coordenadas homogêneas para coordenadas cartesianas, joga fora o w
inline auto
normalize(const vec4f& p)
{
	return vec3f{ p } *math::inverse(p.w);
}

// pega a equação 5.24 (cap5) e inverte ela, speech na sala…
// é assim que se pega a posição do mouse e joga pro espaço, chuta um zn
inline auto
viewportToNDC(int x, int y)
{
	GLint v[4];

	glGetIntegerv(GL_VIEWPORT, v);

	const auto xn = float(x - v[0]) * 2.0f / float(v[2]) - 1.0f;
	const auto yn = float(y - v[1]) * 2.0f / float(v[3]) - 1.0f;

	return vec4f{ xn, yn, -1, 1 };
}

inline Ray
P4::makeRay(int x, int y) const
{
	auto _editor = _currentScene->_editor;

	auto c = _editor->camera();
	mat4f m{ vpMatrix(c) };

	m.invert();

	// p é o ponto em coordenada globais de onde o mouse esta
	auto p = normalize(m * viewportToNDC(x, height() - y));
	auto t = c->transform();
	// cria um raio r com origem na posição da câmera e direção de -z (- forward())
	Ray r{ t->position(), -t->forward() };

	if (c->projectionType() == Camera::Perspective) // se for camera perspec ajeite a direção
		r.direction = (p - r.origin).versor();
	else // se for paralela, ajeite a origem
		r.origin = p - r.direction * c->nearPlane();
	return r;
}

inline void
P4::buildDefaultMeshes()
{
	_defaultMeshes["None"] = nullptr;
	_defaultMeshes["Box"] = GLGraphics3::box();
	_defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline void
P4::buildDefaultMaterials()
{
	static Material soft;
	{
		soft.ambient = Color{ 123, 154, 119 };
		soft.diffuse = Color{ 255, 164, 10 };
		soft.spot = Color::black;
		soft.shine = 100.0f;
		soft.specular = Color{ 202, 202, 203 };
	}
	static Material hard;
	{
		hard.ambient = Color{ 69, 69, 69 };
		hard.diffuse = Color{ 101, 127, 196 };
		hard.spot = Color{129, 0, 0};
		hard.shine = 80.0f;
		hard.specular = Color{ Color::white };
	}
	static Material curly;
	{
		curly.ambient = Color{51,51,51};
		curly.diffuse = Color{204, 204, 204};
		curly.spot = Color{0,0,0};
		curly.shine = 30.0f;
		curly.specular = Color{160, 137, 225};
	}
	static Material charm;
	{
		curly.ambient = Color{117, 117, 118};
		curly.diffuse = Color{204, 204, 204};
		curly.spot = Color{81, 0, 0};
		curly.shine = 100.0f;
		curly.specular = Color{Color::black};
	}
	_defaultMaterials["Soft"] = &soft;
	_defaultMaterials["Hard"] = &hard;
	_defaultMaterials["Curly"] = &curly;
	_defaultMaterials["Charm"] = &charm;
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
	return new Primitive(mit->second, mit->first);
}

inline void
P4::buildScene1()
{
	_current = _currentScene = _scene1 = new Scene{ "Scene 1" };
	auto _scene = _scene1;
	_scene->_editor = new SceneEditor{ *_scene };
	//_editor = new SceneEditor{ *_scene };
	_scene->_editor->setDefaultView((float)width() / (float)height());

	// color of first scene
	 _scene->backgroundColor = Color { 63, 63, 63 };
	 _scene->ambientLight = Color { 115, 0, 64 };
	// **Begin initialization of temporary attributes
	// It should be replaced by your scene initialization
	{
		// cria uma luz Direcional e posiciona
		auto o = new SceneObject{ "Point Light 0", *_scene };
		o->setType(SceneObject::Light);

		o->setParent(_scene->root());
		_scene->root()->addChild(o);									
		
		auto dLight = new Light;
		dLight->setType(Light::Point);
		Color color{ 50,100,250 };
		dLight->color = Color::white;
		dLight->setFalloff(0.0f);
		o->addComponent(dLight);
		_scene->addLight(*dLight); // insert into list of lights of scene

		//o->transform()->setLocalEulerAngles(vec3f{ 0,0,0 });
		o->transform()->setLocalPosition(vec3f{ 0, 1.3, -1.4 });
		// ###
		// cria uma box
		o = new SceneObject{ "Root Box 1", *_scene };
		auto p = makePrimitive(_defaultMeshes.find("Box"));
		p->material = *_defaultMaterials["Soft"];

		o->addComponent(p);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalPosition(vec3f{ 1.5f,0,-1 });
		o->transform()->setLocalEulerAngles(vec3f{ 0, -45.0f, 0.0f });
		o->transform()->setLocalScale(vec3f{ 1, 1.3, 1 });

		// cria uma box
		o = new SceneObject{ "Root Box 2", *_scene };
		p = makePrimitive(_defaultMeshes.find("Box"));
		// p->material = default
		o->addComponent(p);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalPosition(vec3f{-1.4f,0, -2.4});
		o->transform()->setLocalEulerAngles(vec3f{ 0, 0, 0 });
		o->transform()->setLocalScale(vec3f{ 0.2, 0.2, 0.2 });
		//o->transform()->setScale(vec3f{ 1,0.5,1 });

		// cria uma box
		o = new SceneObject{ "Root Box 3", *_scene };
		p = makePrimitive(_defaultMeshes.find("Box"));
		p->material = *_defaultMaterials["Hard"];
		o->addComponent(p);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalPosition(vec3f{ -1.2f, 0.0f, -1.4f});
		o->transform()->setLocalEulerAngles(vec3f{ 9.9, 46, -11 });
		o->transform()->setLocalScale(vec3f{ 0.5f, 0.5f, 0.5f });

		// cria uma box
		o = new SceneObject{ "Root Box 4", *_scene };
		p = makePrimitive(_defaultMeshes.find("Box"));
		p->material = *_defaultMaterials["Curly"];
		o->addComponent(p);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalPosition(vec3f{ -0.7f, 0.0f, -0.4f});
		o->transform()->setLocalEulerAngles(vec3f{ 0,0,0 });
		o->transform()->setLocalScale(vec3f{ 0.2f, 0.2f, 0.2f });

		// cria uma esfera
		o = new SceneObject{ "Root Sphere", *_scene };
		p = makePrimitive(_defaultMeshes.find("Sphere"));
		//p->material = default;
		o->addComponent(p);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalPosition(vec3f{0, 0, -2});
		o->transform()->setLocalEulerAngles(vec3f{ 0,0,0 });
		o->transform()->setLocalScale(vec3f{ 0.2f, 0.2f, 0.2f });

		//// Camera:
		//// cria uma camera e coloca numa posicao desejada
		//o = new SceneObject{ "Main Camera", *_scene };
		//auto camera = new Camera;
		//o->addComponent(camera);
		//o->setType(o->Camera);
		//o->setParent(_scene->root());
		//_scene->root()->addChild(o);
		//o->transform()->setLocalEulerAngles(vec3f{ -21,0.6,0.6 });
		//o->transform()->setLocalPosition(vec3f(-1.3, 4.8, 8.9));

		o = new SceneObject{ "Camera 0", *_scene };
		auto camera = new Camera;
		o->addComponent(camera);
		o->setType(o->Camera);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalEulerAngles(vec3f{ 0,180,0 });
		o->transform()->setLocalPosition(vec3f(0,0,-5.2));
		//camera->nearPlane = 0.5;
		_scene->setCurrentCamera(camera);
		// ###
	}
	// **End initialization of temporary attributes
}

inline void
P4::buildScene2()
{
	_scene2 = new Scene{ "Scene 2" };
	auto _scene = _scene2;
	_scene->_editor = new SceneEditor{ *_scene };
	_scene->_editor->setDefaultView((float)width() / (float)height());

	// color of second scene 
	_scene->backgroundColor = Color{33, 65, 31 };
	_scene->ambientLight = Color{ 89, 67, 39 };
	// **Begin initialization of temporary attributes
	// It should be replaced by your scene initialization
	{
		auto o = new SceneObject{ "Camera 0", *_scene };
		auto camera = new Camera;
		o->addComponent(camera);
		o->setType(o->Camera);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalEulerAngles(vec3f{ 0,0,0 });
		o->transform()->setLocalPosition(vec3f(0, 0, 7));
		_scene->setCurrentCamera(camera);

		o = new SceneObject{ "Root Box 1", *_scene };
		auto p = makePrimitive(_defaultMeshes.find("Box"));
		p->material = *_defaultMaterials["Soft"];
		p->material.specular = Color{ Color::yellow };

		o->addComponent(p);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalPosition(vec3f{ 2,0,0 });
		o->transform()->setLocalEulerAngles(vec3f{ 0, -45.0f, 0.0f });
		o->transform()->setLocalScale(vec3f{ 1, 1, 1 });

		o = new SceneObject{ "Root Box 2", *_scene };
		p = makePrimitive(_defaultMeshes.find("Box"));
		//p->material = *_defaultMaterials["Soft"];
		p->material.specular = Color{ Color::blue };
		o->addComponent(p);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalPosition(vec3f{ -1.7,0,0.9 });
		o->transform()->setLocalEulerAngles(vec3f{ 0, 0, 0.0f });
		o->transform()->setLocalScale(vec3f{0.2, 0.2, 0.2 });

		// cria uma esfera
		o = new SceneObject{ "Root Sphere", *_scene };
		p = makePrimitive(_defaultMeshes.find("Sphere"));
		//p->material = default;
		o->addComponent(p);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		o->transform()->setLocalPosition(vec3f{ -0.9, 0, 1.1 });
		o->transform()->setLocalEulerAngles(vec3f{ 0,0,0 });
		o->transform()->setLocalScale(vec3f{ 0.5f, 0.5f, 0.5f });

		o = new SceneObject{ "Point Light 0", *_scene };
		o->setType(SceneObject::Light);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		auto dLight = new Light;
		dLight->setType(Light::Point);
		dLight->color = Color{ 170, 82, 82 };
		// dLight->setFalloff(default = 1);
		o->addComponent(dLight);
		_scene->addLight(*dLight); // insert into list of lights of scene
		o->transform()->setLocalPosition(vec3f{ 0,2,0 });

		o = new SceneObject{ "Directional Light 0", *_scene };
		o->setType(SceneObject::Light);
		o->setParent(_scene->root());
		_scene->root()->addChild(o);
		dLight = new Light;
		dLight->setType(Light::Directional);
		dLight->color = Color{ Color::white };
		o->addComponent(dLight);
		_scene->addLight(*dLight); // insert into list of lights of scene
		o->transform()->setLocalPosition(vec3f{ 0,0,-10 });
		o->transform()->setLocalEulerAngles(vec3f{ 0,180,10 });
	}
	// **End initialization of temporary attributes
}

void
P4::initialize()
{
	Application::loadShaders(_phongProgram, "shaders/phong.vs", "shaders/phong.fs");
	Application::loadShaders(_gouraudProgram, "shaders/gouraud.vs", "shaders/gouraud.fs");

	Assets::initialize();
	buildDefaultMeshes();
	buildDefaultMaterials();
	
	buildScene1();
	buildScene2();

	_scene1->_renderer = new GLRenderer{ *_scene1 };
	_scene1->_rayTracer = new RayTracer{ *_scene1 };

	_scene2->_renderer = new GLRenderer{ *_scene2 };
	_scene2->_rayTracer = new RayTracer{ *_scene2 };

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glEnable(GL_LINE_SMOOTH);

	// tonalizacao de gouraud para sceneEditor
	_gouraudProgram.use();
}

void P4::addSphere(SceneObject* o)
{
	auto _scene = _currentScene;

	static int newSphereCount = 1;
	static char newSphereName[11];
	snprintf(newSphereName, sizeof(newSphereName), "Sphere %d", newSphereCount++);

	auto newSphere = new SceneObject{ newSphereName, *_scene };
	newSphere->addComponent(makePrimitive(_defaultMeshes.find("Sphere")));
	newSphere->setType(newSphere->ThreeDObject);
	newSphere->setParent(_scene->root());
	o->addChild(newSphere);
}

void P4::addBox(SceneObject* o)
{
	auto _scene = _currentScene;

	static int newBoxCount = 1;
	static char newBoxName[11];
	snprintf(newBoxName, sizeof(newBoxName), "Box %d", newBoxCount++);

	auto newBox = new SceneObject{ newBoxName, *_scene };
	newBox->addComponent(makePrimitive(_defaultMeshes.find("Box")));
	newBox->setType(newBox->ThreeDObject);
	newBox->setParent(_scene->root());
	std::cout << "In insert box, parentName: " << newBox->parent()->name();
	o->addChild(newBox);
}

void P4::addLight(SceneObject* o, int type)
{
	auto _scene = _currentScene;

	static int pCount = 1, dCount = 1, sCount = 1;
	static char pName[21], dName[21], sName[21];

	static SceneObject* lightObj;
	static Light* sLight, * pLight, * dLight;

	switch (type)
	{
	case Light::Spot:
		// sets name
		snprintf(sName, sizeof(sName), "Spot Light %d\0", sCount++);

		// instanciate object
		lightObj = new SceneObject{ sName, *_scene };
		lightObj->setType(SceneObject::Light);

		// create Point light component
		sLight = new Light;
		sLight->setType(Light::Spot);
		lightObj->addComponent(sLight);
		// insert into list of lights of scene
		_scene->addLight(*sLight); // _lights.push_back(sLight);

		// sets parenthood
		lightObj->setParent(_scene->root());
		o->addChild(lightObj);
		break;

	case Light::Point:
		// sets name
		snprintf(pName, sizeof(pName), "Point Light %d\0", pCount++);

		// instanciate object
		lightObj = new SceneObject{ pName, *_scene };
		lightObj->setType(SceneObject::Light);

		// create Point light component
		pLight = new Light;
		pLight->setType(Light::Point);
		lightObj->addComponent(pLight);

		// insert into list of lights of scene
		_scene->addLight(*pLight); // _lights.push_back(pLight);

		// sets parenthood
		lightObj->setParent(_scene->root());
		o->addChild(lightObj);
		break;

	case Light::Directional:
		// sets name
		snprintf(dName, sizeof(dName), "Directional Light %d\0", dCount++);

		// instanciate object
		lightObj = new SceneObject{ dName, *_scene };
		lightObj->setType(SceneObject::Light);

		// create directional light component
		dLight = new Light;
		dLight->setType(Light::Directional);
		lightObj->addComponent(dLight);
		// insert into list of lights of scene
		_scene->addLight(*dLight); //_lights.push_back(dLight);

		// sets parenthood
		lightObj->setParent(_scene->root());
		o->addChild(lightObj);
		break;
	}
}

void P4::addCamera(SceneObject* o)
{
	auto _scene = _currentScene;

	static int newCCount = 1;
	static char newCName[10];
	snprintf(newCName, sizeof(newCName), "Camera %d", newCCount++);

	auto newCamera = new SceneObject(newCName, *_scene);
	newCamera->addComponent(new Camera);
	newCamera->setType(newCamera->Camera);
	newCamera->setParent(_scene->root());
	o->addChild(newCamera);
}

void P4::addEmptyObject(SceneObject* o)
{
	auto _scene = _currentScene;

	static int objectCount = 1;
	static char objectName[11];
	snprintf(objectName, sizeof(objectName), "Object %d", objectCount++);

	auto emptyObject = new SceneObject{ objectName, *_scene };
	emptyObject->setType(emptyObject->EmptyObject);
	emptyObject->setParent(_scene->root());
	o->addChild(emptyObject);
}

void P4::switchScenes()
{
	_currentScene = _scene1 == _currentScene ? _scene2 : _scene1;
	_current = _currentScene;
}

void P4::hierarchyTree(SceneObject* o)
{
	ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnArrow };

	if (!o->hasKids()) // if doesn't have kids: is a leaf
	{
		flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx(o,
			_current == o ? flag | ImGuiTreeNodeFlags_Selected : flag,
			o->name());

		if (ImGui::IsItemClicked())
			_current = o;

		// take object to new parent
		if (ImGui::BeginDragDropSource())
		{
			ImGui::Text(dynamic_cast<cg::SceneObject*>(_current)->name());
			ImGui::SetDragDropPayload("newParent", &_current, sizeof(SceneObject*));
			ImGui::EndDragDropSource();
		}

		// receives object as child and adds it to children's list
		if (ImGui::BeginDragDropTarget()) {

			if (auto* payload = ImGui::AcceptDragDropPayload("newParent"))
			{
				auto* sourceObj = *(cg::SceneObject**)payload->Data;

				o->addChild(sourceObj);
				sourceObj->parent()->removeChild(sourceObj);
				sourceObj->setParent(o);
			}
			ImGui::EndDragDropTarget();
		}
	}
	else // if has kids: is a regular node
	{
		auto open = (ImGui::TreeNode(o->name()));

		if (ImGui::IsItemClicked())
			_current = o;

		// takes object to new parent
		if (ImGui::BeginDragDropSource())
		{
			ImGui::Text(dynamic_cast<cg::SceneObject*>(_current)->name());
			ImGui::SetDragDropPayload("newParent", &_current, sizeof(SceneObject*));
			ImGui::EndDragDropSource();
		}

		// receives object as child and adds it to children's list
		if (ImGui::BeginDragDropTarget()) {
			if (auto* payload = ImGui::AcceptDragDropPayload("newParent"))
			{
				auto* sourceObj = *(cg::SceneObject**)payload->Data;

				sourceObj->parent()->removeChild(sourceObj);
				sourceObj->setParent(o);
				o->addChild(sourceObj);
			}
			ImGui::EndDragDropTarget();
		}

		if (open)
		{
			for (auto child : o->children())
				hierarchyTree(child);
			ImGui::TreePop();
		}
	}
}

inline void
P4::hierarchyWindow()
{
	auto _scene = _currentScene;
	ImGui::Begin("Hierarchy");
	if (ImGui::Button("Create###object"))
		ImGui::OpenPopup("CreateObjectPopup");
	if (ImGui::BeginPopup("CreateObjectPopup"))
	{
		if (ImGui::MenuItem("Empty Object"))
		{
			addEmptyObject(_scene->root());
		}
		if (ImGui::BeginMenu("3D Object"))
		{
			if (ImGui::MenuItem("Box"))
			{
				addBox(_scene->root());
			}
			if (ImGui::MenuItem("Sphere"))
			{
				addSphere(_scene->root());
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Directional Light"))
			{
				addLight(_scene->root(), Light::Directional);
			}
			if (ImGui::MenuItem("Point Light"))
			{
				addLight(_scene->root(), Light::Point);
			}
			if (ImGui::MenuItem("Spot light"))
			{
				addLight(_scene->root(), Light::Spot);
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Camera"))
		{
			addCamera(_scene->root());
		}
		ImGui::EndPopup();
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete"))
	{
		if (SceneObject* object = dynamic_cast<SceneObject*>(_current))
		{
			// checks if child has a light and removes it if so
			_scene->removeLight(object);
		 	object->parent()->deleteChild(object);
		}
	}

	ImGui::Separator();

	ImGuiTreeNodeFlags f{ ImGuiTreeNodeFlags_OpenOnArrow };

	auto open = ImGui::TreeNodeEx(_scene,
		_current == _scene ? f | ImGuiTreeNodeFlags_Selected : f,
		_scene->name());

	if (ImGui::IsItemClicked())
		_current = _scene;
	if (open)
	{
		for (const auto o : _scene->root()->children())
			hierarchyTree(o);
		ImGui::TreePop();
	}
	ImGui::End();
}

namespace ImGui
{ // begin namespace ImGui

	void
		ObjectNameInput(NameableObject* object)
	{
		const int bufferSize{ 128 };
		static NameableObject* current;
		static char buffer[bufferSize];

		if (object != current)
		{
			strcpy_s(buffer, bufferSize, object->name());
			current = object;
		}
		if (ImGui::InputText("Name", buffer, bufferSize))
			object->setName(buffer);
	}

	inline bool
		ColorEdit3(const char* label, Color& color)
	{
		return ImGui::ColorEdit3(label, (float*)&color);
	}

	inline bool
		DragVec3(const char* label, vec3f& v)
	{
		return DragFloat3(label, (float*)&v, 0.1f, 0.0f, 0.0f, "%.2g");
	}

	void
		TransformEdit(Transform* transform)
	{
		vec3f temp;

		temp = transform->localPosition();
		if (ImGui::DragVec3("Position", temp))
			transform->setLocalPosition(temp);
		temp = transform->localEulerAngles();
		if (ImGui::DragVec3("Rotation", temp))
			transform->setLocalEulerAngles(temp);
		temp = transform->localScale();
		if (ImGui::DragVec3("Scale", temp)) {
			float minScale = 0.01;
			if (temp.x < minScale)
				temp.x = minScale;
			if (temp.y < minScale)
				temp.y = minScale;
			if (temp.z < minScale)
				temp.z = minScale;

			transform->setLocalScale(temp);
		}
	}

} // end namespace ImGui

inline void
P4::sceneGui()
{
	auto scene = (Scene*)_current;

	ImGui::ObjectNameInput(_current);
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Colors"))
	{
		ImGui::ColorEdit3("Background", scene->backgroundColor);
		ImGui::ColorEdit3("Ambient Light", scene->ambientLight);
	}
}

inline void
P4::inspectShape(Primitive& primitive)
{
	char buffer[16];

	snprintf(buffer, 16, "%s", primitive.meshName());
	ImGui::InputText("Mesh", buffer, 16, ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		if (auto* payload = ImGui::AcceptDragDropPayload("PrimitiveMesh"))
		{
			auto mit = *(MeshMapIterator*)payload->Data;
			primitive.setMesh(mit->second, mit->first);
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::SameLine();
	if (ImGui::Button("...###PrimitiveMesh"))
		ImGui::OpenPopup("PrimitiveMeshPopup");
	if (ImGui::BeginPopup("PrimitiveMeshPopup"))
	{
		auto& meshes = Assets::meshes();

		if (!meshes.empty())
		{
			for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
				if (ImGui::Selectable(mit->first.c_str()))
					primitive.setMesh(Assets::loadMesh(mit), mit->first);
			ImGui::Separator();
		}
		for (auto mit = _defaultMeshes.begin(); mit != _defaultMeshes.end(); ++mit)
			if (ImGui::Selectable(mit->first.c_str()))
				primitive.setMesh(mit->second, mit->first);
		ImGui::EndPopup();
	}
}

inline void
P4::inspectMaterial(Material& material)
{
	ImGui::ColorEdit3("Ambient", material.ambient);
	ImGui::ColorEdit3("Diffuse", material.diffuse);
	ImGui::ColorEdit3("Spot", material.spot);
	ImGui::ColorEdit3("Specular", material.specular);
	ImGui::DragFloat("Shine", &material.shine, 1, 0, 1000.0f);
}

inline void
P4::inspectPrimitive(Primitive& primitive)
{
	//const auto flag = ImGuiTreeNodeFlags_NoTreePushOnOpen;

	//if (ImGui::TreeNodeEx("Shape", flag))
	inspectShape(primitive);
	//if (ImGui::TreeNodeEx("Material", flag))
	inspectMaterial(primitive.material);
}

inline void
P4::inspectLight(Light& light)
{
	static const char* lightTypes[]{ "Directional", "Point", "Spot" };
	auto lt = light.type();

	if (ImGui::BeginCombo("Type", lightTypes[lt]))
	{
		for (auto i = 0; i < IM_ARRAYSIZE(lightTypes); ++i)
		{
			bool selected = lt == i;

			if (ImGui::Selectable(lightTypes[i], selected))
				lt = (Light::Type)i;
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	light.setType(lt);

	auto falloff = light.falloff();

	if (light.type() != Light::Directional)
		if (ImGui::SliderInt("Falloff", &falloff, 0, 2))
			light.setFalloff(falloff);

	auto ghama = light.ghama();

	if (light.type() == Light::Spot)
		if (ImGui::SliderFloat("Ghama", &ghama, 0, 60))
			light.setGhama(ghama);

	auto fe = light.fallExponent();

	if (light.type() == Light::Spot)
		if (ImGui::SliderFloat("fallExponent", &fe, 0, 5))
			light.setFallExponent(fe);

	ImGui::ColorEdit3("Color", light.color);

	ImGui::Checkbox("On", &light.on);
}

void
P4::inspectCamera(Camera& camera)
{
	static const char* projectionNames[]{ "Perspective", "Orthographic" };
	auto cp = camera.projectionType();

	if (ImGui::BeginCombo("Projection", projectionNames[cp]))
	{
		for (auto i = 0; i < IM_ARRAYSIZE(projectionNames); ++i)
		{
			auto selected = cp == i;

			if (ImGui::Selectable(projectionNames[i], selected))
				cp = (Camera::ProjectionType)i;
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	camera.setProjectionType(cp);
	if (cp == View3::Perspective)
	{
		auto fov = camera.viewAngle();

		if (ImGui::SliderFloat("View Angle",
			&fov,
			MIN_ANGLE,
			MAX_ANGLE,
			"%.0f deg",
			1.0f))
			camera.setViewAngle(fov <= MIN_ANGLE ? MIN_ANGLE : fov);
	}
	else
	{
		auto h = camera.height();

		if (ImGui::DragFloat("Height",
			&h,
			MIN_HEIGHT * 10.0f,
			MIN_HEIGHT,
			math::Limits<float>::inf()))
			camera.setHeight(h <= 0 ? MIN_HEIGHT : h);
	}

	float n;
	float f;

	camera.clippingPlanes(n, f);

	if (ImGui::DragFloatRange2("Clipping Planes",
		&n,
		&f,
		MIN_DEPTH,
		MIN_DEPTH,
		math::Limits<float>::inf(),
		"Near: %.2f",
		"Far: %.2f"))
	{
		if (n <= 0)
			n = MIN_DEPTH;
		if (f - n < MIN_DEPTH)
			f = n + MIN_DEPTH;
		camera.setClippingPlanes(n, f);
	}
}

inline void
P4::addComponentButton(SceneObject& object)
{
	auto _scene = _currentScene;

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponentPopup");
	if (ImGui::BeginPopup("AddComponentPopup"))
	{
		if (ImGui::MenuItem("Primitive"))
		{
			auto hasComponent = object.components().has("Primitive");
			if (!hasComponent)
				object.addComponent(makePrimitive(_defaultMeshes.find("Box")));
		}
		if (ImGui::MenuItem("Light"))
		{
			auto hasComponent = object.components().has("Light");
			if (!hasComponent)
			{
				auto l = new Light;
				object.addComponent(l);
				_scene->addLight(*l); //_lights.push_back(l);
			}
		}
		if (ImGui::MenuItem("Camera"))
		{
			auto hasComponent = object.components().has("Camera");
			if (!hasComponent)
				object.addComponent(new Camera);
		}
		ImGui::EndPopup();
	}
}

void
P4::processComponents(ComponentCollection& components)
{
	auto _scene = _currentScene;

	for (auto it = components.begin(); it != components.end(); it++)
	{
		auto component = *it;
		if (auto p = dynamic_cast<Primitive*>(component))
		{
			auto notDelete{ true };
			auto open = ImGui::CollapsingHeader(p->typeName(), &notDelete);

			if (!notDelete)
				it = components.remove(it);
			else if (open)
				inspectPrimitive(*p);
		}

		else if (auto l = dynamic_cast<Light*>(component))
		{
			auto notDelete{ true };
			auto open = ImGui::CollapsingHeader(l->typeName(), &notDelete);

			if (!notDelete)
			{
				_scene->removeLight(l);
				it = components.remove(it);
			}
			else if (open)
				inspectLight(*l);
		}

		else if (auto c = dynamic_cast<Camera*>(component))
		{
			auto notDelete{ true };
			auto open = ImGui::CollapsingHeader(c->typeName(), &notDelete);

			if (!notDelete)
				it = components.remove(it);
			else if (open)
			{
				auto isCurrent = c == _scene->currentCamera();

				ImGui::Checkbox("Current", &isCurrent);
				_scene->setCurrentCamera(isCurrent ? c : nullptr);
				//Camera::setCurrent(isCurrent ? c : nullptr);

				/*ImGui::SameLine();

				ImGui::Checkbox("Draw Camera", &c->show);*/

				inspectCamera(*c);
			}
		}
	}
}

inline void
P4::sceneObjectGui()
{
	auto object = (SceneObject*)_current;

	addComponentButton(*object);
	ImGui::Separator();
	ImGui::ObjectNameInput(object);
	ImGui::SameLine();
	ImGui::Checkbox("###visible", &object->visible);
	ImGui::Separator();
	if (ImGui::CollapsingHeader(object->transform()->typeName()))
		ImGui::TransformEdit(object->transform());

	processComponents((ComponentCollection&)object->components());
}

inline void
P4::objectGui()
{
	if (_current == nullptr)
		return;
	if (dynamic_cast<SceneObject*>(_current))
	{
		sceneObjectGui();
		return;
	}
	if (dynamic_cast<Scene*>(_current))
		sceneGui();
}

inline void
P4::inspectorWindow()
{
	ImGui::Begin("Inspector");
	objectGui();
	ImGui::End();
}

inline void
P4::editorViewGui()
{
	//_editor = SceneEditor::current()
	auto _editor = _currentScene->_editor;

	if (ImGui::Button("Set Default View"))
		_editor->setDefaultView(float(width()) / float(height()));
	ImGui::Separator();

	auto t = _editor->camera()->transform();
	vec3f temp;

	temp = t->localPosition();
	if (ImGui::DragVec3("Position", temp))
		t->setLocalPosition(temp);
	temp = t->localEulerAngles();
	if (ImGui::DragVec3("Rotation", temp))
		t->setLocalEulerAngles(temp);
	inspectCamera(*_editor->camera());
	ImGui::Separator();
	{
		static int sm;

		ImGui::Combo("Shading Mode", &sm, "None\0Flat\0Gouraud\0\0");
		// TODO

		static Color edgeColor;
		static bool showEdges;

		ImGui::ColorEdit3("Edges", edgeColor);
		ImGui::SameLine();
		ImGui::Checkbox("###showEdges", &showEdges);
	}
	ImGui::Separator();
	ImGui::Checkbox("Show Ground", &_editor->showGround);
}

inline void
P4::assetsWindow()
{
	ImGui::Begin("Assets");
	if (ImGui::CollapsingHeader("Meshes"))
	{
		auto& meshes = Assets::meshes();

		for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
		{
			auto meshName = mit->first.c_str();
			auto selected = false;

			ImGui::Selectable(meshName, &selected);
			if (ImGui::BeginDragDropSource())
			{
				Assets::loadMesh(mit);
				ImGui::Text(meshName);
				ImGui::SetDragDropPayload("PrimitiveMesh", &mit, sizeof(mit));
				ImGui::EndDragDropSource();
			}
		}
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Textures"))
	{
		// next semester
	}
	ImGui::End();
}

inline void
P4::editorView()
{
	if (!_currentScene->_showEditorView)
		return;
	ImGui::Begin("Editor View Settings");
	editorViewGui();
	ImGui::End();
}

inline void
P4::fileMenu()
{
	if (ImGui::MenuItem("New"))
	{
		// TODO
	}
	if (ImGui::MenuItem("Open...", "Ctrl+O"))
	{
		// TODO
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Save", "Ctrl+S"))
	{
		// TODO
	}
	if (ImGui::MenuItem("Save As..."))
	{
		// TODO
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Exit", "Alt+F4"))
	{
		shutdown();
	}
}

inline bool
showStyleSelector(const char* label)
{
	static int style = 1;

	if (!ImGui::Combo(label, &style, "Classic\0Dark\0Light\0"))
		return false;
	switch (style)
	{
	case 0: ImGui::StyleColorsClassic();
		break;
	case 1: ImGui::StyleColorsDark();
		break;
	case 2: ImGui::StyleColorsLight();
		break;
	}
	return true;
}

inline void
P4::showOptions()
{
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
	showStyleSelector("Color Theme##Selector");
	ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
	ImGui::PopItemWidth();
}

inline void
P4::mainMenu()
{
	auto _scene = _currentScene;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			fileMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			//if (Camera::current() == 0)
			if(_scene->currentCamera() == nullptr)
				ImGui::MenuItem("Edit View", nullptr, true, false);
			else
			{
				static const char* viewLabels[]{ "Editor", "Renderer" };

				auto& _viewMode = _scene->_viewMode;
				if (ImGui::BeginCombo("View", viewLabels[_viewMode]))
				{
					for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
					{
						if (ImGui::Selectable(viewLabels[i], _viewMode == i))
							_viewMode = (ViewMode)i;
					}
					ImGui::EndCombo();
					// TODO: change mode only if scene has changed
					if (_viewMode == ViewMode::Editor)
						_scene->_image = nullptr;
				}
			}
			auto& _showAssets = _scene->_showAssets;
			auto& _showEditorView = _scene->_showEditorView;
			
			ImGui::Separator();
			ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
			ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::BeginMenu("Options"))
			{
				showOptions();
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Scenes"))
		{
			static const char* scenesLabels[]{ "Scene 1", "Scene 2" };

			if (ImGui::BeginCombo("View", scenesLabels[_sceneNum]))
			{
				for (auto i = 0; i < IM_ARRAYSIZE(scenesLabels); ++i)
				{
					if (ImGui::Selectable(scenesLabels[i], _sceneNum == i))
					{
						auto temp = (CurrentScene)i;
						if (_sceneNum != temp)
						{
							_sceneNum = temp;
							switchScenes();
						}
					}
				}
				ImGui::EndCombo();
				// TODO: change mode only if scene has changed
				if (_sceneNum == ViewMode::Editor)
					_scene->_image = nullptr;
			}

			auto& _showAssets = _scene->_showAssets;
			auto& _showEditorView = _scene->_showEditorView;

			ImGui::Separator();
			ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
			ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

	}
}

void
P4::gui()
{
	mainMenu();
	auto _viewMode = _currentScene->_viewMode;
	if (_viewMode == ViewMode::Renderer)
		return;
	hierarchyWindow();
	inspectorWindow();
	assetsWindow();
	editorView();
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
	glPolygonMode(GL_FRONT_AND_BACK, mode);
	glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

inline void
P4::drawPrimitive(Primitive& primitive)
{
	auto _scene = _currentScene;

	auto program = GLSL::Program::current();

	auto mesh = primitive.mesh();
	auto m = glMesh(mesh);

	if (nullptr == m)
		return;

	auto t = primitive.transform();
	auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

	// sets material features to the shader
	auto pm = primitive.material;
	program->setUniformVec4("m.Oa", pm.ambient);
	program->setUniformVec4("m.Od", pm.diffuse);
	program->setUniformVec4("m.Os", pm.spot);
	program->setUniform("m.Ns", pm.shine);

	program->setUniformMat4("transform", t->localToWorldMatrix());
	program->setUniformMat3("normalMatrix", normalMatrix);

	m->bind();
	drawMesh(m, GL_FILL);

	if (primitive.sceneObject() != _current)
		return;
	drawMesh(m, GL_LINE);
	// **Begin BVH test
	auto bvh = bvhMap[mesh];

	if (bvh == nullptr)
		bvhMap[mesh] = bvh = new BVH{ *mesh, 16 };
	// **End BVH test
	if (primitive.sceneObject() != _current)
		return;
	//_program.setUniformVec4("color", _selectedWireframeColor);
	//_program.setUniform("flatMode", (int)1);
	//drawMesh(m, GL_LINE);
	//_editor->setVectorColor(Color::white);
	//_editor->drawNormals(*mesh, t->localToWorldMatrix(), normalMatrix);
	//_editor->setLineColor(_selectedWireframeColor);
	//_editor->drawBounds(mesh->bounds(), t->localToWorldMatrix());
	auto _editor = _scene->_editor;

	bvh->iterate([this, t, _editor](const BVHNodeInfo& node)
		{
			_editor->setLineColor(node.isLeaf ? Color::yellow : Color::magenta);
			_editor->drawBounds(node.bounds, t->localToWorldMatrix());
		});
	//_program.setUniformVec4("color", _selectedWireframeColor);
	//_program.setUniform("flatMode", (int)1);
	//drawMesh(m, GL_LINE);
	//_editor->setVectorColor(Color::white);
	//_editor->drawNormals(*mesh, t->localToWorldMatrix(), normalMatrix);
	_editor->setLineColor(Color::yellow);
	// desenha a bounding box da malha
	_editor->drawBounds(mesh->bounds(), t->localToWorldMatrix());
}

inline void
P4::drawLight(Light& light)
{
	auto _scene = _currentScene;
	auto _editor = _scene->_editor;

	std::vector<vec3f> lightVertexes;

	if (light.type() == light.Directional) {
		lightVertexes = {
			// corpo da luz directional
			{ 0,0.2,0.5 }, { 0,0.2,-0.5 }, { -0.2,0,0.5 }, { -0.2,0,-0.5 },
			{ 0,0,0.5 }, { 0,0,-0.5 }, { 0.2,0,0.5 }, { 0.2,0,-0.5 },
			{ 0,-0.2,0.5 }, { 0,-0.2,-0.5 },

			// setinhas das pontas
			{ 0,0.2,-0.5 }, { -0.125,0.2,-0.25 }, { 0,0.2,-0.5 }, { 0.125,0.2,-0.25 },
			{ -0.2,0,-0.5 }, { -0.325,0,-0.25 }, { -0.2,0,-0.5 }, { 0.325,0,-0.25 },
			{ 0,0,-0.5 }, { -0.125,0,-0.25 }, { 0,0,-0.5 }, { 0.125,0,-0.25 },
			{ 0.2,0,-0.5 }, { 0.075,0,-0.25 }, { 0.2,0,-0.5 }, { 0.375,0,-0.25 },
			{ 0,-0.2,-0.5 }, { -0.125,-0.2,-0.25 }, { 0,-0.2,-0.5 }, { 0.125,-0.2,-0.25 }
		};
	}
	else if (light.type() == light.Point) {
		lightVertexes = {
			{ 0,0.2,0 }, { 0,0,0.2 }, { 0,0,0.2 }, { 0.2,0,0 },
			{ 0.2,0,0 }, { 0,0.2,0 }, { 0,-0.2,0 }, { 0,0,-0.2 },
			{ 0,0,-0.2 }, { -0.2,0,0 }, { -0.2,0,0 }, { 0,-0.2,0 },
			{ 0,0.2,0 }, { 0,0,0.2 }, { 0,0,0.2 }, { -0.2,0,0 },
			{ -0.2,0,0 }, { 0,0.2,0 }, { 0,-0.2,0 }, { 0,0,0.2 },
			{ 0,0,0.2 }, { 0.2,0,0 }, { 0.2,0,0 }, { 0,-0.2,0 },
			{ 0,-0.2,0 }, { 0,0,0.2 }, { 0,0,0.2 }, { -0.1,0,0 },
			{ -0.2,0,0 }, { 0,-0.2,0 }, { 0,0.2,0 }, { 0,0,-0.2 },
			{ 0,0,-0.2 }, { 0.2,0,0 }, { 0.2,0,0 }, { 0,0.2,0 }
		};
	}
	else if (light.type() == light.Spot) {

		lightVertexes = {
			{ 0,0,0 }, { -0.5,1.5,-0.5 }, { 0,0,0 }, { 0.5,1.5,-0.5 },
			{ 0,0,0 }, { 0.5,1.5,0.5 }, { 0,0,0 }, { -0.5,1.5,0.5 },
			{ -0.5,1.5,-0.5 }, { 0.5,1.5,-0.5 }, { 0.5,1.5,-0.5 },  { 0.5,1.5,0.5 },
			{ 0.5,1.5,0.5 }, { -0.5,1.5,0.5 }, { -0.5,1.5,0.5 }, { -0.5,1.5,-0.5 }
		};
		// size of the draw
		float spotSize = 1.25;

		// calculates radius
		auto circleRadius = (float)tan(math::toRadians(light.ghama())) * spotSize;

		// position of circle of spot light drawing
		vec3f circleCenter{ 0, 0, -spotSize };
		vec3f circleLeftEdge{ circleRadius, 0, -spotSize };
		vec3f circleRightEdge{ -circleRadius, 0, -spotSize };
		vec3f circleUpperEdge{ 0, circleRadius, -spotSize };
		vec3f circleBottomEdge{ 0, -circleRadius, -spotSize };

		circleCenter = light.transform()->transform(circleCenter);
		circleLeftEdge = light.transform()->transform(circleLeftEdge);
		circleRightEdge = light.transform()->transform(circleRightEdge);
		circleUpperEdge = light.transform()->transform(circleUpperEdge);
		circleBottomEdge = light.transform()->transform(circleBottomEdge);

		// normal of the circle
		auto circleNormal = light.transform()->rotation() * vec3f { 0, 0, -1 };

		auto spotEdge = light.transform()->transform(vec3f(0, 0, 0));

		// save scale
		auto temp = light.transform()->localScale();
		light.transform()->setLocalScale({ 1,1,1 });

		_editor->setLineColor(light.color);
		_editor->drawCircle(circleCenter, circleRadius, circleNormal);
		_editor->drawLine(spotEdge, circleLeftEdge);
		_editor->drawLine(spotEdge, circleRightEdge);
		_editor->drawLine(spotEdge, circleUpperEdge);
		_editor->drawLine(spotEdge, circleBottomEdge);


		light.transform()->setLocalScale(temp);
		_editor->setLineColor(Color::blue);
	}

	if (light.type() != Light::Spot)
	{
		auto lightTransform = light.transform();
		auto pos = lightTransform->position();

		// save scale
		auto temp = lightTransform->localScale();

		lightTransform->setLocalScale({ 1,1,1 });

		for (int i = 0; i < lightVertexes.size(); i++) {
			lightVertexes[i] = lightTransform->transform(lightVertexes[i]);
		}

		_editor->setLineColor(light.color);
		for (int i = 0; i < lightVertexes.size() / 2; i++) {
			_editor->drawLine(lightVertexes[i * 2], lightVertexes[(i * 2) + 1]);
		}

		// reset previous scale & line color
		lightTransform->setLocalScale(temp);
		_editor->setLineColor(Color::blue);
	}
}

inline void
P4::drawCamera(Camera& camera)
{
	auto _scene = _currentScene;
	auto _editor = _scene->_editor;

	float front;
	float back;
	auto diff = camera.clippingPlanes(front, back);
	auto mg = mat4f{ camera.cameraToWorldMatrix() };

	auto alpha = math::toRadians(camera.viewAngle());
	auto theta = (float)tan(alpha * 0.5);
	auto height = camera.height();
	auto aRatio = camera.aspectRatio();

	Color fColor{ 255, 150, 0 };
	_editor->setLineColor(fColor);

	vec3f frontAxes[4];
	vec3f backAxes[4];

	if (camera.projectionType() == camera.Perspective)
	{
		// sets reasonable size of frustum
		if (diff >= 15)
			back = front + 15;

		auto fHeight = front * theta;
		auto bHeight = back * theta;

		frontAxes[0] = { -(fHeight * aRatio), fHeight, -front };
		frontAxes[2] = { fHeight * aRatio, -fHeight, -front };
		frontAxes[1] = { fHeight * aRatio, fHeight, -front };
		frontAxes[3] = { -(fHeight * aRatio), -fHeight, -front };

		backAxes[0] = { -(bHeight * aRatio), bHeight, -back };
		backAxes[2] = { bHeight * aRatio, -bHeight, -back };
		backAxes[1] = { bHeight * aRatio, bHeight, -back };
		backAxes[3] = { -(bHeight * aRatio), -bHeight, -back };
	}
	else
	{
		// sets reasonable size of frustum
		if (diff >= 15)
			back = front + 15;

		// projectional camera height
		auto pHeight = height / 2;

		frontAxes[0] = { -(pHeight * aRatio), pHeight, -front };
		frontAxes[1] = { pHeight * aRatio, pHeight, -front };
		frontAxes[2] = { pHeight * aRatio, -pHeight, -front };
		frontAxes[3] = { -(pHeight * aRatio), -pHeight, -front };

		backAxes[0] = { -(pHeight * aRatio), pHeight, -back };
		backAxes[1] = { pHeight * aRatio, pHeight, -back };
		backAxes[2] = { pHeight * aRatio, -pHeight, -back };
		backAxes[3] = { -(pHeight * aRatio), -pHeight, -back };
	}

	// sets correct position of axes
	for (int i = 0; i < 4; i++)
	{
		frontAxes[i] = mg.transform(frontAxes[i]);
		backAxes[i] = mg.transform(backAxes[i]);
	}

	// draw edges
	for (int i = 0; i < 4; i++)
	{
		_editor->drawLine(backAxes[i], backAxes[(i + 1) % 4]);
		_editor->drawLine(frontAxes[i], frontAxes[(i + 1) % 4]);
		_editor->drawLine(frontAxes[i], backAxes[i]);
	}

	Color eColor{ 0,0,255 };
	_editor->setLineColor(eColor);
}

inline void
P4::renderScene()
{
	auto _scene = _currentScene;
	auto _rayTracer = _scene->_rayTracer;

	if (auto camera = _scene->currentCamera())
	{
		if (_scene->_image == nullptr)
		{
			const auto w = width(), h = height();

			_scene->_image = new GLImage{ w, h };
			_rayTracer->setImageSize(w, h);
			_rayTracer->setCamera(camera);
			_rayTracer->renderImage(*_scene->_image);
		}
		_scene->_image->draw(0, 0);
	}
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

inline void
P4::cameraPreview(Camera& camera) {

	auto _scene = _currentScene;
	auto _renderer = _scene->_renderer;
	// A5: janelinha de preview usando tonalizacao de Phong
	_phongProgram.use();

	int oldViewPort[4];
	glGetIntegerv(GL_VIEWPORT, oldViewPort);
	glEnable(GL_SCISSOR_TEST);
	glScissor(width() / 10, height() / 10, width() / 4, height() / 4);
	glViewport(width() / 10, height() / 10, width() / 4, height() / 4);

	_renderer->setCamera(&camera);
	_renderer->setImageSize(width(), height());
	establishLights(); // establishes lights using P4::establishLights();
	_renderer->render();

	// desativa o cut area e retorna os valores de glviweport
	glDisable(GL_SCISSOR_TEST);
	glViewport(oldViewPort[0], oldViewPort[1], oldViewPort[2], oldViewPort[3]);
}

void P4::renderObjectTree(SceneObject* obj)
{
	auto _scene = _currentScene;
	auto _editor = _scene->_editor;

	//std::cout <<"\nentrei0 ";
	for (const auto& o : obj->children())
	{
		//std::cout <<"\nentrei1 ";
		if (!o->visible)
			continue;

		//for (auto component : o->components())
		for (auto it = o->components().begin(); it != o->components().end(); it++)
		{
			auto component = *it;
			if (auto p = dynamic_cast<Primitive*>(component))
			{
				drawPrimitive(*p);
			}

			else if (auto c = dynamic_cast<Camera*>(component))
			{
				if (o == _current)
				{
					drawCamera(*c);
				}
			}

			else if (auto l = dynamic_cast<Light*>(component))
			{
				//if (o == _current)
				drawLight(*l);
			}

			if (o == _current)
			{
				auto t = o->transform();
				_editor->drawAxes(t->position(), mat3f{ t->rotation() });
			}
		}
		renderObjectTree(o);
	}
}

// establishes lights effects inside a scene.
// it needs a scene (and its objects) to have its lights set, and a GLSL program
// witch will receive the scene configuration
void
P4::establishLights(SceneObject* o, int& _NL)
{
	auto program = GLSL::Program::current();

	for (auto child : o->children())
	{
		for (auto c : child->components())
		{
			if (auto l = dynamic_cast<Light*>(c))
			{
				if (l->on) // se a luz estiver ligada
				{
					std::string lw = { "lights[" + std::to_string(_NL) + "]." };

					program->setUniformVec4((lw + "color").c_str(), l->color);
					program->setUniform((lw + "type").c_str(), l->type());

					switch (l->type())
					{
					case(Light::Directional):
						program->setUniformVec3((lw + "direction").c_str(), child->transform()->localRotation() * vec3f { 0, 0, -1 });
						break;

					case(Light::Point):
						program->setUniformVec3((lw + "position").c_str(), child->transform()->localPosition());
						program->setUniform((lw + "falloff").c_str(), l->falloff());
						break;

					case(Light::Spot):
						program->setUniformVec3((lw + "position").c_str(), child->transform()->localPosition());
						program->setUniformVec3((lw + "direction").c_str(), child->transform()->localRotation() * vec3f { 0, 0, -1 });
						program->setUniform((lw + "ghama").c_str(), math::toRadians(l->ghama()));
						program->setUniform((lw + "falloff").c_str(), l->falloff());
						program->setUniform((lw + "fallExponent").c_str(), l->fallExponent());
						break;
					}
					_NL++;
				}
			}
		}
		establishLights(child, _NL);
	}
	return;
}

void
P4::establishLights()
{
	auto _scene = _currentScene;

	int _NL = 0;
	// goes through objects tree, establishing lights
	establishLights(_scene->root(), _NL);
	auto program = GLSL::Program::current();
	// sets number of lights inside the shader
	program->setUniform("NL", _NL);
	// sets natural light
	program->setUniformVec4("ambientLight", _scene->ambientLight);
}

void
P4::render()
{
	auto _scene = _currentScene;
	auto _editor = _scene->_editor;

	if (_scene->_viewMode == ViewMode::Renderer)
	{
		renderScene();
		return;
	}

	// A4: tonalizacao de gouraud utilizado por sceneEditor
	_gouraudProgram.use();

	if (_moveFlags)
	{
		const auto delta = _editor->orbitDistance() * CAMERA_RES;
		auto d = vec3f::null();

		if (_moveFlags.isSet(MoveBits::Forward))
			d.z -= delta;
		if (_moveFlags.isSet(MoveBits::Back))
			d.z += delta;
		if (_moveFlags.isSet(MoveBits::Left))
			d.x -= delta;
		if (_moveFlags.isSet(MoveBits::Right))
			d.x += delta;
		if (_moveFlags.isSet(MoveBits::Up))
			d.y += delta;
		if (_moveFlags.isSet(MoveBits::Down))
			d.y -= delta;
		_editor->pan(d);
	}
	_editor->newFrame();

	auto vp = vpMatrix(_editor->camera());
	_gouraudProgram.setUniformMat4("vpMatrix", vp);

	establishLights();
	renderObjectTree(_scene->root());

	// apresenta preview da camera do objeto atual (se possuir)
	if (auto o = dynamic_cast<SceneObject*>(_current))
	{
		for (auto component : o->components())
		{
			if (auto c = dynamic_cast<Camera*>(component))
				cameraPreview(*c);
		}
	}
}

bool
P4::windowResizeEvent(int width, int height)
{
	auto _scene = _currentScene;

	_scene->_editor->camera()->setAspectRatio(float(width) / float(height));
	_scene->_viewMode = ViewMode::Editor;
	_scene->_image = nullptr;
	return true;
}

// sets the focus of the camera to the _current object
inline void
P4::AltF()
{
	auto _scene = _currentScene;
	auto _editor = _scene->_editor;

	if (auto current = dynamic_cast<SceneObject*>(_current))
	{
		auto eCameraTransform = _editor->camera()->transform();
		auto curPosition = current->transform()->position();

		auto s = current->transform()->localScale();
		auto d = ((s[0] + s[1] + s[2]) / 3) * 5;

		auto ea = eCameraTransform->eulerAngles();

		auto x = d * (float)sin(math::toRadians(ea.y)) * (float)cos(math::toRadians(ea.x));
		auto y = d * (float)-sin(math::toRadians(ea.x));
		auto z = d * (float)cos(math::toRadians(ea.x)) * (float)cos(math::toRadians(ea.y));

		curPosition += vec3f{ x,y,z };
		eCameraTransform->setPosition(curPosition);
	}
}

bool
P4::keyInputEvent(int key, int action, int mods)
{
	auto active = action != GLFW_RELEASE && mods == GLFW_MOD_ALT;

	switch (key)
	{
	case GLFW_KEY_W:
		_moveFlags.enable(MoveBits::Forward, active);
		break;
	case GLFW_KEY_S:
		_moveFlags.enable(MoveBits::Back, active);
		break;
	case GLFW_KEY_A:
		_moveFlags.enable(MoveBits::Left, active);
		break;
	case GLFW_KEY_D:
		_moveFlags.enable(MoveBits::Right, active);
		break;
	case GLFW_KEY_Q:
		_moveFlags.enable(MoveBits::Up, active);
		break;
	case GLFW_KEY_Z:
		_moveFlags.enable(MoveBits::Down, active);
		break;
	case GLFW_KEY_F:
		AltF();
		break;
	}
	return false;
}

bool
P4::scrollEvent(double, double yOffset)
{
	auto _scene = _currentScene;
	auto _editor = _scene->_editor;

	if (ImGui::GetIO().WantCaptureMouse)
		return false;
	_editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
	return true;
}

bool
P4::mouseButtonInputEvent(int button, int actions, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return false;
	(void)mods;

	auto _scene = _currentScene;

	auto active = actions == GLFW_PRESS;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (active)
		{
			cursorPosition(_pivotX, _pivotY);

			const auto ray = makeRay(_pivotX, _pivotY);
			auto minDistance = math::Limits<float>::inf();

			// **Begin picking of temporary scene objects
			// It should be replaced by your picking code
			// for (const auto& o : _scene->root()->children())
			// vai ter que fazer pra toda a hierarquia
			for (const auto& o : _scene->root()->children())
			{
				if (!o->visible)
					continue;

				//float distance;
				Intersection hit;
				
				hit.distance = math::Limits<float>::inf();

				for (auto component : o->components()) 
				{
					if (auto p = dynamic_cast<Primitive*>(component))
						if (p->intersect(ray, hit) && hit.distance < minDistance) // o raio lançado intercepta o primitivo do objeto de cena??
						{
							minDistance = hit.distance;
							_current = o;
						}
				}		
			}
			// **End picking of temporary scene objects
		}
		return true;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
		_dragFlags.enable(DragBits::Rotate, active);
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		_dragFlags.enable(DragBits::Pan, active);
	if (_dragFlags)
		cursorPosition(_pivotX, _pivotY);
	return true;
}

bool
P4::mouseMoveEvent(double xPos, double yPos)
{
	auto _scene = _currentScene;
	auto _editor = _scene->_editor;

	if (!_dragFlags)
		return false;
	_mouseX = (int)xPos;
	_mouseY = (int)yPos;

	const auto dx = (_pivotX - _mouseX);
	const auto dy = (_pivotY - _mouseY);

	_pivotX = _mouseX;
	_pivotY = _mouseY;
	if (dx != 0 || dy != 0)
	{
		if (_dragFlags.isSet(DragBits::Rotate))
		{
			const auto da = -_editor->camera()->viewAngle() * CAMERA_RES;
			isKeyPressed(GLFW_KEY_LEFT_ALT) ?
				_editor->orbit(dy * da, dx * da) :
				_editor->rotateView(dy * da, dx * da);
		}
		if (_dragFlags.isSet(DragBits::Pan))
		{
			const auto dt = -_editor->orbitDistance() * CAMERA_RES;
			_editor->pan(-dt * math::sign(dx), dt * math::sign(dy), 0);
		}
	}
	return true;
}
