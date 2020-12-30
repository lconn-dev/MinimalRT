#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>


struct material {
  glm::vec3 amb, diffuse, specular, specularColor;
  float shine = 16.f;

  material() : amb(0.f), diffuse(1.f), specular(1.f), specularColor(1.f){}
};


struct triangle {
  glm::vec3 v0, v1, v2, n0, n1, n2;

  material* mat = nullptr;

  glm::vec3 getSurfNormal() {
    return glm::triangleNormal(v0, v1, v2);
  }

  void applyModel(glm::mat4 const& mo) {
    v0 = mo * glm::vec4(v0, 1.f);
    v1 = mo * glm::vec4(v1, 1.f);
    v2 = mo * glm::vec4(v2, 1.f);
  }

  triangle() :
    v0(0.f), v1(0.f), v2(0.f),
    n0(0.f), n1(0.f), n2(0.f) {}

  triangle(std::vector<glm::vec3> const& face, material* matPtr, std::vector<glm::vec3> norms) : mat(matPtr) {
    assert(face.size() == 3);
    assert(norms.size() == 3);
    v0 = face[0];
    v1 = face[1];
    v2 = face[2];
    n0 = norms[0];
    n1 = norms[1];
    n2 = norms[2];
  }

};
