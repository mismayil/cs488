#pragma once

#include <glm/glm.hpp>
#include "polyroots.hpp"

// intersection info
struct TAO {
    double tao;
    bool hit;
    glm::vec3 n;
};

class Primitive {
public:
  virtual ~Primitive();
  virtual TAO intersect(glm::vec3 eye, glm::vec3 ray);
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  TAO intersect(glm::vec3 eye, glm::vec3 ray);

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
  TAO intersect(glm::vec3 eye, glm::vec3 ray);

private:
  glm::vec3 m_pos;
  double m_size;
};
