#include "World.h"
#include "BoxTable.h"
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>

namespace sql = sqlpp::sqlite3;

World::World() {
  // empty
}

World::~World() {
  // empty
}

void World::initPhysics() {
  // dynamics world
  _broadphase.reset(new btDbvtBroadphase());
  _collisionConfiguration.reset(new btDefaultCollisionConfiguration());
  _dispatcher.reset(new btCollisionDispatcher(_collisionConfiguration.get()));
  _solver.reset(new btSequentialImpulseConstraintSolver());
  _dynamicsWorld.reset(new btDiscreteDynamicsWorld(
      _dispatcher.get(), _broadphase.get(), _solver.get(),
      _collisionConfiguration.get()));
  _dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

  // ground
  _groundShape.reset(new btStaticPlaneShape(btVector3(0, 1, 0), 0));
  _groundMotionState.reset(new btDefaultMotionState(
      btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0))));
  _groundRigidBody.reset(
      new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
          0, _groundMotionState.get(), _groundShape.get())));
  _groundRigidBody->setFriction(1.5f);
  _dynamicsWorld->addRigidBody(_groundRigidBody.get());

  // box
  _boxShape.reset(new btBoxShape(btVector3(0.5, 0.5, 0.5)));
}

World::Box& World::addBox(const btVector3& pos, float yaw, float pitch,
                          float roll, const glm::vec3& color) {
  // physics
  btTransform transform(btQuaternion(yaw, pitch, roll), pos);
  std::unique_ptr<btMotionState> pose(new btDefaultMotionState(transform));
  std::unique_ptr<btRigidBody> body(
      new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
          1, pose.get(), _boxShape.get())));
  body->setFriction(1.1f);
  _dynamicsWorld->addRigidBody(body.get());

  _boxes.emplace_back(Box{std::move(pose), std::move(body), color});
  return _boxes.back();
}

World::Box& World::addRandomBox(const glm::vec3& pos) {
  glm::vec3 color(_rand(_mt), _rand(_mt), _rand(_mt));
  return addBox(btVector3(pos.x, pos.y, pos.z), _rand(_mt) * btRadians(90.0f),
                0, 0, color);
}

void World::update(float dt, float timeStep) {
  _dynamicsWorld->stepSimulation(dt, 5, timeStep);
}

sql::connection_config getDbConfig() {
  sql::connection_config config;
  config.path_to_database = "box.db";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  // config.debug = true;
  return config;
}

void World::load() {
  sql::connection db(getDbConfig());
  db.execute(R"(
    CREATE TABLE IF NOT EXISTS `box` (
      `x`     REAL NOT NULL DEFAULT 0,
      `y`     REAL NOT NULL DEFAULT 0,
      `z`     REAL NOT NULL DEFAULT 0,
      `yaw`   REAL NOT NULL DEFAULT 0,
      `pitch` REAL NOT NULL DEFAULT 0,
      `roll`  REAL NOT NULL DEFAULT 0,
      `red`   REAL NOT NULL DEFAULT 1,
      `green` REAL NOT NULL DEFAULT 0,
      `blue`  REAL NOT NULL DEFAULT 0
    );)");
  box_db::Box tbl;
  for (const auto& row : db(select(all_of(tbl)).from(tbl).unconditionally())) {
    addBox(btVector3(row.x, row.y, row.z), row.yaw, row.pitch, row.roll,
           glm::vec3(row.red, row.green, row.blue));
  }
}

void World::save() {
  sql::connection db(getDbConfig());
  box_db::Box tbl;

  // clear the table
  db(remove_from(tbl).unconditionally());

  // insert all boxes
  for (const auto& box : _boxes) {
    btTransform trans;
    box.pose->getWorldTransform(trans);
    auto& p = trans.getOrigin();
    float yaw, pitch, roll;
    trans.getBasis().getEulerYPR(yaw, pitch, roll);
    auto& c = box.color;
    db(insert_into(tbl).set(
        // position
        tbl.x = p.x(), tbl.y = p.y(), tbl.z = p.z(),
        // orientation
        tbl.yaw = yaw, tbl.pitch = pitch, tbl.roll = roll,
        // color
        tbl.red = c.r, tbl.green = c.g, tbl.blue = c.b));
  }
}
