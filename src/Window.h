#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

// Window sends all input events to this interface.
class InputHandler {
public:
  // Camera controls. All values are in the range [-1, 1].
  virtual void inputMovement(float ahead, float right) = 0;
  virtual void inputRotation(float yaw, float pitch) = 0;

  // Actions
  virtual void shoot() = 0;
  virtual void resetPosition() = 0;
  virtual void toggleWireframe() = 0;
};

/*
 * A GUI window with an OpenGL canvas.
 */
class Window {
public:
  Window();
  ~Window();

  // Returns true if the application should quit.
  bool handleEvents(InputHandler& handler);

  inline int getWidth() const { return _width; }
  inline int getHeight() const { return _height; }

  void swapBuffers();

private:
  void die(const char* what);

private:
  int _width, _height;
  SDL_Window* _window;
  SDL_GLContext _context;
  std::shared_ptr<spdlog::logger> _logger;
};

#endif // _WINDOW_H_
