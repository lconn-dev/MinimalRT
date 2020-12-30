#include <iostream>
#include <execution>
#include <vector>
#include <memory>
#include <limits>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <atomic>

#include "tga.hpp"
#include "framebuffer.hpp"
#include "model.hpp"
#include "objLoader.hpp"
#include "camera.hpp"
#include "shading.hpp"
#include "triangle.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


struct geoModel {
  std::string name;
  material* mat;
  glm::mat4 model;
  geoModel(std::string n, material& ma, glm::mat4 mod)
    : name(n), mat(&ma), model(mod){}
};

// Store information about a ray intersection
struct intersectInfo {
  bool hit = false;
  float dist = 0.f;
  size_t geoBufferTriIndex = 0;
  glm::vec2 intersectCoord;
};

// Manage the scene geometry and calculate intersections
class scene {
private: 
  std::vector<std::shared_ptr<triangle>> triBuffer;
  camera* cam;

public:

  size_t getGeoBufSz() {
    return triBuffer.size();
  }

  triangle* getTriangle(size_t index) {
    return triBuffer[index].get();
  }

  // initialize scene
  scene(camera& useCam) : cam(&useCam){}

  // Load a obj model into the scene geometry buffer
  bool loadObj(geoModel& modelInfo) {
    
    // Load OBJ File into memory
    objLoader::objFile obj(modelInfo.name);

    // Load triangles into geo buffer
    for (auto x = 0; x < obj.getFaceSz(); x++) {
      // TODO: store ptr to geoModel instead of just material
      triBuffer.push_back(std::make_shared<triangle>(obj.getFace(x), modelInfo.mat, obj.getFaceNormals(x)));
    }

    std::for_each(std::execution::par_unseq, std::begin(triBuffer), std::end(triBuffer), [=](auto& tri) {
      tri->applyModel(cam->view * modelInfo.model);
    });

  }

  // Intersect a ray with the scene and store the result in an intersectInfo struct
  bool rayTriIntersect(intersectInfo& result, glm::vec3 const& rayStart, glm::vec3 const& rayDir) {

    float nearestHit = FLT_MAX; // Set the nearest hit to max distance

    for (size_t t = 0; t < triBuffer.size(); t++) {
      const auto& tri = triBuffer[t].get();
      //float distance = 0.f;
      if (glm::intersectRayTriangle(rayStart, rayDir, tri->v0, tri->v1, tri->v2, result.intersectCoord, result.dist)) {
        
        if (result.dist > cam->nearP && result.dist < nearestHit) {
          nearestHit = result.dist;
          result.geoBufferTriIndex = t;
          result.hit = true;
        }
      }

    }
    return result.hit;
  }

};

int main() {
  std::cout << "SimpleRT - Luke Connor 2020 " << std::endl;
  std::cout << "Loading..." << std::endl;
  
  const glm::vec3 bgColor(0.f,0.f,0.f);
  const int displayPercentDivisor = 32;
  const int xres = 1280;
  const int yres = 720;
  float fov = 90.f;
  const float aspectRatio = (float)xres / (float)yres;
  const float near = 0.001f, far = 500.f;
  camera cam(fov, xres, yres, near, far);

  framebuffer::frameBuf32 frame(xres, yres);
  tga::tgaImg output(&frame);

  material defaultMat;
  defaultMat.amb = glm::vec3(0.0025f);
  defaultMat.specularColor = glm::vec3(0.5, 0.2, 0.2);
  defaultMat.diffuse = glm::vec3(0.5f, 0.f, 0.f);

  glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.0f, -2.30f));
  model = glm::rotate(model, glm::radians(5.f), glm::vec3(0.f, 1.f, 0.f));
  glm::mat4 mvp = cam.projection * cam.view * model;

  scene sceneManager(cam);
  geoModel dragon("../../../models/dragonHiRes.obj", defaultMat, model);
  sceneManager.loadObj(dragon);

  // Setup lights
  using lightPtr = std::shared_ptr<light>;
  lightPtr light1(std::make_shared<pointLight>(glm::vec3(0, 3, 1.2), 40.f));
  std::vector<lightPtr> lights;
  lights.push_back(light1);

  std::cout << "Loaded Scene: " << sceneManager.getGeoBufSz() << " faces." << std::endl;
  // optional dump of obj file for debugging
  //obj.generateDumpFile("dump.txt");

  std::cout << "Rendering..." << std::endl;
  auto startTime = std::chrono::steady_clock::now();

  // use a vector sequence for parallel for_each until we have a custom iterator
  std::vector<size_t> xSeq(xres); 
  std::iota(std::begin(xSeq), std::end(xSeq), 0);

  // Atomic progress counter
  std::atomic<size_t> progressX;

  std::for_each(std::execution::par_unseq, std::begin(xSeq), std::end(xSeq), [&](auto& x){
    for (auto y = 0; y < yres; y++) {

      // Setup Ray parameters, direction and origin point
      glm::vec3 rayDir = cam.getRayDir(glm::vec2(x, y));
      glm::vec3 rayStartPos = cam.getPos();

      // Intersect camera ray with scene
      intersectInfo intersectData;     
      sceneManager.rayTriIntersect(intersectData, rayStartPos, rayDir);
      
      if (intersectData.hit) {
        // get the hit triangle and calculate the fragment position of the intersection point
        auto * hitTri = sceneManager.getTriangle(intersectData.geoBufferTriIndex);
        glm::vec3 fragPos = rayStartPos + intersectData.dist * rayDir;
        
        // shade this pixel
        glm::vec3 normal = hitTri->getSurfNormal();

        glm::vec3 finalColor(0.f);
        for (auto& lig : lights) {
          finalColor += lig->phong(fragPos, cam, *hitTri);
        }

        // reinhard tone mapping
        glm::vec3 mapped = finalColor / (finalColor + glm::vec3(1.0));
        // gamma correction 
        const float gamma = 2.2f;
        mapped = glm::pow(mapped, glm::vec3(1.0f / gamma));
        frame.write(x, y, color::rgb(mapped));
      } else {
        // create gradient for background
        glm::vec3 grade = glm::mix(glm::vec3(0.5f), glm::vec3(0.0f), (float)y / yres);
        frame.write(x, y, color::rgb(glm::vec3(grade)));
      }
     
      //if (progressX % displayPercentDivisor == 0) std::cout << "Progress: " << ((float)progressX / xres) * 100.f << "%" << std::endl;
    }
    
    progressX++;
    if (progressX % displayPercentDivisor == 0) std::cout << "Progress: " << floor(((float)progressX / xres) * 100.f) << "%" << std::endl;
  });

  
  std::cout << "Render Complete, writing image...";

  auto endTime = std::chrono::steady_clock::now();

  if (output.save("output")) {
    std::cout << " Done!" << std::endl;
  } else {
    std::cout << " Error Writing File!!!" << std::endl;
  }

  // output time stats
  std::chrono::duration<float> elapsedTime = endTime - startTime;
  float elapsedSeconds = elapsedTime.count();
  float elapsedMinutes = (elapsedSeconds / 60 > 1.0f) ? elapsedSeconds / 60 : 0;
  std::cout << "Operation completed in " << std::max(0.f,floor(elapsedMinutes)) << " minutes, " 
            << fmod(elapsedSeconds, 60.f) << " seconds." << std::endl;

  return 0;
}