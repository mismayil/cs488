#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "polyroots.hpp"
#include "util.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual TAO *intersect(glm::vec3 eye, glm::vec3 ray);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  TAO *intersect(glm::vec3 eye, glm::vec3 ray);

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  virtual ~NonhierBox();
  TAO *intersect(glm::vec3 eye, glm::vec3 ray);

private:
  glm::vec3 m_pos;
  double m_size;
};

class Sphere : public Primitive {
  NonhierSphere *nsphere;
public:
  Sphere();
  virtual ~Sphere();
  TAO *intersect(glm::vec3 eye, glm::vec3 ray);
};

class Cube : public Primitive {
  NonhierBox *nbox;
public:
  Cube();
  virtual ~Cube();
  TAO *intersect(glm::vec3 eye, glm::vec3 ray);
};

class BoundedBox {
    double xmin, xmax, ymin, ymax, zmin, zmax;
public:
    BoundedBox(std::vector<glm::vec3> v);
    bool intersect(glm::vec3 eye, glm::vec3 ray);
};
