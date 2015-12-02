#include <iostream>

#include <glm/ext.hpp>
#include "AreaLight.hpp"

AreaLight::AreaLight(glm::vec3 pos, double w, double h, glm::vec3 color, glm::vec3 falloff) : Light(pos, color, falloff), width(w), height(h) {}

double AreaLight::getArea() { return width * height; };

std::vector<softRay> AreaLight::getRays(glm::vec3 point, double tao) {
    std::vector<softRay> s;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double rx = random(0, width);
            double ry = random(0, height);
            glm::vec3 npos = position + glm::vec3(rx, ry, 0);
            Ray lightRay(npos, normalize(npos - point));
            Ray shadowRay(point, (EPS + tao) * lightRay.d);
            s.push_back({lightRay, shadowRay});
        }
    }

    return s;
}
