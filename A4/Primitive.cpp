#include "Primitive.hpp"
#include <iostream>

Primitive::~Primitive()
{
}

double Primitive::intersect(glm::vec3 eye, glm::vec3 ray) {
    return 0;
}

glm::vec3 Primitive::getNormal(glm::vec3 point) {
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

double NonhierSphere::intersect(glm::vec3 eye, glm::vec3 ray) {
    double tao[2];
    double A = glm::dot(ray, ray);
    double B = glm::dot(2.0f * (eye - m_pos), ray);
    double C = glm::dot(eye - m_pos, eye - m_pos) - m_radius * m_radius;
    size_t res = quadraticRoots(A, B, C, tao);
    if (res == 0) return -1;
    if (res == 1) return tao[0];
    return tao[0] < tao[1] ? tao[0] : tao[1];
}

glm::vec3 NonhierSphere::getNormal(glm::vec3 point) {
    return glm::normalize(point - m_pos);
}

NonhierBox::~NonhierBox()
{
}
