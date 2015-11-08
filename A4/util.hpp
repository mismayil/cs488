#pragma once

#include <math.h>
#include <glm/glm.hpp>

#define EPS 0.001
#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)
#define RAD(a) (a * M_PI / 180)

// intersection info
class TAO {
    public:
        double tao;
        bool hit;
        glm::vec3 n;
        TAO(double tao, bool hit, glm::vec3 n) : tao(tao), hit(hit), n(n) {}
};

struct plane {
    double A, B, C, D;
    plane(double A, double B, double C, double D) : A(A), B(B), C(C), D(D) {}
};

struct slab {
    plane p1, p2;
    glm::vec3 n1;
    glm::vec3 n2;
    slab(plane p1, plane p2, glm::vec3 n1, glm::vec3 n2) : p1(p1), p2(p2), n1(n1), n2(n2) {}
};

plane getplane(glm::vec3 a, glm::vec3 b, glm::vec3 c);
double intersectPLane(struct plane plane, glm::vec3 eye, glm::vec3 ray);
TAO *intersectTriangle(glm::vec3 eye, glm::vec3 ray, glm::vec3 a, glm::vec3 b, glm::vec3 c);
