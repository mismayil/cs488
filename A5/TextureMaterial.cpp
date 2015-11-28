#include "TextureMaterial.hpp"

TextureMaterial::TextureMaterial(const char *filename) {
    image = new Image();
    image->loadPng(filename);
}

TextureMaterial::~TextureMaterial() {
    delete image;
}

Image* TextureMaterial::getImage() {
    return image;
}
