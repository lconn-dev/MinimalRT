/*// Calculate ray intersections & pixel colors
  for (auto x = 0; x < xres; x++) {

    for (auto y = 0; y < yres; y++) {

      const glm::vec3 rayDir = cam.getRayDir(glm::vec2(x,y));
      const glm::vec3 rayStartPos = cam.getPos();
      glm::vec2 intersection(0.f);

      bool miss = true;
      double nearestHit = DBL_MAX;
      size_t triIndex = 0;
      glm::vec3 fragPos(0.f);

      for (size_t t = 0; t < triBuffer.size(); t++) {

        std::unique_ptr<triangle> & tri = triBuffer[t];

        float distance = 0.f;
        if (glm::intersectRayTriangle(
          rayStartPos, rayDir, tri->v0, tri->v1, tri->v2, intersection, distance)
        ){
          // make sure the distance is positive so we dont render triangles behind the camera
          //const double EPSILON = 0.0001;
          if (distance > near && distance < nearestHit) {
            nearestHit = distance;
            fragPos = rayStartPos + distance * rayDir;
            triIndex = t;
            miss = false;
          }

        }
      }

      if (!miss) {
        // shade this pixel
        glm::vec3 normal = triBuffer[triIndex]->getSurfNormal();

        glm::vec3 finalColor(0.f);
        for (auto& lig : lights) {
          finalColor += lig->phong(fragPos, cam, *triBuffer[triIndex].get());
        }

        // reinhard tone mapping
        glm::vec3 mapped = finalColor / (finalColor + glm::vec3(1.0));
        // gamma correction
        const float gamma = 2.2f;
        mapped = glm::pow(mapped, glm::vec3(1.0f / gamma));
        frame.write(x, y, color::rgb(mapped));
      } else {
        glm::vec3 grade = glm::mix(glm::vec3(0.5f), glm::vec3(0.0f), (float)y/yres);
        frame.write(x, y, color::rgb(glm::vec3(bgColor)));
      }

    }
    if (x % displayPercentDivisor == 0) std::cout << "Progress: " << ((float)x / xres) * 100.f << "%" << std::endl;
  }
  */