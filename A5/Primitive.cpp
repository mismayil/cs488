#include "Primitive.hpp"
#include <vector>
#include <iostream>

using namespace std;

Primitive::~Primitive()
{
}

TAO* Primitive::intersect(glm::vec3 eye, glm::vec3 ray) {
    return new TAO();
}

void Primitive::mapuv(glm::vec3 point, Image *texture, int uv[2]) {}

Sphere::Sphere() {
    nsphere = new NonhierSphere(glm::vec3(0), 1.0);
}

Sphere::~Sphere()
{
}

TAO* Sphere::intersect(glm::vec3 eye, glm::vec3 ray) {
    return nsphere->intersect(eye, ray);
}

Cube::Cube() {
    nbox = new NonhierBox(glm::vec3(0), 1.0);
}

TAO* Cube::intersect(glm::vec3 eye, glm::vec3 ray) {
    return nbox->intersect(eye, ray);
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

TAO* NonhierSphere::intersect(glm::vec3 eye, glm::vec3 ray) {
    double tao[2];
    double A = glm::dot(ray, ray);
    double B = glm::dot(2.0f * (eye - m_pos), ray);
    double C = glm::dot(eye - m_pos, eye - m_pos) - m_radius * m_radius;
    size_t res = quadraticRoots(A, B, C, tao);
    if (res == 0) return new TAO();
    if (res == 1) return new TAO(tao[0], std::isnan(tao[0]) || std::isinf(tao[0]) || tao[0] < 0 ? false : true, (eye + (float) tao[0] * ray) - m_pos);
    double mintao = MIN(tao[0], tao[1]);
    return new TAO(mintao, std::isnan(mintao) || std::isinf(mintao) || mintao < 0 ? false : true, (eye + (float) mintao * ray) - m_pos);
}

void NonhierSphere::mapuv(glm::vec3 point, Image *texture, int uv[2]) {
    glm::vec3 vn = normalize(glm::vec3(0, m_radius, 0));
    glm::vec3 ve = normalize(glm::vec3(m_radius, 0, 0));
    glm::vec3 vp = normalize(point - m_pos);
    double phi = acos(MAX(MIN(-glm::dot(vn, vp), 1), -1));
    double v = phi / M_PI;
    double theta = acos(MAX(MIN(glm::dot(vp, ve) / glm::sin(phi), 1), -1)) / (2.0 * M_PI);
    double u;
    if (glm::dot(glm::cross(vn, ve), vp) > 0) u = theta;
    else u = 1 - theta;
    uv[0] = (int)(u * texture->width());
    uv[1] = (int)(v * texture->height());
}

NonhierBox::~NonhierBox()
{
}

TAO* NonhierBox::intersect(glm::vec3 eye, glm::vec3 ray) {
    return box->intersect(eye, ray);
}


BoundedBox::BoundedBox(vector<glm::vec3> v) {
    xmin = v[0].x, ymin = v[0].y, zmin = v[0].z;
    xmax = v[0].x, ymax = v[0].y, zmax = v[0].z;

    for (int i = 0; i < v.size(); i++) {
        if (v[i].x < xmin) xmin = v[i].x;
        if (v[i].y < ymin) ymin = v[i].y;
        if (v[i].z < zmin) zmin = v[i].z;
        if (v[i].x > xmax) xmax = v[i].x;
        if (v[i].y > ymax) ymax = v[i].y;
        if (v[i].z > zmax) zmax = v[i].z;
    }
}

TAO* BoundedBox::intersect(glm::vec3 eye, glm::vec3 ray) {
    double ax = 1.0 / ray.x;
    double ay = 1.0 / ray.y;
    double az = 1.0 / ray.z;

    double txmin, txmax, tymin, tymax, tzmin, tzmax;

    if (ax >= 0) {
        txmin = ax * (xmin - eye.x);
        txmax = ax * (xmax - eye.x);
    } else {
        txmin = ax * (xmax - eye.x);
        txmax = ax * (xmin - eye.x);
    }

    if (ay >= 0) {
        tymin = ay * (ymin - eye.y);
        tymax = ay * (ymax - eye.y);
    } else {
        tymin = ay * (ymax - eye.y);
        tymax = ay * (ymin - eye.y);
    }

    if (az >= 0) {
        tzmin = az * (zmin - eye.z);
        tzmax = az * (zmax - eye.z);
    } else {
        tzmin = az * (zmax - eye.z);
        tzmax = az * (zmin - eye.z);
    }

    double M = MIN(MIN(txmax, tymax), MIN(tymax, tzmax));
    double m = MAX(MAX(txmin, tymin), MAX(tymin, tzmin));

    glm::vec3 n;

    if (m > txmin - EPS && m < txmin + EPS) n = glm::vec3(1, 0, 0);
    if (m > txmax - EPS && m < txmax + EPS) n = glm::vec3(-1, 0, 0);
    if (m > tymin - EPS && m < tymin + EPS) n = glm::vec3(0, 1, 0);
    if (m > tymax - EPS && m < tymax + EPS) n = glm::vec3(0, -1, 0);
    if (m > tzmin - EPS && m < tzmin + EPS) n = glm::vec3(0, 0, 1);
    if (m > tzmax - EPS && m < tzmax + EPS) n = glm::vec3(0, 0, -1);

    if (std::isnan(m) || std::isinf(m) || m > M || m < EPS) return new TAO();

    return new TAO(m, true, n);
}
