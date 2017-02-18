#include <fstream>
#include <sstream>
#include <iostream> // std::cout, std::endl
#include "glitter.hpp"
#include "ShaderProgram.hpp"

static GLuint compileShader(const GLenum shader_type, const GLchar* shader_src) {
  const GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &shader_src, nullptr);
  glCompileShader(shader);
  GLint shader_was_compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_was_compiled);
  if (shader_was_compiled != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(shader, 1024, &log_length, message);
    std::cout << message << std::endl;
    // exit
  }
  return shader;
}

static GLuint linkProgram(const GLchar* vertex_shader_src, const GLchar* fragment_shader_src) {
  const GLuint program = glCreateProgram();
  glAttachShader(program, compileShader(GL_VERTEX_SHADER, vertex_shader_src));
  glAttachShader(program, compileShader(GL_FRAGMENT_SHADER, fragment_shader_src));
  glLinkProgram(program);
  GLint program_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
  if (program_linked != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetProgramInfoLog(program, 1024, &log_length, message);
    std::cout << message << std::endl;
    // exit
  }
  return program;
}

// reads a file into a string
static std::string fname_to_string(const std::string& fname) {
  std::ifstream file(fname, std::ios::in | std::ios::binary);
  if (file.is_open()) {
    std::ostringstream stream;
    stream << file.rdbuf();
    file.close();
    return stream.str();
  }
  // todo: use optional, or exit
  std::cout << "failed to open file named: " << fname << std::endl;
  return std::string();
}

ShaderProgram::ShaderProgram(const std::string& vertex_shader_fname, const std::string& fragment_shader_fname) {
  m_program = linkProgram(fname_to_string(vertex_shader_fname).c_str(),
    fname_to_string(fragment_shader_fname).c_str());
}

GLuint ShaderProgram::getProgram() const
{
  return m_program;
}