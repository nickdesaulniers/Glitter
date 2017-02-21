#include <string>
#include "glitter.hpp"

class TextureLoader {
  GLuint m_texture;
public:
  TextureLoader(const std::string& fname);
  GLuint getTexture() const;
  struct TextureGuard {
    TextureGuard(GLuint texture) { glBindTexture(GL_TEXTURE_2D, texture); };
    ~TextureGuard() { glBindTexture(GL_TEXTURE_2D, 0); };
  };
};