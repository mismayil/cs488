#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "Image.hpp"

class TextureMaterial : public Material {
public:
  TextureMaterial(const char *filename);
  virtual ~TextureMaterial();
  Image *getImage();
private:
    Image *image;
};
