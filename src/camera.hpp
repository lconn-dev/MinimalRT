#pragma once 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct camera {
  glm::mat4 projection, view, inverseViewProjection;
  glm::vec3 camDirection, camUp, position;
  float xsz, ysz, FOV;
  float nearP, farP;

  camera(float fov, float xres, float yres, float near, float far)
    : xsz(xres), ysz(yres), FOV(fov), nearP(near), farP(far)
  {
    position = glm::vec3(0.f, 0.f, 0.f);
    projection = glm::perspective(glm::radians(FOV), xres / yres, near, far);
    camDirection = glm::vec3(0.f, 0.f, -1.f);
    camUp = glm::vec3(0, 1.f, 0);
    view = glm::lookAt(glm::vec3(0.f), camDirection, camUp);
    inverseViewProjection = glm::inverse(projection * view);
  }

  glm::vec3 getPos() const {
    return position;
  }

  glm::vec3 getRayDir(glm::vec2 const& pixelRaster) const {
    // convert pixel coordinate to -1, 1 space.
    const glm::vec2 normalizedPixel(pixelRaster.x / (xsz * 0.5) - 1.f, pixelRaster.y / (ysz * 0.5) - 1.f);
    const glm::vec4 screenPos(normalizedPixel.x, -normalizedPixel.y, 1.f, 1.f);
    const glm::vec4 worldPos = inverseViewProjection * screenPos;
    const glm::vec3 direction(glm::normalize(glm::vec3(worldPos)));
    return direction;
  }

};