#include "util.hpp"
#include <iostream>

plane getplane(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;

    glm::vec3 abxac = glm::cross(ab, ac);
    double d = -abxac.x * a.x - abxac.y * a.y - abxac.z * a.z;
    return plane(abxac.x, abxac.y, abxac.z, d);
}

double intersectPLane(struct plane plane, glm::vec3 eye, glm::vec3 ray) {
    return -(plane.A * eye.x + plane.B * eye.y + plane.C * eye.z + plane.D) / (plane.A * ray.x + plane.B * ray.y + plane.C * ray.z);
}

TAO *intersectTriangle(glm::vec3 eye, glm::vec3 ray, glm::vec3 u, glm::vec3 v, glm::vec3 w) {
    double a = u.x - v.x;
    double b = u.y - v.y;
    double c = u.z - v.z;
    double d = u.x - w.x;
    double e = u.y - w.y;
    double f = u.z - w.z;
    double g = ray.x;
    double h = ray.y;
    double i = ray.z;
    double j = u.x - eye.x;
    double k = u.y - eye.y;
    double l = u.z - eye.z;
    double M = a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);
    double alpha = (j * (e *  i - h * f) + k * (g * f - d * i) + l * (d * h - e * g)) / M;
    double beta = (i * (a * k - j * b) + h * (j * c - a * l) + g * (b * l - k * c)) / M;
    double tao = -(f * (a * k - j * b) + e * (j * c - a * l) + d * (b * l - k * c)) / M;
    glm::vec3 n = glm::cross(v - u, w - u);
    if (tao >= EPS && alpha >= 0.0 && beta >= 0.0 && (alpha + beta) <= 1.0) return new TAO(tao, true, n);
    return new TAO(0, false, glm::vec3(0));
}
