#include "Graphics.h"
#include "Window.h"
#include "World.h"

#include <chrono>
#include <cstdlib>

using namespace std::chrono_literals;

// use a fixed time step of 66.66Hz = 15 milliseconds
constexpr std::chrono::duration<double> timeStep(15ms);

int main(int, char* []) {
  World world;
  Window window;
  Graphics graphics(world);

  world.initPhysics();
  world.load();

  // track rendering time and update state at a fixed timestep
  using clock = std::chrono::high_resolution_clock;
  auto timeCurrent = clock::now();
  std::chrono::duration<double> timeAccum(0s);

  // game loop
  while (true) {
    if (window.handleEvents(graphics))
      break;

    auto now = clock::now();
    std::chrono::duration<double> timeDelta = now - timeCurrent;
    if (timeDelta > 0.25s)
      timeDelta = 0.25s;
    timeCurrent = now;
    timeAccum += timeDelta;

    // update game state
    world.update(timeDelta.count(), timeStep.count());
    /* while (timeAccum >= timeStep) { */
    /*   timeAccum -= timeStep; */
    /*   update(timeStep.count()); */
    /* } */
    /* // calculate alpha to interpolate between prev_state and curr_state */
    /* double time_alpha = time_accum.count() / time_step.count(); */
    /* auto render_state = interpolate(prev_state, curr_state, alpha); */
    /* render(render_state); */

    graphics.update(timeDelta.count());
    graphics.render(window);
    window.swapBuffers();
  }

  world.save();

  return 0;
}
