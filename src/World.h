#ifndef _WORLD_H_
#define _WORLD_H_

#include <btBulletDynamicsCommon.h>
#include <glm/vec3.hpp>
#include <memory>
#include <random>
#include <vector>

/*
 * World containing boxes with physical simulation.
 */
class World {
public:
  World();
  ~World();

  void initPhysics();

  struct Box {
    std::unique_ptr<btMotionState> pose;
    std::unique_ptr<btRigidBody> body;
    glm::vec3 color;
  };

  const std::vector<Box>& getBoxes() const { return _boxes; }

  Box& addBox(const btVector3& pos, float yaw, float pitch, float roll,
              const glm::vec3& color);
  Box& addRandomBox(const glm::vec3& pos);

  void update(float dt, float timeStep);

  // persistence
  void load();
  void save();

private:
  // boxes
  std::vector<Box> _boxes;
  std::unique_ptr<btCollisionShape> _boxShape;

  // ground
  std::unique_ptr<btCollisionShape> _groundShape;
  std::unique_ptr<btMotionState> _groundMotionState;
  std::unique_ptr<btRigidBody> _groundRigidBody;

  // dynamics world
  std::unique_ptr<btBroadphaseInterface> _broadphase;
  std::unique_ptr<btDefaultCollisionConfiguration> _collisionConfiguration;
  std::unique_ptr<btCollisionDispatcher> _dispatcher;
  std::unique_ptr<btSequentialImpulseConstraintSolver> _solver;
  std::unique_ptr<btDiscreteDynamicsWorld> _dynamicsWorld;

  // pseudo-random number gen
  std::mt19937 _mt{std::mt19937{}()};
  std::uniform_real_distribution<float> _rand{0, 1};
};

#endif // _WORLD_H_
