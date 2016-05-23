#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <fstream>

constexpr const char* SHADER_NAME[] = {
    "Vertex",
    "Fragment",
    "Geometry",
    "Tessellation Control",
    "Tessellation Evaluation",
};

constexpr const char* SHADER_EXT[] = {
    ".vert", ".frag", ".geom", ".tesc", ".tese",
};

constexpr GLenum SHADER_GLENUM[] = {
    GL_VERTEX_SHADER,         // vs
    GL_FRAGMENT_SHADER,       // fs
    GL_GEOMETRY_SHADER,       // gs
    GL_TESS_CONTROL_SHADER,   // tc
    GL_TESS_EVALUATION_SHADER // te
};

Shader::Shader() : _program(0), _shaders{0}, _dirty(0) {
  // empty
}

Shader::~Shader() {
  clear();
  if (_program) {
    glDeleteProgram(_program);
    _program = 0;
  }
}

void Shader::loadString(ShaderType type, const std::string& source) {
  if (!_program)
    _program = glCreateProgram();

  if (!_shaders[type]) {
    _shaders[type] = glCreateShader(SHADER_GLENUM[type]);
    glAttachShader(_program, _shaders[type]);
  }

  // set the source code for the shader
  const GLchar* cstr = source.c_str();
  const GLint len = source.size();
  glShaderSource(_shaders[type], 1, &cstr, &len);

  _dirty |= (1 << type);
}

bool Shader::loadFile(ShaderType type, const std::string& fileName) {
  std::ifstream file(fileName, std::ios::binary | std::ios::ate);

  if (!file.is_open())
    return false;

  std::string src(file.tellg(), 0);
  file.seekg(0, std::ios::beg);
  file.read(&src[0], src.size());
  file.close();

  loadString(type, src);

  return true;
}

int Shader::load(const std::string& filePrefix) {
  int numLoaded = 0;
  for (int i = 0; i < Count; ++i) {
    if (loadFile(static_cast<ShaderType>(i), filePrefix + SHADER_EXT[i]))
      ++numLoaded;
  }
  return numLoaded;
}

bool Shader::link() {
  if (!_program)
    return false; // shaders must be loaded first!

  if (_dirty) {
    for (int i = 0; i < Count; ++i) {
      if (_dirty & (1 << i)) {
        glCompileShader(_shaders[i]);
        if (!checkShader(i))
          return false;
      }
    }
    _dirty = 0;
  } else {
    GLint linked;
    glGetProgramiv(_program, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE)
      return true; // program already up-to-date and linked
  }

  glLinkProgram(_program);
  return checkProgram();
}

bool Shader::use() {
  if (!link())
    return false;
  glUseProgram(_program);
  return true;
}

inline auto getLogger() {
  static std::shared_ptr<spdlog::logger> s_logger;
  if (!s_logger)
    s_logger = spdlog::stdout_logger_mt("shader", true /*use color*/);
  return s_logger;
}

GLint Shader::getLocation(const char* uniform) {
  GLint location = glGetUniformLocation(_program, uniform);
  if (location < 0)
    getLogger()->error() << "No such Uniform: " << uniform;
  return location;
}

void Shader::setUniform(const char* name, float v) {
  glUniform1f(getLocation(name), v);
}

void Shader::setUniform(const char* name, const glm::vec2& v) {
  glUniform2fv(getLocation(name), 1, glm::value_ptr(v));
}

void Shader::setUniform(const char* name, const glm::vec3& v) {
  glUniform3fv(getLocation(name), 1, glm::value_ptr(v));
}

void Shader::setUniform(const char* name, const glm::vec4& v) {
  glUniform4fv(getLocation(name), 1, glm::value_ptr(v));
}

void Shader::setUniform(const char* name, const glm::mat4& v) {
  glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::clear() {
  for (auto& handle : _shaders) {
    if (handle != 0) {
      glDetachShader(_program, handle);
      glDeleteShader(handle);
      handle = 0;
    }
  }
  _dirty = 0;
}

void Shader::printActiveUniforms() const {
  auto logger = getLogger();

  GLint numUniforms;
  glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numUniforms);
  logger->debug() << "Active Uniforms: " << numUniforms;

  GLchar buffer[256];
  for (GLint i = 0; i < numUniforms; ++i) {
    GLsizei len;
    GLint size;
    GLenum type;
    glGetActiveUniform(_program, i, sizeof(buffer), &len, &size, &type, buffer);
    logger->debug() << "  " << i + 1 << ") " << buffer;
  }
}

bool Shader::checkShader(int i) {
  GLint compiled, infoLogLen;
  glGetShaderiv(_shaders[i], GL_COMPILE_STATUS, &compiled);
  glGetShaderiv(_shaders[i], GL_INFO_LOG_LENGTH, &infoLogLen);

  if (infoLogLen > 0) {
    std::string infoLog(infoLogLen, 0);
    glGetShaderInfoLog(_shaders[i], infoLogLen, &infoLogLen, &infoLog[0]);
    if (static_cast<size_t>(infoLogLen) != infoLog.length())
      getLogger()->debug() << "Mismatch! infoLogLen=" << infoLogLen
                           << ", infoLog.length()=" << infoLog.length();
    getLogger()->warn() << SHADER_NAME[i] << " Shader Log:\n" << infoLog;
  }

  if (compiled == GL_TRUE)
    return true;

  getLogger()->error() << SHADER_NAME[i] << " shader failed to compile!";
  getLogger()->flush();
  assert(false);

  return false;
}

bool Shader::checkProgram() {
  GLint linked, infoLogLen;
  glGetProgramiv(_program, GL_LINK_STATUS, &linked);
  glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLen);

  if (infoLogLen > 0) {
    std::string infoLog(infoLogLen, 0);
    glGetProgramInfoLog(_program, infoLogLen, &infoLogLen, &infoLog[0]);
    assert(static_cast<size_t>(infoLogLen) == infoLog.length());
    getLogger()->warn() << "Shader Program Log:\n" << infoLog;
  }

  if (linked == GL_TRUE)
    return true;

  getLogger()->error() << "Shader program failed to link!";

  return false;
}
