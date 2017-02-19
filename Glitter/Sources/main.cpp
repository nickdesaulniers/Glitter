// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

#include "ShaderProgram.hpp"

struct VAOGuard {
  VAOGuard(const GLuint vao) { glBindVertexArray(vao); }
  ~VAOGuard() { glBindVertexArray(0); }
};

struct VBOGuard {
  VBOGuard(const GLuint vbo) { glBindBuffer(GL_ARRAY_BUFFER, vbo); }
  ~VBOGuard() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};

struct ProgramGuard {
  ProgramGuard(const GLuint program) { glUseProgram(program); }
  ~ProgramGuard() { glUseProgram(0); }
};

struct Shape {
  const std::vector<glm::vec2> m_vertices;
  const glm::vec3 m_color;
  GLuint m_vao;
  std::shared_ptr<ShaderProgram> m_program;
  Shape(std::vector<glm::vec2> vertices, glm::vec3 color, std::shared_ptr<ShaderProgram> program) :
      m_vertices(std::move(vertices)), m_color(color), m_program(std::move(program)) {
    glGenVertexArrays(1, &m_vao);
    VAOGuard vao_guard(m_vao);

    glUseProgram(m_program->getProgram());

    // should just use a uniform
    std::vector<glm::vec3> colors;
    for (auto _ : m_vertices) {
      colors.push_back(m_color);
    }

    bufferStaticData(m_vertices, m_program->getAttribute("position"));
    bufferStaticData(colors, m_program->getAttribute("a_color"));

    print_vertices();
  }
  template <typename T>
  void bufferStaticData(const std::vector<T>& data, const GLint attribute) const {
    const GLsizeiptr num_bytes = data.size() * sizeof(T);
    constexpr GLint elem_per_vertex = sizeof(T) / sizeof(float);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    VBOGuard vbo_guard(vbo);
    glBufferData(GL_ARRAY_BUFFER, num_bytes, &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attribute, elem_per_vertex, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attribute);
  }
  void draw() const {
    ProgramGuard program_guard(m_program->getProgram());
    VAOGuard vao_guard(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertices.size());
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

  // oh boy, Windows paths seem relative to the CWD where the .exe was executed from.
  auto program = std::make_shared<ShaderProgram>("Glitter\\Shaders\\hello.vert",
    "Glitter\\Shaders\\hello.frag");

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
