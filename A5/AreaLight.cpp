#include <iostream>

#include <glm/ext.hpp>
#include "AreaLight.hpp"

AreaLight::AreaLight(glm::vec3 pos, double w, double h, glm::vec3 color, glm::vec3 falloff) : Light(pos, color, falloff), width(w), height(h) {}

double AreaLight::getArea() { return width * height; };

std::vector<Ray> AreaLight::getRays(glm::vec3 point) {
    std::vector<Ray> v;

    for (int i = 0; i < SHADOW_SAMPLE; i++) {
        for (int j = 0; j < SHADOW_SAMPLE; j++) {
            double rx = random(position.x + i * width / SHADOW_SAMPLE, position.x + (i + 1) * width / SHADOW_SAMPLE);
            double ry = random(position.y + j * height / SHADOW_SAMPLE, position.y + (j + 1) * height / SHADOW_SAMPLE);
            glm::vec3 npos = glm::vec3(rx, ry, position.z);
            glm::vec3 direction = normalize(npos - point);
            Ray lightRay(point + EPS * direction, direction);
            v.push_back(lightRay);
        }
    }

    return v;
}

int AreaLight::getSamples() { return SHADOW_SAMPLE * SHADOW_SAMPLE; }
