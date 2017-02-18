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
  const std::vector<glm::vec2> m_vertices;
  const glm::vec3 m_color;
  GLuint m_vao;
  GLuint m_vbo;
  const GLuint m_program;
  Shape(std::vector<glm::vec2> vertices, glm::vec3 color, GLuint program) :
      m_vertices(std::move(vertices)), m_color(color), m_program(program) {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glUseProgram(m_program);
    GLint posAttrib = glGetAttribLocation(m_program, "position");
    GLint colorAttrib = glGetAttribLocation(m_program, "a_color");

    bufferStaticData(m_vertices.size() * sizeof(glm::vec2), &m_vertices[0], posAttrib, 2);

    // should just use a uniform
    std::vector<glm::vec3> colors;
    for (auto _ : m_vertices) {
      colors.push_back(m_color);
    }

    bufferStaticData(colors.size() * sizeof(glm::vec3), &colors[0], colorAttrib, 3);

    glBindVertexArray(0);

    print_vertices();
  }
  void bufferStaticData(GLsizeiptr num_bytes, const GLvoid* data, GLuint attribute,
      GLint elem_per_vertex) const {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, num_bytes, data, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute, elem_per_vertex, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attribute);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  void draw() const {
    glUseProgram(m_program);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertices.size());
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
    "in vec3 a_color;"
    "out vec4 v_color;"
    "void main() {"
    "  gl_Position = vec4(position, 0.0, 1.0);"
    "  v_color = vec4(a_color, 1.0);"
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
    // exit
  }

  const GLchar* fshader_src =
    "#version 150\n"
    "in vec4 v_color;"
    "out vec4 frag_color;"
    "void main() {"
    "  frag_color = v_color;"
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
    // exit
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
    // exit
  }

  std::vector<glm::vec2> t1_vertices = {
    {  0.0f,  0.5f },
    {  0.5f, -0.5f },
    { -0.5f, -0.5f }
  };
  glm::vec3 red = { 1.0, 0.0, 0.0 };
  shapes.emplace_back(t1_vertices, red, program);

  std::vector<glm::vec2> t2_vertices = {
    { 0.0f, -0.75f },
    { -0.5f, 0.25f },
    { 0.5f, 0.25f }
  };
  glm::vec3 green = { 0.0, 1.0, 0.0 };
  shapes.emplace_back(t2_vertices, green, program);

  std::vector<glm::vec2> s1_vertices = {
    { -0.85, 0.85 },
    { -0.65, 0.85 },
    { -0.85, 0.65 },
    { -0.65, 0.65 }
  };
  glm::vec3 blue = { 0.0, 0.0, 1.0 };
  shapes.emplace_back(s1_vertices, blue, program);
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
