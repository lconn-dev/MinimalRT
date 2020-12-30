#pragma once

#include "camera.hpp"
#include "triangle.hpp"

#include <glm/glm.hpp>

// barycentric calculation function from Christer Ericson's Real-Time Collision Detection
// https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
void barycentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, float& u, float& v, float& w){
  glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
  float d00 = glm::dot(v0, v0);
  float d01 = glm::dot(v0, v1);
  float d11 = glm::dot(v1, v1);
  float d20 = glm::dot(v2, v0);
  float d21 = glm::dot(v2, v1);
  float denom = d00 * d11 - d01 * d01;
  v = (d11 * d20 - d01 * d21) / denom;
  w = (d00 * d21 - d01 * d20) / denom;
  u = 1.0f - v - w;
}

class light {
public:
  glm::vec3 position;
  glm::vec3 diffuseColor;
  float intensity = 1.f;

  light() : diffuseColor(1.f, 1.f, 1.f), position(0.f) {}

  virtual glm::vec3 lambert(
    glm::vec3 const& fragPos, camera const& cam, triangle& tri) = 0;

  virtual glm::vec3 phong(
    glm::vec3 const& fragPos, camera const& cam, triangle& tri) = 0;

};


class pointLight : public light {
  using vec3 = glm::vec3;
public:
  pointLight(glm::vec3 const& pos, float in) {
    position = pos;
    intensity = in;
  }

  // todo add light intensity calculation

  glm::vec3 lambert(glm::vec3 const& fragPos, camera const& cam, triangle& tri) override {
    using vec3 = glm::vec3;

    //vec3 norm = tri.getSurfNormal();
    float u, v, w;
    barycentric(fragPos, tri.v0, tri.v1, tri.v2, u, v, w);
    vec3 norm = glm::normalize((tri.n0 * u) + (tri.n1 * v) + (tri.n2 * w));

    float dot = glm::dot(norm, glm::normalize(this->position));
    float diff = std::max(0.0f, dot);
    vec3 diffuse = diff * diffuseColor; // apply light color

    // Ambient Coloring
    const vec3 ambient = tri.mat->amb;

    return (diffuse + ambient) * tri.mat->diffuse;
  }

  vec3 phong(glm::vec3 const& fragPos, camera const& cam, triangle& tri) override {
    vec3 res(0.f);
    auto& viewPos = cam.position;
    vec3 viewDir(glm::normalize(viewPos - fragPos));
    vec3 lightDir(glm::normalize(this->position - fragPos));

    float u, v, w;
    barycentric(fragPos, tri.v0, tri.v1, tri.v2, u, v, w);
    vec3 normal = glm::normalize((tri.n0 * u) + (tri.n1 * v) + (tri.n2 * w));
    float specular = 0.0;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specAngle = std::max(glm::dot(normal, halfwayDir), 0.f);
    specular = pow(specAngle, tri.mat->shine);

    auto diffuse = lambert(fragPos, cam, tri);

    res = diffuse + (specular * tri.mat->specularColor);

    return res;
  }
};
