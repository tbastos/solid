#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "Shader.h"
#include "Window.h"
#include "World.h"

/*
 * OpenGL scene manager / renderer.
 */
class Graphics : public InputHandler {
public:
  Graphics(World& world);
  ~Graphics();

  void update(float dt);
  void render(Window& window);

  // InputHandler
  void inputMovement(float ahead, float right) override;
  void inputRotation(float yaw, float pitch) override;
  void shoot() override;
  void resetPosition() override;
  void toggleWireframe() override;

private:
  World& _world;

  // controller
  float _inAhead = 0.0f, _inRight = 0.0f;
  float _inYaw = 0.0f, _inPitch = 0.0f;
  float _shootCoolDown = 0.0f;

  // camera
  float _yaw, _pitch;
  glm::vec3 _pos, _front;
  glm::mat4 _view;

  // OpenGL state
  Shader _groundShader;
  GLuint _groundVAO = 0, _groundVBO = 0;
  Shader _boxShader;
  GLuint _boxVAO = 0, _boxVBO = 0;
  bool _wireframe = false;
};

#endif // _GRAPHICS_H_
