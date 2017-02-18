#include <string>
#include <unordered_map>
class ShaderProgram {
public:
  ShaderProgram(const std::string& vertex_shader_fname, const std::string& fragment_shader_fname);
  GLuint getProgram() const;
  GLint getAttribute(const std::string& name);
private:
  std::unordered_map<std::string, const GLint> m_attributes;
  GLuint m_program;
  void readAttributes();
};