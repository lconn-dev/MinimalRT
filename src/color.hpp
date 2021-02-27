#pragma once

#include <glm/glm.hpp>

namespace color {

  // normalized RGB float struture
  struct rgb {
    float r = 0.f, g = 0.f, b = 0.f;
    rgb(float ra, float ba, float ga) : r(ra), b(ba), g(ga){}
    rgb(){}
    rgb(glm::vec3 c) {
      r = c.r;
      g = c.g;
      b = c.b;
    }
  };

}