// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

class Shape {
public:
  std::vector<glm::vec2> m_vertices;
  GLuint m_vao;
  GLuint m_vbo;
  GLint m_posAttrib;
  Shape(std::vector<glm::vec2> vertices, GLint posAttrib): m_vertices(std::move(vertices)),
      m_posAttrib(posAttrib) {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec2), &m_vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(m_posAttrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_posAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    print_vertices();
  }
  void draw() const {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    glBindVertexArray(0);
  }
  void print_vertices() const {
    std::cout << "created polygon with " << m_vertices.size() << " vertices {" << std::endl;
    for (const glm::vec2& vertex_pair : m_vertices) {
      std::cout << "  " << vertex_pair[0] << ", " << vertex_pair[1] << std::endl;
    }
    std::cout << "}" << std::endl;
  }
};

void setup(std::vector<Shape>& shapes) {
  const GLchar* vshader_src =
    "#version 150\n"
    "in vec2 position;"
    "void main() {"
    "  gl_Position = vec4(position, 0.0, 1.0);"
    "}";
  GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vshader, 1, &vshader_src, nullptr);
  glCompileShader(vshader);

  GLint vertex_compiled;
  glGetShaderiv(vshader, GL_COMPILE_STATUS, &vertex_compiled);
  if (vertex_compiled != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(vshader, 1024, &log_length, message);
    std::cout << message << std::endl;
  }

  const GLchar* fshader_src =
    "#version 150\n"
    "out vec4 frag_color;"
    "void main() {"
    "  frag_color = vec4(1.0, 0.0, 0.0, 1.0);"
    "}";
  GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fshader, 1, &fshader_src, nullptr);
  glCompileShader(fshader);

  GLint fragment_compiled;
  glGetShaderiv(fshader, GL_COMPILE_STATUS, &fragment_compiled);
  if (fragment_compiled != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(fshader, 1024, &log_length, message);
    std::cout << message << std::endl;
  }

  GLuint program = glCreateProgram();

  glAttachShader(program, vshader);
  glAttachShader(program, fshader);
  glBindFragDataLocation(program, 0, "frag_color");
  glLinkProgram(program);

  GLint program_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
  if (program_linked != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetProgramInfoLog(program, 1024, &log_length, message);
    std::cout << message << std::endl;
  }
  glUseProgram(program);
  GLint posAttrib = glGetAttribLocation(program, "position");

  std::vector<glm::vec2> t1_vertices = {
    {  0.0f,  0.5f },
    {  0.5f, -0.5f },
    { -0.5f, -0.5f }
  };
  shapes.emplace_back(t1_vertices, posAttrib);

  std::vector<glm::vec2> t2_vertices = {
    { 0.0f, -0.75f },
    { -0.5f, 0.25f },
    { 0.5f, 0.25f }
  };
  shapes.emplace_back(t2_vertices, posAttrib);

  std::vector<glm::vec2> s1_vertices = {
    { -0.85, 0.85 },
    { -0.65, 0.85 },
    { -0.85, 0.65 },
    { -0.65, 0.85 },
    { -0.85, 0.65 },
    { -0.65, 0.65 }
  };
  shapes.emplace_back(s1_vertices, posAttrib);
}

int main(int argc, char * argv[]) {

  // Load GLFW and Create a Window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  auto mWindow = glfwCreateWindow(mWidth, mHeight, "My first game", nullptr, nullptr);

  // Check for Valid Context
  if (mWindow == nullptr) {
    fprintf(stderr, "Failed to Create OpenGL Context");
    return EXIT_FAILURE;
  }

  // Create Context and Load OpenGL Functions
  glfwMakeContextCurrent(mWindow);
  gladLoadGL();
  fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

  std::vector<Shape> shapes;
  setup(shapes);

  // Rendering Loop
  while (glfwWindowShouldClose(mWindow) == false) {
    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(mWindow, true);

    // Background Fill Color
    glClearColor(0.25f, 0.5f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const Shape& shape: shapes) {
      shape.draw();
    }

    // Flip Buffers and Draw
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
