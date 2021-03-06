#pragma once

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "TAO.hpp"
#include "Ray.hpp"
#include "PerlinNoise.hpp"

#define EPS 1e-4
#define THRESHOLD 0.1
#define SAMPLE 3
#define SHADOW_SAMPLE 3
#define DOF_SAMPLE 3
#define MAX_DEPTH 5
#define ADAPTIVE_DEPTH 1
#define AIR_REF_INDEX 1
#define NUM_THREADS 4
#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)
#define RAD(a) (a * M_PI / 180)

#undef ADAPTIVE_FALSE_COLOR

enum OPERATION {
    UNION,
    INTERSECTION,
    DIFFERENCE
};

class Material;

struct plane {
    double A, B, C, D;
    plane(double A, double B, double C, double D) : A(A), B(B), C(C), D(D) {}
};

struct pixel {
    double x, y;
    double offset;
};

struct DOF {
    double focal;
    double aperture;
    DOF(double focal, double aperture);
    std::vector<Ray> getRays(double nx, double ny, glm::vec3 eye, glm::vec3 view, glm::vec3 up, glm::vec3 left, glm::vec3 direction, size_t w, size_t h);
};

plane getplane(glm::vec3 a, glm::vec3 b, glm::vec3 c);
double intersectPLane(struct plane plane, Ray ray);
TAO *intersectTriangle(Ray ray, glm::vec3 a, glm::vec3 b, glm::vec3 c);
void print(glm::vec3 v);
glm::vec3 normalize(glm::vec3 v);
bool eq(double a, double b);
bool refract(glm::vec3 d, glm::vec3 n, double eta, glm::vec3 &t);
void progress(int id, int &percent, uint x, uint y, size_t w, size_t h);
double random(double min, double max);
