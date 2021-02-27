// Wavefront OBJ Loader
#pragma once

#include <glm/glm.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

namespace objLoader {

  class objFile {
    using idVec = std::vector<size_t>;
    using vec3vec = std::vector<glm::vec3>;
  
  public:
    //struct face {
    //  vec3vec verts, norms;
    //  face(vec3vec v, vec3vec n) : verts(v), norms(n){}
   // };

  private:
    glm::vec3 maxBounds, minBounds;
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> normals;
    std::vector<idVec> faceIndexs;
    std::vector<idVec> normalIndexs;

    //std::vector<face> faces;

    size_t vertsSz = 0;
    bool good = false;

    void readVertex(std::stringstream & ss) {
      float v1, v2, v3;
      ss >> v1 >> v2 >> v3;
      // check against max bounds and store if they exceed what we already have
      // todo: handle multi-object scenes
      if (v1 > maxBounds.x) {
        maxBounds.x = v1;
      } else if(v1 < minBounds.x) {
        minBounds.x = v1;
      }

      if (v2 > maxBounds.y) {
        maxBounds.y = v2;
      } else if (v2 < minBounds.y) {
        minBounds.y = v2;
      }

      if (v3 > maxBounds.z) {
        maxBounds.z = v3;
      } else if (v3 < minBounds.z) {
        minBounds.z = v3;
      }

      verts.push_back(glm::vec3(v1, v2, v3));
    }

    void readNorm(std::stringstream& ss) {
      float v1, v2, v3;
      ss >> v1 >> v2 >> v3;
      normals.push_back(glm::vec3(v1, v2, v3));
    }

    // Retreive 
    void readFace(std::stringstream & ss) {
      std::string faceStr, faceInfo;

      while (std::getline(ss, faceStr)) {
        std::stringstream faceStream(faceStr);
        idVec vertIdxs;
        idVec normIdxs;

        while (std::getline(faceStream, faceInfo, ' ')) {
          std::string vertInfo;
          short counter = 0;
          std::stringstream faceInfoStream(faceInfo);
          while (std::getline(faceInfoStream, vertInfo, '/')) {
          
            //if (counter == 0) {
            //  indexs.push_back(std::atoi(vertInfo.c_str()));
            //  first = false;
            //}

            switch (counter) {
              case 0:{
                // vertex index
                vertIdxs.push_back(std::atoi(vertInfo.c_str()));
                break;
              }
              case 1: {
                // textures

                break;
              }
              case 2: {
                // normal indexs
                normIdxs.push_back(std::atoi(vertInfo.c_str()));

                break;
              }
              default:
                break;
            }
            ++counter;
          }

        }

        faceIndexs.push_back(vertIdxs);
        normalIndexs.push_back(normIdxs);

      }
    }

  public:

    // Parse a triangulated obj file with the name fn
    objFile(std::string fn) : maxBounds(0.f) {
      std::ifstream f(fn);
      if (f.good()) {

        std::string line;

        while (std::getline(f, line)) {
          //std::cout << line << std::endl;

          // get the first 2 chars of the line, we can determine what type
          // of line it is based on these
          const std::string first2 = line.substr(0, 2);
          std::stringstream ss(line.substr(2, line.size()));

          if (first2 == "v ") {
            // vertex
            readVertex(ss);
          } else if(first2 == "f ") {
            // face
            readFace(ss);
          } else if (first2 == "vn") {
            // normal
            readNorm(ss);
          }

        }
        good = true;
      }
    }

    // Check if the file was loaded and the parsing was sucessful
    bool isGood() {
      return good;
    }

    glm::vec3 getMinBounds() {
      return minBounds;
    }

    glm::vec3 getMaxBounds() {
      return maxBounds;
    }
    
    // Get a face's verts as a vec3 vector at a face index
    std::vector<glm::vec3> getFace(size_t fid) {
      std::vector<glm::vec3> resFace;
      for (auto v : faceIndexs[fid]) {
        resFace.push_back(verts.at(v-1));
      }
      return resFace;
    }

    std::vector<glm::vec3> getFaceNormals(size_t fid) {
      
      normals.at(0);
      
      std::vector<glm::vec3> resNorm;
      for (auto v : normalIndexs[fid]) {
        resNorm.push_back(normals.at(v-1));
      }
      return resNorm;
    }

    //face getFaceInfo(size_t fid) {
      

    //}


    // Get face count
    size_t getFaceSz() {
      return faceIndexs.size();
    }

    // Write a file containing face ID, vertex ID, and face vertex info
    void generateDumpFile(std::string fname) {
      std::ofstream dump(fname);
      for (size_t cf = 0; cf < getFaceSz(); cf++) {
        dump << "FACE: " << cf << " VERT IDS:";
        for (auto& vertId : faceIndexs[cf]) {
          dump << vertId << " ";
        }
        dump << std::endl;
        auto faceVerts = getFace(cf);
        for (auto& fv : faceVerts) {
          dump << "  " << fv.x << ", " << fv.y << ", " << fv.z << ", " << std::endl;
        }
      }
    }

  };

}