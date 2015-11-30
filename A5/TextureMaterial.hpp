#pragma once

#include <glm/glm.hpp>

#include "PhongMaterial.hpp"
#include "Image.hpp"

class TextureMaterial : public PhongMaterial {
    Image *texture;
    int *uv;
public:
    TextureMaterial(const char *filename, glm::vec3 ks, double shininess, double reflectiveness, double refractiveness, double bumpness);
    virtual ~TextureMaterial();
    Image *getTexture();
    void setuv(int *uv);
    int *getuv();
    glm::vec3 getkd();
};
