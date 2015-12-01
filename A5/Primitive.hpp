#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "polyroots.hpp"
#include "util.hpp"
#include "Image.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual TAO *intersect(Ray ray);
  virtual int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);
};

class NonhierSphere : public Primitive {
    glm::vec3 m_pos;
    double m_radius;
public:
    NonhierSphere(const glm::vec3& pos, double radius);
    virtual ~NonhierSphere();
    TAO *intersect(Ray ray);
    int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);
};

class BoundedBox : public Primitive {
    double xmin, xmax, ymin, ymax, zmin, zmax;
public:
    BoundedBox(std::vector<glm::vec3> v);
    TAO *intersect(Ray ray);
};

class NonhierBox : public Primitive {
    BoundedBox *box;
    glm::vec3 m_pos;
    double m_size;
public:
    NonhierBox(const glm::vec3& pos, double size);
    virtual ~NonhierBox();
    TAO *intersect(Ray ray);
    int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);
};

class NonhierCylinder : public Primitive {
    glm::vec3 m_pos;
    double m_height;
    double m_radius;
public:
    NonhierCylinder(glm::vec3 pos, double height, double radius);
    virtual ~NonhierCylinder();
    TAO *intersect(Ray ray);
    //int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);
};

class NonhierCone : public Primitive {
    glm::vec3 m_apex;
    double m_angle;
    double m_height;
public:
    NonhierCone(glm::vec3 apex, double angle, double height);
    virtual ~NonhierCone();
    TAO *intersect(Ray ray);
    //int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);
};

class Sphere : public Primitive {
    NonhierSphere *nsphere;
public:
    Sphere();
    virtual ~Sphere();
    TAO *intersect(Ray ray);
    int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);
};

class Cube : public Primitive {
    NonhierBox *nbox;
public:
    Cube();
    virtual ~Cube();
    TAO *intersect(Ray ray);
    int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);
};

class Cylinder : public Primitive {
    NonhierCylinder *ncylinder;
public:
    Cylinder();
    virtual ~Cylinder();
    TAO *intersect(Ray ray);
    //int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);
};

class Cone : public Primitive {
    NonhierCone *ncone;
public:
    Cone();
    virtual ~Cone();
    TAO *intersect(Ray ray);
    //int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);
};
