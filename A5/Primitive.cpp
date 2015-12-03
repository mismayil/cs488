#include "Primitive.hpp"
#include <vector>
#include <iostream>

using namespace std;

Primitive::~Primitive() {}

TAO* Primitive::intersect(Ray ray) {
    return new TAO();
}

int* Primitive::mapuv(glm::vec3 point, glm::vec3 n, Image *texture) {}

Sphere::Sphere() {
    nsphere = new NonhierSphere(glm::vec3(0), 1.0);
}

Sphere::~Sphere() {
    delete nsphere;
}

TAO* Sphere::intersect(Ray ray) {
    return nsphere->intersect(ray);
}

int* Sphere::mapuv(glm::vec3 point, glm::vec3 n, Image *texture) {
    return nsphere->mapuv(point, n, texture);
}

Cube::Cube() {
    nbox = new NonhierBox(glm::vec3(0), 1.0);
}

TAO* Cube::intersect(Ray ray) {
    return nbox->intersect(ray);
}

int* Cube::mapuv(glm::vec3 point, glm::vec3 n, Image *texture) {
    return nbox->mapuv(point, n, texture);
}

Cube::~Cube() {
    delete nbox;
}

Cylinder::Cylinder() {
    ncylinder = new NonhierCylinder(glm::vec3(0), 1, 1);
}

Cylinder::~Cylinder() {
    delete ncylinder;
}

TAO* Cylinder::intersect(Ray ray) {
    return ncylinder->intersect(ray);
}

Cone::Cone() {
    ncone = new NonhierCone(glm::vec3(0), 30, 1);
}

Cone::~Cone() {
    delete ncone;
}

TAO* Cone::intersect(Ray ray) {
    return ncone->intersect(ray);
}

NonhierSphere::NonhierSphere(const glm::vec3& pos, double radius) : m_pos(pos), m_radius(radius) {}

NonhierSphere::~NonhierSphere() {}

TAO* NonhierSphere::intersect(Ray ray) {
    double tao[2];
    double A = glm::dot(ray.d, ray.d);
    double B = glm::dot(2.0f * (ray.o - m_pos), ray.d);
    double C = glm::dot(ray.o - m_pos, ray.o - m_pos) - m_radius * m_radius;
    size_t res = quadraticRoots(A, B, C, tao);
    glm::vec3 nmin, nmax;

    if (res == 0) return NULL;

    if (res == 1) {
        bool hit = (std::isnan(tao[0]) || std::isinf(tao[0]) || tao[0] < 0) ? false : true;
        if (!hit) return NULL;
        nmin = nmax = (ray.o + (float) tao[0] * ray.d) - m_pos;
        return new TAO(tao[0], tao[0], nmin, nmax);
    }

    double taomin = MIN(tao[0], tao[1]);
    double taomax = MAX(tao[0], tao[1]);
    bool hit = (std::isnan(taomin) || std::isinf(taomin) || taomin < 0) ? false : true;
    if (!hit) return NULL;
    nmin = (ray.o + (float) taomin * ray.d) - m_pos;
    nmax = (ray.o + (float) taomax * ray.d) - m_pos;
    return new TAO(taomin, taomax, nmin, nmax);
}

int* NonhierSphere::mapuv(glm::vec3 point, glm::vec3 n, Image *texture) {
    glm::vec3 vn = normalize(glm::vec3(0, m_radius, 0));
    glm::vec3 ve = normalize(glm::vec3(-m_radius, 0, 0));
    glm::vec3 vp = normalize(point - m_pos);
    double phi = acos(MAX(MIN(-glm::dot(vn, vp), 1), -1));
    double v = phi / M_PI;
    double theta = acos(MAX(MIN(glm::dot(vp, ve) / glm::sin(phi), 1), -1)) / (2.0 * M_PI);
    double u;
    if (glm::dot(glm::cross(vn, ve), vp) > 0) u = theta;
    else u = 1 - theta;
    int *uv = new int[2];
    uv[0] = (int)(u * texture->width());
    uv[1] = (int)(v * texture->height());
    return uv;
}

NonhierBox::NonhierBox(const glm::vec3& pos, double size) : m_pos(pos), m_size(size) {
    std::vector<glm::vec3> v;   // box vertices

    for (float i = m_pos.x; i < m_pos.x + 2 * m_size; i += m_size) {
        for (float j = m_pos.y; j < m_pos.y + 2 * m_size; j += m_size) {
            for (float k = m_pos.z; k < m_pos.z + 2 * m_size; k += m_size) {
                v.push_back(glm::vec3(i, j, k));
            }
        }
    }

    box = new BoundedBox(v);
}

NonhierBox::~NonhierBox() {}

TAO* NonhierBox::intersect(Ray ray) {
    return box->intersect(ray);
}

int* NonhierBox::mapuv(glm::vec3 point, glm::vec3 n, Image *texture) {
    glm::vec3 ll;
    glm::vec3 lp;
    int *uv = new int[2];

    if (n == glm::vec3(-1, 0, 0)) {
        ll = m_pos;
        lp = point - ll;
        uv[0] = (int) (abs(lp.z / m_size) * texture->width());
        uv[1] = (int) (abs(lp.y / m_size) * texture->height());
    }

    if (n == glm::vec3(1, 0, 0)) {
        ll = glm::vec3(m_pos.x + m_size, m_pos.y, m_pos.z + m_size);
        lp = point - ll;
        uv[0] = (int) (abs(lp.z / m_size) * texture->width());
        uv[1] = (int) (abs(lp.y / m_size) * texture->height());
    }

    if (n == glm::vec3(0, -1, 0)) {
        ll = m_pos;
        lp = point - ll;
        uv[0] = (int) (abs(lp.x / m_size) * texture->width());
        uv[1] = (int) (abs(lp.z / m_size) * texture->height());
    }

    if (n == glm::vec3(0, 1, 0)) {
        ll = glm::vec3(m_pos.x, m_pos.y + m_size, m_pos.z + m_size);
        lp = point - ll;
        uv[0] = (int) (abs(lp.x / m_size) * texture->width());
        uv[1] = (int) (abs(lp.z / m_size) * texture->height());
    }

    if (n == glm::vec3(0, 0, -1)) {
        ll = glm::vec3(m_pos.x + m_size, m_pos.y, m_pos.z);
        lp = point - ll;
        uv[0] = (int) (abs(lp.x / m_size) * texture->width());
        uv[1] = (int) (abs(lp.y / m_size) * texture->height());
    }

    if (n == glm::vec3(0, 0, 1)) {
        ll = glm::vec3(m_pos.x, m_pos.y, m_pos.z + m_size);
        lp = point - ll;
        uv[0] = (int) (abs(lp.x / m_size) * texture->width());
        uv[1] = (int) (abs(lp.y / m_size) * texture->height());
    }

    return uv;
}

NonhierCylinder::NonhierCylinder(glm::vec3 pos, double height, double radius) : m_pos(pos), m_height(height), m_radius(radius) {}

NonhierCylinder::~NonhierCylinder() {}

TAO* NonhierCylinder::intersect(Ray ray) {

    struct tn {
        double t;
        glm::vec3 n;
    };

    std::vector<tn> taos;
    glm::vec3 dp = ray.o - m_pos;
    glm::vec3 p1 = m_pos;
    glm::vec3 p2 = p1 + glm::vec3(0, m_height, 0);
    glm::vec3 up = normalize(p2 - p1);

    glm::vec3 tmp1 = ray.d - glm::dot(ray.d, up) * up;
    glm::vec3 tmp2 = dp - glm::dot(dp, up) * up;
    double A = glm::dot(tmp1, tmp1);
    double B = 2 * glm::dot(tmp1, tmp2);
    double C = glm::dot(tmp2, tmp2) - pow(m_radius, 2);
    double tao[2];
    tao[0] = -1;
    tao[1] = -1;

    size_t res = quadraticRoots(A, B, C, tao);

    if (res != 0) {
        if (tao[0] >= 0) {
            glm::vec3 q = ray.o + tao[0] * ray.d;
            if (glm::dot(up, q - p1) > 0 && glm::dot(up, q - p2) < 0) taos.push_back({tao[0], glm::vec3(q.x - p1.x, 0, q.z - p1.z)});
        }
        if (tao[1] >= 0) {
            glm::vec3 q = ray.o + tao[1] * ray.d;
            if (glm::dot(up, q - p1) > 0 && glm::dot(up, q - p2) < 0) taos.push_back({tao[1], glm::vec3(q.x - p1.x, 0, q.z - p1.z)});
        }
    }

    if (!eq(ray.d.y, 0.0)) {
        tao[0] = (p1.y - ray.o.y) / ray.d.y;
        tao[1] = (p2.y - ray.o.y) / ray.d.y;

        if (tao[0] >= 0) {
            glm::vec3 q = ray.o + tao[0] * ray.d;
            if (glm::dot(q - p1, q - p1) < pow(m_radius, 2)) taos.push_back({tao[0], glm::vec3(0, 1, 0)});
        }
        if (tao[1] >= 0) {
            glm::vec3 q = ray.o + tao[1] * ray.d;
            if (glm::dot(q - p2, q - p2) < pow(m_radius, 2)) taos.push_back({tao[1], glm::vec3(0, -1, 0)});
        }
    }

    if (taos.size() == 0) return NULL;

    tn taomin = taos[0];
    tn taomax = taos[0];

    for (int i = 0; i < taos.size(); i++) {
        if (taos[i].t < taomin.t) taomin = taos[i];
        if (taos[i].t > taomax.t) taomax = taos[i];
    }

    return new TAO(taomin.t, taomax.t, taomin.n, taomax.n);
}

NonhierCone::NonhierCone(glm::vec3 apex, double angle, double height) : m_apex(apex), m_angle(angle), m_height(height) {}

NonhierCone::~NonhierCone() {}

TAO* NonhierCone::intersect(Ray ray) {

    struct tn {
        double t;
        glm::vec3 n;
    };

    std::vector<tn> taos;
    glm::vec3 dp = ray.o - m_apex;
    glm::vec3 p = m_apex - glm::vec3(0, m_height, 0);
    glm::vec3 down = normalize(p - m_apex);
    double alpha = RAD(m_angle);
    double radius = m_height * tan(alpha);

    glm::vec3 tmp = ray.d - glm::dot(ray.d, down) * down;
    double A = pow(cos(alpha), 2) * glm::dot(tmp, tmp) - pow(sin(alpha), 2) * glm::dot(glm::dot(ray.d, down), glm::dot(ray.d, down));
    double B = 2 * pow(cos(alpha), 2) * glm::dot(tmp, dp - glm::dot(dp, down) * down) - 2 * pow(sin(alpha), 2) * glm::dot(glm::dot(ray.d, down), glm::dot(dp, down));
    tmp = dp - glm::dot(dp, down) * down;
    double C = pow(cos(alpha), 2) * glm::dot(tmp, tmp) - pow(sin(alpha), 2) * glm::dot(glm::dot(dp, down), glm::dot(dp, down));
    double tao[2];
    tao[0] = -1;
    tao[1] = -1;

    size_t res = quadraticRoots(A, B, C, tao);

    if (res != 0) {
        if (tao[0] >= 0) {
            glm::vec3 q = ray.o + tao[0] * ray.d;
            if (glm::dot(down, q - m_apex) > 0 && glm::dot(down, q - p) < 0) taos.push_back({tao[0], glm::vec3(q.x - p.x, 0, q.z - p.z)});
        }
        if (tao[1] >= 0) {
            glm::vec3 q = ray.o + tao[1] * ray.d;
            if (glm::dot(down, q - m_apex) > 0 && glm::dot(down, q - p) < 0) taos.push_back({tao[1], glm::vec3(q.x - p.x, 0, q.z - p.z)});
        }
    }

    if (!eq(ray.d.y, 0.0)) {
        tao[0] = (p.y - ray.o.y) / ray.d.y;

        if (tao[0] >= 0) {
            glm::vec3 q = ray.o + tao[0] * ray.d;
            if (glm::dot(q - p, q - p) < pow(radius, 2)) taos.push_back({tao[0], glm::vec3(0, -1, 0)});
        }
    }

    if (taos.size() == 0) return NULL;

    tn taomin = taos[0];
    tn taomax = taos[0];

    for (int i = 0; i < taos.size(); i++) {
        if (taos[i].t < taomin.t) taomin = taos[i];
        if (taos[i].t > taomax.t) taomax = taos[i];
    }

    return new TAO(taomin.t, taomax.t, taomin.n, taomax.n);
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

TAO* BoundedBox::intersect(Ray ray) {
    double ax = 1.0 / ray.d.x;
    double ay = 1.0 / ray.d.y;
    double az = 1.0 / ray.d.z;

    double txmin, txmax, tymin, tymax, tzmin, tzmax;

    if (ax >= 0) {
        txmin = ax * (xmin - ray.o.x);
        txmax = ax * (xmax - ray.o.x);
    } else {
        txmin = ax * (xmax - ray.o.x);
        txmax = ax * (xmin - ray.o.x);
    }

    if (ay >= 0) {
        tymin = ay * (ymin - ray.o.y);
        tymax = ay * (ymax - ray.o.y);
    } else {
        tymin = ay * (ymax - ray.o.y);
        tymax = ay * (ymin - ray.o.y);
    }

    if (az >= 0) {
        tzmin = az * (zmin - ray.o.z);
        tzmax = az * (zmax - ray.o.z);
    } else {
        tzmin = az * (zmax - ray.o.z);
        tzmax = az * (zmin - ray.o.z);
    }

    double M = MIN(MIN(txmax, tymax), MIN(tymax, tzmax));
    double m = MAX(MAX(txmin, tymin), MAX(tymin, tzmin));

    glm::vec3 nmin, nmax;

    if (eq(m, txmin)) nmin = glm::vec3(1, 0, 0);
    if (eq(m, txmax)) nmin = glm::vec3(-1, 0, 0);
    if (eq(m, tymin)) nmin = glm::vec3(0, 1, 0);
    if (eq(m, tymax)) nmin = glm::vec3(0, -1, 0);
    if (eq(m, tzmin)) nmin = glm::vec3(0, 0, 1);
    if (eq(m, tzmax)) nmin = glm::vec3(0, 0, -1);

    if (eq(M, txmin)) nmax = glm::vec3(1, 0, 0);
    if (eq(M, txmax)) nmax = glm::vec3(-1, 0, 0);
    if (eq(M, tymin)) nmax = glm::vec3(0, 1, 0);
    if (eq(M, tymax)) nmax = glm::vec3(0, -1, 0);
    if (eq(M, tzmin)) nmax = glm::vec3(0, 0, 1);
    if (eq(M, tzmax)) nmax = glm::vec3(0, 0, -1);

    if (std::isnan(m) || std::isinf(m) || m > M || m < EPS) return NULL;

    return new TAO(m, M, nmin, nmax);
}
