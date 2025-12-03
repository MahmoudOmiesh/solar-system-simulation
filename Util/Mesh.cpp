#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices,
           std::vector<Texture> &textures) {
  Mesh::vertices = vertices;
  Mesh::indices = indices;
  Mesh::textures = textures;

  _VAO.Bind();
  // Generates Vertex Buffer Object and links it to vertices
  VBO VBO(vertices);
  // Generates Element Buffer Object and links it to indices
  EBO EBO(indices);
  // Links VBO attributes such as coordinates and colors to VAO
  _VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
  _VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex),
                  (void *)(3 * sizeof(float)));
  _VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex),
                  (void *)(6 * sizeof(float)));
  _VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex),
                  (void *)(9 * sizeof(float)));
  // Unbind all to prevent accidentally modifying them
  _VAO.Unbind();
  VBO.Unbind();
  EBO.Unbind();
}

void Mesh::Draw(Shader &shader, Camera &camera) {
  // Bind shader to be able to access uniforms
  shader.Activate();
  _VAO.Bind();

  for (unsigned int i = 0; i < textures.size(); i++) {
    textures[i].texUnit(shader, ("tex" + std::to_string(i)).c_str(), i);
    textures[i].Bind();
  }

  // Take care of the camera Matrix
  glUniform3f(glGetUniformLocation(shader.ID, "cameraPosition"),
              camera.Position.x, camera.Position.y, camera.Position.z);
  camera.Matrix(shader, "cameraMatrix");

  // Draw the actual mesh
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}