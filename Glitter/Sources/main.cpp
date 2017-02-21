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
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderProgram.hpp"
#include "TextureLoader.hpp"

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
  std::vector<glm::vec2> m_vertices;
  GLuint m_vao;
  GLuint m_vertices_vbo;
  std::shared_ptr<ShaderProgram> m_program;
  Shape(std::vector<glm::vec2> vertices, std::shared_ptr<ShaderProgram> program) :
      m_vertices(std::move(vertices)), m_program(program) {
    glUseProgram(m_program->getProgram());
    glGenVertexArrays(1, &m_vao);
    VAOGuard vao_guard(m_vao);

    m_vertices_vbo = bufferStaticData(m_vertices, m_program->getAttribute("aPosition"));

    print_vertices();
  }
  template <typename T>
  GLuint bufferStaticData(const std::vector<T>& data, const GLint attribute) const {
    const GLsizeiptr num_bytes = data.size() * sizeof(T);
    constexpr GLint elem_per_vertex = sizeof(T) / sizeof(float);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    VBOGuard vbo_guard(vbo);
    glBufferData(GL_ARRAY_BUFFER, num_bytes, &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attribute, elem_per_vertex, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attribute);
    return vbo;
  }
  virtual void draw() const = 0;
  void print_vertices() const {
    std::cout << "polygon with " << m_vertices.size() << " vertices {" << std::endl;
    for (const glm::vec2& vertex_pair : m_vertices) {
      std::cout << "  " << vertex_pair[0] << ", " << vertex_pair[1] << std::endl;
    }
    std::cout << "}" << std::endl;
  }
  void rebuffer_vertices() {
    const GLsizeiptr num_bytes = m_vertices.size() * sizeof(glm::vec2);
    constexpr GLint elem_per_vertex = sizeof(glm::vec2) / sizeof(float);
    VBOGuard vbo_guard(m_vertices_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes, &m_vertices[0]);
    print_vertices();
  }
  void move(const glm::vec2& dxdy) {
    for (auto& vertex : m_vertices) {
      vertex += dxdy;
    }
    rebuffer_vertices();
  }
};

struct ColoredShape: public Shape {
  const glm::vec3 m_color;
  std::shared_ptr<ShaderProgram> m_program;
  ColoredShape(std::vector<glm::vec2> vertices, glm::vec3 color, std::shared_ptr<ShaderProgram> program):
      Shape(vertices, program), m_color(color), m_program(program) {};
  virtual void draw() const {
    ProgramGuard program_guard(m_program->getProgram());
    VAOGuard vao_guard(m_vao);
    glUniform3fv(m_program->getUniform("uColor"), 1, glm::value_ptr(m_color));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertices.size());
  }
};

void setUniforms(std::shared_ptr<ShaderProgram> program) {
  glm::mat4 model(1.0f);
  glUniformMatrix4fv(program->getUniform("uModelMatrix"), 1, false, glm::value_ptr(model));

  glm::vec3 eye(4.0, 4.0, 3.0);
  glm::vec3 center(0.0, 0.0, 0.0);
  glm::vec3 up(0.0, 1.0, 0.0);
  glm::mat4 view = glm::lookAt(eye, center, up);
  glUniformMatrix4fv(program->getUniform("uViewMatrix"), 1, false, glm::value_ptr(view));

  float aspect_ratio = static_cast<float>(mWidth) / static_cast<float>(mHeight);
  glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 10.0f);
  glUniformMatrix4fv(program->getUniform("uProjMatrix"), 1, false, glm::value_ptr(proj));
}

void setup(std::vector<Shape*>& shapes) {

  // oh boy, Windows paths seem relative to the CWD where the .exe was executed from.
  auto program = std::make_shared<ShaderProgram>("Glitter\\Shaders\\hello.vert",
    "Glitter\\Shaders\\hello.frag");

  std::vector<glm::vec2> t1_vertices = {
    {  0.0f,  0.5f },
    {  0.5f, -0.5f },
    { -0.5f, -0.5f }
  };
  glm::vec3 red = { 1.0, 0.0, 0.0 };
  shapes.push_back( new ColoredShape(t1_vertices, red, program));

  std::vector<glm::vec2> t2_vertices = {
    { 0.0f, -0.75f },
    { -0.5f, 0.25f },
    { 0.5f, 0.25f }
  };
  glm::vec3 green = { 0.0, 1.0, 0.0 };
  shapes.push_back(new ColoredShape(t2_vertices, green, program));

  std::vector<glm::vec2> s1_vertices = {
    { -0.85, 0.85 },
    { -0.65, 0.85 },
    { -0.85, 0.65 },
    { -0.65, 0.65 }
  };
  glm::vec3 blue = { 0.0, 0.0, 1.0 };
  shapes.push_back(new ColoredShape(s1_vertices, blue, program));

  setUniforms(program);
}

void handle_input(GLFWwindow* const window, std::vector<Shape*>& shapes) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  int up_pressed = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
  int down_pressed = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
  int left_pressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
  int right_pressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
  bool dirty = up_pressed || down_pressed || left_pressed || right_pressed;
  if (dirty) {
    shapes.back()->move(glm::vec2(0.1 * static_cast<float>(right_pressed - left_pressed),
      0.1 * static_cast<float>(up_pressed - down_pressed)));
  }
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

  std::vector<Shape*> shapes;
  setup(shapes);

  // Rendering Loop
  while (glfwWindowShouldClose(mWindow) == false) {
    handle_input(mWindow, shapes);

    // Background Fill Color
    glClearColor(0.25f, 0.5f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const Shape* shape: shapes) {
      shape->draw();
    }

    // Flip Buffers and Draw
    glfwSwapBuffers(mWindow);
    glfwWaitEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
