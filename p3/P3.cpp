#include "geometry/MeshSweeper.h"
#include "P3.h"
#include <iostream>
#include <iterator>

MeshMap P3::_defaultMeshes;

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
P3::buildDefaultMeshes()
{
  _defaultMeshes["None"] = nullptr;
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
  return new Primitive(mit->second, mit->first);
}

inline void
P3::buildScene()
{
  _current = _scene = new Scene{"Scene 1"};
  _editor = new SceneEditor{*_scene};
  _editor->setDefaultView((float)width() / (float)height());

  // Begin A6: cria objetos e luzes filhos da cena raiz e os posiciona
  {	
	auto root = _scene->root();
	   
	// Objetos (serao criadas 3 boxes flat): 
	// cria box flat e posiciona
	auto o = new SceneObject{ "Flat Box 0", *_scene };
	auto p = makePrimitive(_defaultMeshes.find("Box"));
	p->color = Color::red; // cor Vermelha para a malha da caixa
	o->addComponent(p);
	o->setType(o->ThreeDObject);
	o->setParent(_scene->root());
	root->addChild(o);
	o->transform()->setLocalPosition(vec3f(-5.1, 0, -3.5));
	o->transform()->setLocalScale(vec3f(0.01, 5, 3.5));
	// ###

	// cria box flat e posiciona
	o = new SceneObject{ "Flat Box 1", *_scene };
	p = makePrimitive(_defaultMeshes.find("Box"));
	p->color = Color::green; // Verde
	o->addComponent(p);
	o->setType(o->ThreeDObject);
	o->setParent(_scene->root());
	root->addChild(o);
	o->transform()->setLocalPosition(vec3f(0, 0, -7));
	o->transform()->setLocalScale(vec3f(5, 5, 0.01));
	// ###

	// cria box flat e posiciona
	o = new SceneObject{ "Flat Box 2", *_scene };
	p = makePrimitive(_defaultMeshes.find("Box"));
	p->color = Color::blue; // Azul
	o->addComponent(p);
	o->setType(o->ThreeDObject);
	o->setParent(_scene->root());
	root->addChild(o);
	o->transform()->setLocalPosition(vec3f(5.1, 0, -3.5));
	o->transform()->setLocalScale(vec3f(0.01, 5, 3.5));
	// ###

	// Luzes:
	// cria uma luz Direcional e posiciona
    o = new SceneObject{"Directional Light 0", *_scene};
	auto dLight = new Light;
	dLight->setType(Light::Directional);
	dLight->color = Color::blue;
    o->addComponent(dLight);
	o->setType(SceneObject::Light);
    o->setParent(_scene->root());
	root->addChild(o);
	o->transform()->setLocalEulerAngles(vec3f{ 0,-90,90 });
	o->transform()->setLocalPosition(vec3f(2.9,2.2,-2));
	// ###

	// cria uma luz Pontual e posiciona
	o = new SceneObject{ "Point Light 0", *_scene };
	auto pLight = new Light;
	pLight->setType(Light::Point);
	pLight->color = Color::green;
	o->addComponent(pLight);
	o->setType(SceneObject::Light);
	o->setParent(_scene->root());
	root->addChild(o);
	o->transform()->setLocalPosition(vec3f(3.3, 2, -5.6));
	// ###

	// cria uma luz Spot e posiciona
	o = new SceneObject{ "Spot Light 0", *_scene };
	auto sLight = new Light;
	sLight->setType(Light::Spot);
	sLight->color = Color::red;
	o->addComponent(sLight);
	o->setType(SceneObject::Light);
	o->setParent(_scene->root());
	root->addChild(o);
	o->transform()->setLocalEulerAngles(vec3f{ 0,90,0 });
	o->transform()->setLocalPosition(vec3f(-3.2, 3.1, -6.1));
	// ###

	// Camera:
	// cria uma camera e coloca numa posicao desejada
	o = new SceneObject{ "Main Camera", *_scene };
	auto camera = new Camera;
	o->addComponent(camera);
	o->setType(o->Camera);
	o->setParent(_scene->root());
	root->addChild(o);
	o->transform()->setLocalEulerAngles(vec3f{ -21,0.6,0.6 });
	o->transform()->setLocalPosition(vec3f(-1.3, 4.8, 8.9));
	// ###

	Camera::setCurrent(camera);

	// inicia renderizacao mostrando a camera criada
	_current = o;
  }
  // End initialization of A6 scene
}

void
P3::initialize()
{
  Application::loadShaders(_phongProgram, "shaders/phong.vs", "shaders/phong.fs");
  Application::loadShaders(_gouraudProgram, "shaders/gouraud.vs", "shaders/gouraud.fs");

  Assets::initialize();
  buildDefaultMeshes();
  buildScene();

  _renderer = new GLRenderer{*_scene};
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);

  // A4: tonalizacao de gouraud para sceneEditor
  _gouraudProgram.use();
}

void P3::addSphere(SceneObject* o)
{
	static int newSphereCount = 1;
	static char newSphereName[11];
	snprintf(newSphereName, sizeof(newSphereName), "Sphere %d", newSphereCount++);

	auto newSphere = new SceneObject{ newSphereName, *_scene };
	newSphere->addComponent(makePrimitive(_defaultMeshes.find("Sphere")));
	newSphere->setType(newSphere->ThreeDObject);
	newSphere->setParent(_scene->root());
	o->addChild(newSphere);
}

void P3::addBox(SceneObject* o)
{
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

void P3::addLight(SceneObject* o, int type)
{
	static int pCount = 1, dCount = 1, sCount = 1;
	static char pName[21], dName[21], sName[21];

	static SceneObject* lightObj;
	static Light* sLight, *pLight, *dLight;

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

		// sets parenthood
		lightObj->setParent(_scene->root());
		o->addChild(lightObj);
		break;
	}	
}

void P3::addCamera(SceneObject* o)
{
	static int newCCount = 1;
	static char newCName[10];
	snprintf(newCName, sizeof(newCName), "Camera %d", newCCount++);

	auto newCamera = new SceneObject(newCName, *_scene);
	newCamera->addComponent(new Camera);
	newCamera->setType(newCamera->Camera);
	newCamera->setParent(_scene->root());
	o->addChild(newCamera);
}

void P3::addEmptyObject(SceneObject* o)
{
	static int objectCount = 1;
	static char objectName[11];
	snprintf(objectName, sizeof(objectName), "Object %d", objectCount++);

	auto emptyObject = new SceneObject{ objectName, *_scene };
	emptyObject->setType(emptyObject->EmptyObject);
	emptyObject->setParent(_scene->root());
	o->addChild(emptyObject);
}

void P3::hierarchyTree(SceneObject* o)
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
P3::hierarchyWindow()
{
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
		  object->parent()->deleteChild(object);
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
  const int bufferSize{128};
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
P3::sceneGui()
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
P3::inspectShape(Primitive& primitive)
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
P3::inspectMaterial(Material& material)
{
  ImGui::ColorEdit3("Ambient", material.ambient);
  ImGui::ColorEdit3("Diffuse", material.diffuse);
  ImGui::ColorEdit3("Spot", material.spot);
  ImGui::DragFloat("Shine", &material.shine, 1, 0, 1000.0f);
  ImGui::ColorEdit3("Specular", material.specular);
}

inline void
P3::inspectPrimitive(Primitive& primitive)
{
  //const auto flag = ImGuiTreeNodeFlags_NoTreePushOnOpen;

  //if (ImGui::TreeNodeEx("Shape", flag))
    inspectShape(primitive);
  //if (ImGui::TreeNodeEx("Material", flag))
    inspectMaterial(primitive.material);
}

inline void
P3::inspectLight(Light& light)
{
  static const char* lightTypes[]{"Directional", "Point", "Spot"};
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
P3::inspectCamera(Camera& camera)
{
  static const char* projectionNames[]{"Perspective", "Orthographic"};
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
P3::addComponentButton(SceneObject& object)
{
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
			object.addComponent(new Light);
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
P3::processComponents(ComponentCollection& components)
{
	for(auto it = components.begin(); it != components.end(); it++)
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
				it = components.remove(it);
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
				auto isCurrent = c == Camera::current();

				ImGui::Checkbox("Current", &isCurrent);
				Camera::setCurrent(isCurrent ? c : nullptr);

				/*ImGui::SameLine();

				ImGui::Checkbox("Draw Camera", &c->show);*/

				inspectCamera(*c);
			}			
		}
	}	
}

inline void
P3::sceneObjectGui()
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
P3::objectGui()
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
P3::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

inline void
P3::editorViewGui()
{
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
P3::assetsWindow()
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
P3::editorView()
{
  if (!_showEditorView)
    return;
  ImGui::Begin("Editor View Settings");
  editorViewGui();
  ImGui::End();
}

inline void
P3::fileMenu()
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
P3::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  showStyleSelector("Color Theme##Selector");
  ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
  ImGui::PopItemWidth();
}

inline void
P3::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      fileMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
      if (Camera::current() == 0)
        ImGui::MenuItem("Edit View", nullptr, true, false);
      else
      {
        static const char* viewLabels[]{"Editor", "Renderer"};

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
			  _image = nullptr;
        }
      }
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
    ImGui::EndMainMenuBar();
  }
}

void
P3::gui()
{
  mainMenu();
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
P3::drawPrimitive(Primitive& primitive)
{
	exit(1);
  auto program = GLSL::Program::current();

  auto mesh = primitive.mesh();
  auto m = glMesh(mesh);

  if (nullptr == m)
    return;

  auto t = primitive.transform();
  auto normalMatrix = mat3f{t->worldToLocalMatrix()}.transposed();

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
  std::cout << primitive->sceneObject()->name();

  if (primitive.sceneObject() != _current)
    return;
  drawMesh(m, GL_LINE);
  //_program.setUniformVec4("color", _selectedWireframeColor);
  //_program.setUniform("flatMode", (int)1);
  //drawMesh(m, GL_LINE);
  //_editor->setVectorColor(Color::white);
  //_editor->drawNormals(*mesh, t->localToWorldMatrix(), normalMatrix);
  _editor->setLineColor(Color::yellow);
  // eeeu acho que a linha seguinde desenha a bounding box da malha
  std::cout << primitive->sceneObject()->name();
  _editor->drawBounds(mesh->bounds(), t->localToWorldMatrix());
}

inline void
P3::drawLight(Light& light)
{
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

		auto spotEdge = light.transform()->transform(vec3f(0,0,0));

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
P3::drawCamera(Camera& camera)
{
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
	if (auto camera = Camera::current())
	{
		if (_image == nullptr)
		{
			const auto w = width(), h = height();

			_image = new GLImage{ w, h };
			_rayTracer->setImageSize(w, h);
			_rayTracer->setCamera(camera);
			_rayTracer->renderImage(*_image);
		}
		_image->draw(0, 0);
	}
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

inline void
P3::cameraPreview(Camera& camera) {
	// A5: janelinha de preview usando tonalizacao de Phong
	_phongProgram.use();

	int oldViewPort[4];
	glGetIntegerv(GL_VIEWPORT, oldViewPort);	
	glEnable(GL_SCISSOR_TEST);
	glScissor(width() / 10, height() / 10, width() / 4, height() / 4);
	glViewport(width() / 10, height() / 10, width() / 4, height() / 4);
	
	_renderer->setCamera(&camera);
	_renderer->setImageSize(width(), height());
	establishLights(); // establishes lights using P3::establishLights();
	_renderer->render();

	// desativa o cut area e retorna os valores de glviweport
	glDisable(GL_SCISSOR_TEST);
	glViewport(oldViewPort[0], oldViewPort[1], oldViewPort[2], oldViewPort[3]);	
}

void P3::renderObjectTree(SceneObject* obj)
{
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
				_editor->drawAxes(t->position(), mat3f{t->rotation() });
			}
		}
		renderObjectTree(o);
	}
}

// establishes lights effects inside a scene.
// it needs a scene (and its objects) to have its lights set, and a GLSL program
// witch will receive the scene configuration
void
P3::establishLights(SceneObject* o, int& _NL)
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
						program->setUniformVec3((lw + "direction").c_str(), child->transform()->localRotation()* vec3f { 0, 0, -1 });
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
P3::establishLights()
{
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
P3::render()
{
  if (_viewMode == ViewMode::Renderer)
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
  if(auto o = dynamic_cast<SceneObject*>(_current))
  {
	  for (auto component : o->components())
	  {
		 if (auto c = dynamic_cast<Camera*>(component))
			cameraPreview(*c);
	  }
  }
}

bool
P3::windowResizeEvent(int width, int height)
{
  _editor->camera()->setAspectRatio(float(width) / float(height));
  _viewMode = ViewMode::Editor;
  _image = nullptr; 
  return true;
}

// sets the focus of the camera to the _current object
inline void
P3::AltF()
{
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
P3::keyInputEvent(int key, int action, int mods)
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
P3::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
P3::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  (void)mods;

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

			  auto component = o->components();
			  float distance;

			  if (auto p = dynamic_cast<Primitive*>(component))
				  if (p->intersect(ray, distance) && distance < minDistance) // o raio lançado intercepta o primitivo do objeto de cena??
				  {
					  minDistance = distance;
					  _current = o;
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
P3::mouseMoveEvent(double xPos, double yPos)
{
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
