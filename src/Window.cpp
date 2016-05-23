#include "Window.h"
#include "glad.h"

Window::Window() {
  // create a multithreaded console logger with color support
  _logger = spdlog::stdout_logger_mt("window", true /*use color*/);
  _logger->info("Starting GUI");

  // initialize SDL2
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    die("SDL_Init");

  // use OpenGL 3.2+ core profile
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

  // request a 32-bit RGBA framebuffer (sRGB capable)
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

  // request anti-aliasing
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  // enable v-sync
  SDL_GL_SetSwapInterval(1);

  // create window
  _window =
      SDL_CreateWindow("Solid", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  if (!_window)
    die("SDL_CreateWindow");

  SDL_GL_GetDrawableSize(_window, &_width, &_height);

  // create OpenGL context
  _context = SDL_GL_CreateContext(_window);
  if (!_context)
    die("SDL_GL_CreateContext");

  // load OpenGL functions
  gladLoadGL();

  // print OpenGL context info
  int buf_sz, buf_aa, buf_srgb;
  SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &buf_sz);
  SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &buf_aa);
  SDL_GL_GetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &buf_srgb);
  _logger->info() << "OpenGL Info:\n"
                  << "OpenGL " << glGetString(GL_VERSION) << '\n'
                  << buf_sz << "-bit RGBA buffer (sRGB=" << buf_srgb << ")\n"
                  << buf_aa << "-sample anti-aliasing\n";
}

Window::~Window() {
  SDL_GL_DeleteContext(_context);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

bool Window::handleEvents(InputHandler& handler) {
  SDL_Event ev;
  while (SDL_PollEvent(&ev)) {
    switch (ev.type) {
    case SDL_QUIT:
      return true;

    case SDL_WINDOWEVENT:
      if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        SDL_GL_GetDrawableSize(_window, &_width, &_height);
      }
      break;

    case SDL_KEYDOWN:
      switch (ev.key.keysym.sym) {
      case SDLK_F1:
        handler.toggleWireframe();
        break;
      case SDLK_F2:
        handler.resetPosition();
        break;
      case SDLK_SPACE:
        handler.shoot();
        break;
      }
      break;

    case SDL_KEYUP:
      break;

    case SDL_MOUSEMOTION:
      break;
    }
  }

  const Uint8* kbs = SDL_GetKeyboardState(NULL);
  float ahead = 0.0f, right = 0.0f;
  if (kbs[SDL_SCANCODE_W])
    ahead += 1.0f;
  if (kbs[SDL_SCANCODE_S])
    ahead -= 1.0f;
  if (kbs[SDL_SCANCODE_D])
    right += 1.0f;
  if (kbs[SDL_SCANCODE_A])
    right -= 1.0f;
  handler.inputMovement(ahead, right);

  float yaw = 0.0f, pitch = 0.0f;
  if (kbs[SDL_SCANCODE_RIGHT])
    yaw += 1.0f;
  if (kbs[SDL_SCANCODE_LEFT])
    yaw -= 1.0f;
  if (kbs[SDL_SCANCODE_UP])
    pitch += 1.0f;
  if (kbs[SDL_SCANCODE_DOWN])
    pitch -= 1.0f;
  handler.inputRotation(yaw, pitch);

  return false;
}

void Window::swapBuffers() {
  SDL_GL_SwapWindow(_window);
}

void Window::die(const char* what) {
  _logger->critical() << what << " failed: " << SDL_GetError();
  _logger->flush();
  std::exit(-1);
}
