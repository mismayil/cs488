#pragma once

#include <math.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "TAO.hpp"
#include "Ray.hpp"
#include "PerlinNoise.hpp"

#define EPS 1e-6
#define SAMPLE 3
#define MAX_DEPTH 5
#define AIR_REF_INDEX 1.000293
#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)
#define RAD(a) (a * M_PI / 180)

class Material;

struct plane {
    double A, B, C, D;
    plane(double A, double B, double C, double D) : A(A), B(B), C(C), D(D) {}
};

plane getplane(glm::vec3 a, glm::vec3 b, glm::vec3 c);
double intersectPLane(struct plane plane, Ray ray);
TAO *intersectTriangle(Ray ray, glm::vec3 a, glm::vec3 b, glm::vec3 c);
void print(glm::vec3 v);
glm::vec3 normalize(glm::vec3 v);
bool eq(double a, double b);
bool refract(glm::vec3 d, glm::vec3 n, double eta, glm::vec3 &t);
