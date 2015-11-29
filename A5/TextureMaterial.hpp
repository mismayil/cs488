#pragma once

#include <glm/glm.hpp>

#include "PhongMaterial.hpp"
#include "Image.hpp"

class TextureMaterial : public PhongMaterial {
public:
  TextureMaterial(const char *filename, glm::vec3 ks, double shininess, double reflectiveness, double refractiveness);
  virtual ~TextureMaterial();
  Image *getTexture();
  void setuv(int *uv);
  int *getuv();
  glm::vec3 getkd();
private:
    Image *texture;
    int *uv;
};
