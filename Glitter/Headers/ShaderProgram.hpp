#include <string>
#include <unordered_map>
class ShaderProgram {
public:
  ShaderProgram(const std::string& vertex_shader_fname, const std::string& fragment_shader_fname);
  GLuint getProgram() const;
  class Attribute {
    const std::string m_name;
    const GLint m_size;
    const GLenum m_type;
  public:
    Attribute(std::string name, GLint size, GLenum type) :
      m_name(name), m_size(size), m_type(type) {}
  };
private:
  GLuint m_program;
  std::unordered_map<std::string, Attribute> m_attributes;
  std::unordered_map<std::string, Attribute> m_uniforms;
  void readAttributes();
};