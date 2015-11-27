#pragma once

#include <math.h>
#include <glm/glm.hpp>

#define EPS 0.00001
#define SAMPLE 3
#define MAX_DEPTH 5
#define AIR_REF_INDEX 1.000293
#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)
#define RAD(a) (a * M_PI / 180)

class SceneNode;

// intersection info
class TAO {
    public:
        double tao;
        bool hit;
        glm::vec3 n;
        SceneNode *node;
        TAO() : tao(0), hit(false), n(glm::vec3(0)), node(NULL) {}
        TAO(double tao, bool hit, glm::vec3 n) : tao(tao), hit(hit), n(n), node(NULL) {}
};

class RAY {
    public:
        glm::vec3 p;
        glm::vec3 v;
        RAY(glm::vec3 p, glm::vec3 v) : p(p), v(v) {}
};

struct plane {
    double A, B, C, D;
    plane(double A, double B, double C, double D) : A(A), B(B), C(C), D(D) {}
};

plane getplane(glm::vec3 a, glm::vec3 b, glm::vec3 c);
double intersectPLane(struct plane plane, glm::vec3 eye, glm::vec3 ray);
TAO *intersectTriangle(glm::vec3 eye, glm::vec3 ray, glm::vec3 a, glm::vec3 b, glm::vec3 c);
void print(glm::vec3 v);
glm::vec3 normalize(glm::vec3 v);
