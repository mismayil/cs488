#include "util.hpp"
#include <iostream>

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
    if (!std::isnan(tao) && !std::isinf(tao) && tao >= EPS && alpha >= 0.0 && beta >= 0.0 && (alpha + beta) <= 1.0) return new TAO(tao, true, n);
    return new TAO();
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
    if (a > b) return false;
    if (a < b) return false;
    return true;
}

bool refract(glm::vec3 d, glm::vec3 n, double eta, glm::vec3 &t) {
	double tmp = 1 - pow(eta, 2) * (1 - pow(glm::dot(d, n), 2));
	if (tmp < 0) return false;
	t = normalize(eta * (d - n * glm::dot(d, n)) - n * sqrt(tmp));
	return true;
}

void progress(int &percent, uint x, uint y, size_t w, size_t h) {
    if (100 * (y+1) * (x+1) / ((h+1) * (w+1)) > (percent + 10)) {
        percent += 10;
        cout << "progress: " << percent << " %"<< endl;
    }
}
