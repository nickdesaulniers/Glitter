#include "TextureLoader.hpp"

#include <iostream>
// TODO: we can cut this down to only support .jpg for instance
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureLoader::TextureLoader(const std::string & fname) {
  int width, height, n;
  unsigned char* data = stbi_load(fname.c_str(), &width, &height, &n, 0);
  if (data == nullptr) {
    std::cout << "failed to load " << fname << std::endl;
  }
  std::cout << "loaded image " << fname << " " << width << ", " << height << ", " << n << std::endl;
  glActiveTexture(GL_TEXTURE0); // where does this go?
  glGenTextures(1, &m_texture);
  TextureGuard t_guard(m_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // From https://www.khronos.org/opengl/wiki/Common_Mistakes#Image_precision it seems
  // that this should be         GL_RGBA8                  GL_BGRA but that does not work.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

GLuint TextureLoader::getTexture() const {
  return m_texture;
}
