#include "TextureLoader.hpp"

#include <iostream>
// TODO: we can cut this down to only support .jpg for instance
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureLoader::TextureLoader(const std::string & fname) {
  int width, height, n;
  unsigned char* data = stbi_load(fname.c_str(), &width, &height, &n, 0);
  if (data = nullptr) {
    std::cout << "failed to load " << fname << std::endl;
  }
  glGenTextures(1, &m_texture);
  TextureGuard t_guard(m_texture);
  // TODO: these might have to use GL_RGBA for .png
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

GLuint TextureLoader::getTexture() const {
  return m_texture;
}
