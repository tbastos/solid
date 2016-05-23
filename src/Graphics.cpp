#include "Graphics.h"
#include "Window.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// unit ground plane
const GLfloat GROUND[] = { // x, y, z, ...
    -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 0.5f,  0.5f,  0.0f, -0.5f,
    -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f};

// unit cube with normals
const GLfloat CUBE[] = { // x, y, z, nx, ny, nz, ...
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
    0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
    0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
    0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
    0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
    0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
    -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
    -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
    1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
    0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
    1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
    0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
    0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
    0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
    0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
    0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

Graphics::Graphics(World& world) : _world(world) {
  resetPosition();

  // initialize OpenGL
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.3, 0.3, 0.3, 1);

  // prepare ground
  glGenVertexArrays(1, &_groundVAO);
  glGenBuffers(1, &_groundVBO);

  glBindVertexArray(_groundVAO);

  glBindBuffer(GL_ARRAY_BUFFER, _groundVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GROUND), GROUND, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                        (GLvoid*)0);

  glBindVertexArray(0);

  _groundShader.loadString(Shader::Vertex, R"(
#version 330 core

uniform mat4 viewProjection;
layout (location = 0) in vec3 position;
out vec3 FragPos;
const float SIZE = 500;

void main() {
  FragPos = vec3(SIZE * position);
  gl_Position = viewProjection * vec4(FragPos, 1.0);
}
)");
  _groundShader.loadString(Shader::Fragment, R"(
#version 330 core

in vec3 FragPos;
out vec4 color;
float GRID_SIZE = 1;
float LINE_WIDTH = 0.1;

void main() {
  float s =
    smoothstep(GRID_SIZE-LINE_WIDTH, GRID_SIZE, abs(mod(FragPos.x, 2*GRID_SIZE) - GRID_SIZE)) +
    smoothstep(GRID_SIZE-LINE_WIDTH, GRID_SIZE, abs(mod(FragPos.z, 2*GRID_SIZE) - GRID_SIZE));
  float alpha = 1 - clamp((length(FragPos) - 3) / 27, 0, 1);
  color = vec4(vec3(0), s * alpha);
}
)");

  // prepare box
  glGenVertexArrays(1, &_boxVAO);
  glGenBuffers(1, &_boxVBO);

  glBindVertexArray(_boxVAO);

  glBindBuffer(GL_ARRAY_BUFFER, _boxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE), CUBE, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (GLvoid*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (GLvoid*)(3 * sizeof(GLfloat)));

  glBindVertexArray(0);

  _boxShader.loadString(Shader::Vertex, R"(
#version 330 core

uniform mat4 model;
uniform mat4 viewProjection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;

void main() {
  FragPos = vec3(model * vec4(position, 1.0));
  Normal = mat3(model) * normal;
  gl_Position = viewProjection * vec4(FragPos, 1.0);
}
)");
  _boxShader.loadString(Shader::Fragment, R"(
#version 330 core

uniform vec3 viewPos;
uniform vec3 objectColor;

in vec3 Normal;
in vec3 FragPos;
out vec4 color;

const vec3 lightPos = vec3(-50, 100, -50);
const vec3 lightColor = vec3(1, 1, 1);
const vec3 ambient = 0.1 * lightColor;
const float specularStrength = 0.5f;

void main() {
  vec3 normal = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, normal);

  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;

  color = vec4((ambient + diffuse + specular) * objectColor, 1.0f);
}
)");
}

Graphics::~Graphics() {
  glDeleteVertexArrays(1, &_groundVAO);
  glDeleteBuffers(1, &_groundVBO);
  glDeleteVertexArrays(1, &_boxVAO);
  glDeleteBuffers(1, &_boxVBO);
}

void Graphics::update(float dt) {
  const float SPEED_ROTATION = 60.0f;
  const float SPEED_MOVEMENT = 10.0f;

  _shootCoolDown -= dt;

  _yaw += _inYaw * dt * SPEED_ROTATION;
  _pitch += _inPitch * dt * SPEED_ROTATION;
  if (_pitch > 89.0f)
    _pitch = 89.0f;
  else if (_pitch < -89.0f)
    _pitch = -89.0f;

  _front = glm::vec3(cos(glm::radians(_pitch)) * cos(glm::radians(_yaw)),
                     sin(glm::radians(_pitch)),
                     cos(glm::radians(_pitch)) * sin(glm::radians(_yaw)));
  _front = glm::normalize(_front);

  glm::vec3 right = glm::cross(_front, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::vec3 up = glm::cross(right, _front);

  _view = glm::lookAt(_pos, _pos + _front, up);

  _pos += _front * (_inAhead * dt * SPEED_MOVEMENT);
  _pos += right * (_inRight * dt * SPEED_MOVEMENT);
}

void Graphics::render(Window& window) {
  float aspectRatio = window.getWidth() / window.getHeight();
  glViewport(0, 0, window.getWidth(), window.getHeight());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 viewProjections =
      glm::perspective(glm::radians(45.0f), aspectRatio, 1.0f, 1000.0f) * _view;

  // draw ground
  _groundShader.use();
  _groundShader.setUniform("viewProjection", viewProjections);
  glEnable(GL_BLEND);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(_groundVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  glDisable(GL_BLEND);

  // draw boxes
  _boxShader.use();
  _boxShader.setUniform("viewPos", _pos);
  _boxShader.setUniform("viewProjection", viewProjections);
  glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
  for (auto& box : _world.getBoxes()) {
    glm::mat4 model;
    btTransform transform;
    box.pose->getWorldTransform(transform);
    transform.getOpenGLMatrix(glm::value_ptr(model));

    _boxShader.setUniform("model", model);
    _boxShader.setUniform("objectColor", box.color);
    glBindVertexArray(_boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
  }
}

void Graphics::inputMovement(float ahead, float right) {
  _inAhead = ahead;
  _inRight = right;
}

void Graphics::inputRotation(float yaw, float pitch) {
  _inYaw = yaw;
  _inPitch = pitch;
}

void Graphics::shoot() {
  if (_shootCoolDown > 0)
    return;

  World::Box& box = _world.addRandomBox(_pos + _front * 10.0f);

  glm::vec3 imp(_front * 60.0f);
  box.body->applyCentralImpulse(btVector3(imp.x, imp.y, imp.z));

  _shootCoolDown = 0.2f; // 200ms
}

void Graphics::resetPosition() {
  _yaw = -45.0f;
  _pitch = -30.0f;
  _pos = glm::vec3(-50.0f, 50.0f, 50.0f);
}

void Graphics::toggleWireframe() {
  _wireframe = !_wireframe;
}
