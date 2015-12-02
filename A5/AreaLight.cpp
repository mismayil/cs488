#include <iostream>

#include <glm/ext.hpp>
#include "AreaLight.hpp"

AreaLight::AreaLight(glm::vec3 pos, double w, double h, glm::vec3 color, glm::vec3 falloff) : Light(pos, color, falloff), width(w), height(h) {}

double AreaLight::getArea() { return width * height; };

std::vector<Ray> AreaLight::getRays(glm::vec3 point) {
    std::vector<Ray> v;

    for (int i = 0; i < SHADOW_SAMPLE; i++) {
        for (int j = 0; j < SHADOW_SAMPLE; j++) {
            double rx = width / 2 + (random(0, width) + i) / SHADOW_SAMPLE;
            double ry = height / 2 + (random(0, height) + j) / SHADOW_SAMPLE;
            glm::vec3 npos = position + glm::vec3(rx, ry, 0);
            glm::vec3 direction = normalize(npos - point);
            Ray lightRay(point + EPS * direction, direction);
            v.push_back(lightRay);
        }
    }

    return v;
}
