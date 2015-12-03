#include "TextureMaterial.hpp"

TextureMaterial::TextureMaterial(const char *filename, glm::vec3 ks, double shininess, double reflectiveness, double refractiveness, double transparency, double bumpness) :
                                                PhongMaterial(glm::vec3(0), ks, shininess, reflectiveness, refractiveness, transparency, bumpness) {
    texture = new Image();
    texture->loadPng(filename);
}

TextureMaterial::~TextureMaterial() {
    delete texture;
}

Image* TextureMaterial::getTexture() {
    return texture;
}

void TextureMaterial::setuv(int *uv) {
    this->uv = uv;
}

int* TextureMaterial::getuv() {
    return uv;
}

glm::vec3 TextureMaterial::getkd() {
    return texture->getuv()[uv[1]][uv[0]];
}
