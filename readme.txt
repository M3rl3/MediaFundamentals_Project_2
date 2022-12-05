
Build Instructions:
- Solutuion has one project which is automatically the default project.
- Built in Visual Studio 17 (2022). Retarget solution if necessary.

Gameplay and related:
- The setting of this game portrays a long highway dimly lit on each side.
- A bunch of vehicles are moving about, going through their business.

Controls:
- The user can control one of the cars (ie. a tesla cybertruck) by pressing the 'F' key.
  This fixes the camera just behind the car as in a third-person perspective.
  
- Car Controls  
  User can press the 'F' key to assume control of the car.
  The car can be moved by using the W and S keys to change position along the axis of the street/highway.
  
- User can iterate through objects/meshes in the scene by pressing 'O' key ie. Object Mode. 
  This also locks the camera target on the object being pointed to.
  NUM1 returns the camera focus to the origin (0,0,0) of the scene.
  NUM2 iterates through the meshes in the scene and focuses the camera on the next object/mesh.
  NUM3 focuses the camera on the previous object.
  The name of the object in the hierarchy is outputed to the titlebar of the window.
  
- All mesh positioning and translational data is extracted from an external file to prevent re-compilation.

- The free camera (ie. the listener) can be accessed at any point by pressing the 'C' key.

- The 'X' key can be used to turn all models in the scene into wireframe mode.

- A combination of W,A,S,D,Q,E keys can be used to move the models or the camera when in their respective modes.

- All camera, object positions and related information is outputed to the titlebar for debugging.