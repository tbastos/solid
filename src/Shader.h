#ifndef _SHADER_H_
#define _SHADER_H_

#include "glad.h"
#include <glm/glm.hpp>
#include <string>

/*
 * Abstraction for GLSL programs.
 */
class Shader {
public:
  Shader();
  ~Shader();

  // Kinds of shaders that make up a program.
  enum ShaderType {
    Vertex,
    Fragment,
    Geometry,
    TessControl,
    TessEvaluation,
    Count
  };

  // Replaces source code in a shader object given a string or filename.
  void loadString(ShaderType type, const std::string& sourceCode);
  bool loadFile(ShaderType type, const std::string& fileName);

  /*
   * Given a file name prefix (without .extension), this method loads the set
   * of shader files that match fileNamePrefix concatenated with one of the
   * canonical filename extensions: .vert, .frag, .geom, .tesc or .tese.
   * Returns the number of files loaded.
   */
  int load(const std::string& filePrefix);

  // Compiles shaders and links the program.
  // If there are any problems, logs to console and returns false.
  bool link();

  // Calls link() and activates the shader program.
  bool use();

  // Set uniform values by name. The shader program must be in use.
  void setUniform(const char* name, float v);
  void setUniform(const char* name, const glm::vec2& v);
  void setUniform(const char* name, const glm::vec3& v);
  void setUniform(const char* name, const glm::vec4& v);
  void setUniform(const char* name, const glm::mat4& v);

  // Prints to stdout a list with all active uniform variables in the program
  void printActiveUniforms() const;

  // Detaches and deletes all shader objects (not the program).
  void clear();

private:
  GLint getLocation(const char* uniform);
  bool checkShader(int i);
  bool checkProgram();

private:
  GLuint _program;        // program object
  GLuint _shaders[Count]; // shader objects; TODO remove these in production
  int _dirty;             // non-zero when a shader contains modifications
};

#endif // _SHADER_H_
