#ifndef __P3_h
#define __P3_h

#include "Assets.h"
#include "GLRenderer.h"
#include "Light.h"
#include "Primitive.h"
#include "SceneEditor.h"
#include "core/Flags.h"
#include "RayTracer.h"
#include "graphics/Application.h"
#include "graphics/GLImage.h"
#include <vector>

using namespace cg;

class P3: public GLWindow
{
public:
  P3(int width, int height):
    GLWindow{"cg2019 - P3", width, height}
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  /// Update the GUI.
  void gui() override;

  /// Render the scene.
  void render() override;

private:
  enum ViewMode
  {
    Editor = 0,
    Renderer = 1
  };

  enum class MoveBits
  {
    Left = 1,
    Right = 2,
    Forward = 4,
    Back = 8,
    Up = 16,
    Down = 32
  };

  enum class DragBits
  {
    Rotate = 1,
    Pan = 2
  };

  GLSL::Program _program;
  GLSL::Program _phongProgram { "Phong" };
  GLSL::Program _gouraudProgram { "Gouraud" };
  Reference<Scene> _scene;
  Reference<SceneEditor> _editor;
  Reference<GLRenderer> _renderer;

  SceneNode* _current{};
  Color _selectedWireframeColor{255, 102, 0};
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};
  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;
  int _NL; // number of lights
  bool _showAssets{true};
  bool _showEditorView{true};
  ViewMode _viewMode{ViewMode::Editor};
  Reference<RayTracer> _rayTracer;
  Reference<GLImage> _image;

  static MeshMap _defaultMeshes;

  void buildScene();
  void renderScene();

  void establishLights(SceneObject* o, int&);
  void establishLights();

  void mainMenu();
  void fileMenu();
  void showOptions();

  void hierarchyWindow();
  void inspectorWindow();
  void assetsWindow();
  void editorView();
  void sceneGui();
  void sceneObjectGui();
  void objectGui();  
  void editorViewGui();
  void processComponents(ComponentCollection&);
  void inspectPrimitive(Primitive&);
  void inspectShape(Primitive&);
  void inspectMaterial(Material&);
  void inspectLight(Light&);
  void inspectCamera(Camera&);
  void addComponentButton(SceneObject&);
  void renderObjectTree(SceneObject*);
  void hierarchyTree(SceneObject*);
  void addSphere(SceneObject*);
  void addBox(SceneObject*);
  void addCamera(SceneObject*);
  void addEmptyObject(SceneObject* o);
  void addDirectionalLight(SceneObject*);
  void addLight(SceneObject* o, int type);
  void cameraPreview(Camera& camera);
  void AltF();

  void drawPrimitive(Primitive&);
  void drawLight(Light&);
  void drawCamera(Camera&);

  bool windowResizeEvent(int, int) override;
  bool keyInputEvent(int, int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

  Ray makeRay(int, int) const;

  static void buildDefaultMeshes();

}; // P3

#endif // __P3_h
