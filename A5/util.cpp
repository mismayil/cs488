#include "util.hpp"
#include <iostream>
#include <thread>

using namespace std;

plane getplane(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;

    glm::vec3 abxac = glm::cross(ab, ac);
    double d = -abxac.x * a.x - abxac.y * a.y - abxac.z * a.z;
    return plane(abxac.x, abxac.y, abxac.z, d);
}

double intersectPLane(struct plane plane, Ray ray) {
    return -(plane.A * ray.o.x + plane.B * ray.o.y + plane.C * ray.o.z + plane.D) / (plane.A * ray.d.x + plane.B * ray.d.y + plane.C * ray.d.z);
}

DOF::DOF(double focal, double aperture) : focal(focal), aperture(aperture) {}

vector<Ray> DOF::getRays(double nx, double ny, glm::vec3 eye, glm::vec3 view, glm::vec3 up, glm::vec3 left, glm::vec3 direction, size_t w, size_t h) {
    double dEyeImage = 1;
    double dEyePixel = glm::length(direction);
    glm::vec3 fp = eye + ((float)dEyePixel * (float)(dEyeImage + focal) / (float)dEyeImage) * normalize(direction);
    if (ny - aperture / 2 > 0) ny -= aperture / 2;
    if (nx - aperture / 2 > 0) nx -= aperture / 2;
    glm::vec3 c = glm::vec3(0);
    vector<Ray> frays;

    for (int a = 0; a < SAMPLE; a++) {
        for (int b = 0; b < SAMPLE; b++) {
            double npx = random(nx + a * aperture / DOF_SAMPLE, nx + (a + 1) * aperture / DOF_SAMPLE);
            double npy = random(ny + b * aperture / DOF_SAMPLE, ny + (b + 1) * aperture / DOF_SAMPLE);
            glm::vec3 d = view + (-1 + 2 * (float)npy / h) * up + (-1 + 2 * (float)npx / w) * left;
            glm::vec3 pxl = eye + d;
            Ray fray(pxl, normalize(fp - pxl));
            frays.push_back(fray);
        }
    }

    return frays;
}

TAO *intersectTriangle(Ray ray, glm::vec3 u, glm::vec3 v, glm::vec3 w) {
    double a = u.x - v.x;
    double b = u.y - v.y;
    double c = u.z - v.z;
    double d = u.x - w.x;
    double e = u.y - w.y;
    double f = u.z - w.z;
    double g = ray.d.x;
    double h = ray.d.y;
    double i = ray.d.z;
    double j = u.x - ray.o.x;
    double k = u.y - ray.o.y;
    double l = u.z - ray.o.z;
    double M = a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);
    double alpha = (j * (e *  i - h * f) + k * (g * f - d * i) + l * (d * h - e * g)) / M;
    double beta = (i * (a * k - j * b) + h * (j * c - a * l) + g * (b * l - k * c)) / M;
    double tao = -(f * (a * k - j * b) + e * (j * c - a * l) + d * (b * l - k * c)) / M;
    glm::vec3 n = glm::cross(v - u, w - u);
    if (!std::isnan(tao) && !std::isinf(tao) && tao >= EPS && alpha >= 0.0 && beta >= 0.0 && (alpha + beta) <= 1.0) return new TAO(tao, tao, n, n);
    return NULL;
}

void print(glm::vec3 v) {
    cout << "[" << v.x << " " << v.y << " " << v.z << "]" << endl;
}

glm::vec3 normalize(glm::vec3 v) {
    glm::vec3 nv = glm::normalize(v);
    if (std::isnan(nv.x)) return v;
    return nv;
}

bool eq(double a, double b) {
    return fabs(a - b) < DBL_EPSILON;
}

bool refract(glm::vec3 d, glm::vec3 n, double eta, glm::vec3 &t) {
	double tmp = 1 - pow(eta, 2) * (1 - pow(glm::dot(d, n), 2));
	if (tmp < 0) return false;
	t = normalize(eta * (d - n * glm::dot(d, n)) - n * sqrt(tmp));
	return true;
}

void progress(int id, int &percent, uint x, uint y, size_t w, size_t h) {
    if (100 * (y+1) * (x+1) / ((h+1) * (w+1)) > (percent + 10)) {
        percent += 10;
        cout << "[thread " << id << "] : " << percent << " %"<< endl;
    }
}

double random(double min, double max) {
    srand(time(NULL));
    return min + (rand() / (double) (RAND_MAX) * (max - min));
}
