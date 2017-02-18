#include <string>
class ShaderProgram {
  GLuint m_program;
public:
  ShaderProgram(const std::string& vertex_shader_fname, const std::string& fragment_shader_fname);
  GLuint getProgram() const;
};