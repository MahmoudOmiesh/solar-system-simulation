#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Util/Mesh.h"

const int WINDOW_WIDTH = 1400;
const int WINDOW_HEIGHT = 1000;
const int TRAIL_LENGTH = 200;

glm::vec3 sphericalToCartesian(float r, float theta, float phi);
Mesh generateBackgroundQuad(const char *texturePath);
Mesh generateSphereMesh(float radius, int stacks, int sectors,
                        const char *texturePath);

struct Star {
  float radius;
  glm::vec3 position;
  Mesh mesh;
};

struct Moon {
  float radius;
  float orbitalRadius;
  float revolutionSpeed;
  float rotationSpeed;
  float axisTilt;
  Mesh mesh;
};

struct Planet {
  float radius;
  float revolutionSpeed;
  float rotationSpeed;
  float axisTilt;
  glm::vec3 position;
  std::vector<glm::vec3> trail;
  std::vector<Moon> moons;
  Mesh mesh;
};

int main() {
  // INIT WINDOW
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                        "Solar System Sim", nullptr, nullptr);

  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  gladLoadGL();
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glEnable(GL_DEPTH_TEST);

  // LIGHT
  Shader lightShader("../Shaders/light.vert", "../Shaders/light.frag");

  Star sun = {2.5f, glm::vec3(0.0f, 0.0f, 0.0f),
              generateSphereMesh(2.5f, 20, 20, "../Textures/sun.jpg")};

  glm::vec4 sunColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  glm::vec3 sunPosition = sun.position;
  glm::mat4 sunModel = glm::mat4(1.0f);
  sunModel = glm::translate(sunModel, sunPosition);
  sunModel = glm::scale(sunModel, glm::vec3(sun.radius));

  lightShader.Activate();
  glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE,
                     glm::value_ptr(sunModel));
  glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), sunColor.x,
              sunColor.y, sunColor.z, sunColor.w);

  //  PLANET OBJECTS
  Shader shaderProgram("../Shaders/default.vert", "../Shaders/default.frag");

  Moon earthMoon = {
      0.5f,  4.0f,  1.0f,
      0.05f, 0.05f, generateSphereMesh(1.0f, 20, 20, "../Textures/moon.jpg")};

  std::vector<Planet> planets = {
      // Mercury
      {1.2f, 0.85f, 0.03f, 0.01f, glm::vec3(18.0f, 0.0f, 0.0f),
       std::vector<glm::vec3>(), std::vector<Moon>(),
       generateSphereMesh(1.0f, 20, 20, "../Textures/mercury.jpg")},
      // Venus (retrograde rotation)
      {1.8f, 0.55f, -0.008f, 0.4f, glm::vec3(30.0f, 0.0f, 0.0f),
       std::vector<glm::vec3>(), std::vector<Moon>(),
       generateSphereMesh(1.0f, 20, 20, "../Textures/venus.jpg")},
      // Earth
      {2.0f, 0.45f, 1.0f, 0.41f, glm::vec3(42.0f, 0.0f, 0.0f),
       std::vector<glm::vec3>(), std::vector<Moon>{earthMoon},
       generateSphereMesh(1.0f, 20, 20, "../Textures/earth.jpg")},
      // Mars
      {1.5f, 0.35f, 0.95f, 0.44f, glm::vec3(56.0f, 0.0f, 0.0f),
       std::vector<glm::vec3>(), std::vector<Moon>(),
       generateSphereMesh(1.0f, 20, 20, "../Textures/mars.jpg")},
      // Jupiter
      {3.5f, 0.18f, 1.9f, 0.05f, glm::vec3(80.0f, 0.0f, 0.0f),
       std::vector<glm::vec3>(), std::vector<Moon>(),
       generateSphereMesh(1.0f, 20, 20, "../Textures/jupiter.jpg")},
      // Saturn
      {3.0f, 0.13f, 1.7f, 0.47f, glm::vec3(105.0f, 0.0f, 0.0f),
       std::vector<glm::vec3>(), std::vector<Moon>(),
       generateSphereMesh(1.0f, 20, 20, "../Textures/saturn.jpg")},
      // Uranus (retrograde rotation/tilt)
      {2.5f, 0.09f, -1.2f, 1.70f, glm::vec3(130.0f, 0.0f, 0.0f),
       std::vector<glm::vec3>(), std::vector<Moon>(),
       generateSphereMesh(1.0f, 20, 20, "../Textures/uranus.jpg")},
      // Neptune
      {2.3f, 0.07f, 1.3f, 0.50f, glm::vec3(160.0f, 0.0f, 0.0f),
       std::vector<glm::vec3>(), std::vector<Moon>(),
       generateSphereMesh(1.0f, 20, 20, "../Textures/neptune.jpg")}};

  GLuint modelUniformLocation = glGetUniformLocation(shaderProgram.ID, "model");

  shaderProgram.Activate();

  glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), sunColor.x,
              sunColor.y, sunColor.z, sunColor.w);
  glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), sunPosition.x,
              sunPosition.y, sunPosition.z);

  // Planet Trails
  Shader trailShader("../Shaders/trail.vert", "../Shaders/trail.frag");
  GLuint trailVAO, trailVBO;
  glGenVertexArrays(1, &trailVAO);
  glGenBuffers(1, &trailVBO);
  glBindVertexArray(trailVAO);
  glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * TRAIL_LENGTH, NULL,
               GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glBindVertexArray(0);

  // BACKGROUND
  Shader backgroundShader("../Shaders/background.vert",
                          "../Shaders/background.frag");
  Mesh backgroundMesh = generateBackgroundQuad("../Textures/bg.jpg");

  Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

  //   MAIN LOOP
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Inputs(window);
    camera.updateMatrix(45.0f, 0.1f, 200.0f);

    // BACKGROUND
    glDepthMask(GL_FALSE);
    backgroundShader.Activate();
    backgroundMesh.Draw(backgroundShader, camera);
    glDepthMask(GL_TRUE);

    float elapsed = glfwGetTime();

    // PLANETS
    for (Planet &planet : planets) {
      shaderProgram.Activate();
      glm::mat4 model = glm::mat4(1.0f);

      float angle = elapsed * planet.revolutionSpeed;
      float orbitRadius =
          glm::length(glm::vec2(planet.position.x, planet.position.z));
      glm::vec3 orbitPosition = glm::vec3(
          sun.position.x + orbitRadius * cos(angle), planet.position.y,
          sun.position.z + orbitRadius * sin(angle));

      planet.trail.push_back(orbitPosition);
      if (planet.trail.size() > TRAIL_LENGTH) {
        planet.trail.erase(planet.trail.begin());
      }

      float rotationAngle = elapsed * planet.rotationSpeed;

      model = glm::translate(model, orbitPosition);
      model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
      model = glm::rotate(model, planet.axisTilt, glm::vec3(1.0f, 0.0f, 0.0f));
      model = glm::scale(model, glm::vec3(planet.radius));

      glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE,
                         glm::value_ptr(model));

      planet.mesh.Draw(shaderProgram, camera);

      for (Moon &moon : planet.moons) {
        float moonOrbitAngle = elapsed * moon.revolutionSpeed;
        float moonRotationAngle = elapsed * moon.rotationSpeed;

        glm::mat4 moonModel = glm::mat4(1.0f);
        moonModel = glm::translate(moonModel, orbitPosition);
        moonModel =
            glm::rotate(moonModel, moonOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moonModel = glm::translate(moonModel,
                                   glm::vec3(moon.orbitalRadius, 0.0f, 0.0f));
        moonModel = glm::rotate(moonModel, moonRotationAngle,
                                glm::vec3(0.0f, 1.0f, 0.0f));
        moonModel =
            glm::rotate(moonModel, moon.axisTilt, glm::vec3(1.0f, 0.0f, 0.0f));
        moonModel = glm::scale(moonModel, glm::vec3(moon.radius));

        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE,
                           glm::value_ptr(moonModel));

        moon.mesh.Draw(shaderProgram, camera);
      }

      // Trail
      if (!planet.trail.empty()) {
        trailShader.Activate();
        glm::mat4 vp = camera.cameraMatrix;
        glUniformMatrix4fv(glGetUniformLocation(trailShader.ID, "viewProj"), 1,
                           GL_FALSE, glm::value_ptr(vp));
        glUniform3f(glGetUniformLocation(trailShader.ID, "trailColor"), 0.8f,
                    0.8f, 0.8f);

        glBindVertexArray(trailVAO);
        glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        planet.trail.size() * sizeof(glm::vec3),
                        planet.trail.data());
        glDrawArrays(GL_LINE_STRIP, 0, planet.trail.size());
      }
    }

    // SUN
    lightShader.Activate();
    sun.mesh.Draw(lightShader, camera);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  //   CLEANUP
  shaderProgram.Delete();
  lightShader.Delete();
  backgroundShader.Delete();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

Mesh generateSphereMesh(float radius, int stacks, int sectors,
                        const char *texturePath) {
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  vertices.reserve((stacks + 1) * (sectors + 1));
  indices.reserve(stacks * sectors * 6);

  for (int i = 0; i <= stacks; ++i) {
    float v = (float)i / stacks;
    float theta = v * glm::pi<float>();

    for (int j = 0; j <= sectors; ++j) {
      float u = (float)j / sectors;
      float phi = u * 2 * glm::pi<float>();

      glm::vec3 position = sphericalToCartesian(radius, theta, phi);
      glm::vec3 normal = glm::normalize(position);
      glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
      glm::vec2 texCoord = glm::vec2(u, 1.0f - v);

      vertices.push_back(Vertex{position, normal, color, texCoord});
    }
  }

  for (int i = 0; i < stacks; ++i) {
    for (int j = 0; j < sectors; ++j) {
      GLuint i0 = i * (sectors + 1) + j;
      GLuint i1 = i0 + 1;
      GLuint i2 = i0 + (sectors + 1);
      GLuint i3 = i2 + 1;

      indices.insert(indices.end(), {i0, i2, i1, i1, i2, i3});
    }
  }

  std::vector<Texture> textures;
  textures.push_back(
      Texture(texturePath, GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE));
  return Mesh(vertices, indices, textures);
}

Mesh generateBackgroundQuad(const char *texturePath) {
  std::vector<Vertex> vertices = {
      {{-1.0f, -1.0f, 0.0f}, {0, 0, 1}, {1, 1, 1}, {0, 0}},
      {{1.0f, -1.0f, 0.0f}, {0, 0, 1}, {1, 1, 1}, {1, 0}},
      {{1.0f, 1.0f, 0.0f}, {0, 0, 1}, {1, 1, 1}, {1, 1}},
      {{-1.0f, 1.0f, 0.0f}, {0, 0, 1}, {1, 1, 1}, {0, 1}},
  };
  std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};
  std::vector<Texture> textures = {
      Texture(texturePath, GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE)};
  return Mesh(vertices, indices, textures);
}

glm::vec3 sphericalToCartesian(float r, float theta, float phi) {
  float x = r * sin(theta) * cos(phi);
  float y = r * cos(theta);
  float z = r * sin(theta) * sin(phi);
  return glm::vec3(x, y, z);
};