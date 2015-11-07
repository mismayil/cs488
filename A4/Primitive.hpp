#pragma once

#include <glm/glm.hpp>
#include "polyroots.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual double intersect(glm::vec3 eye, glm::vec3 ray);
  virtual glm::vec3 getNormal(glm::vec3 point);
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
  double intersect(glm::vec3 eye, glm::vec3 ray);
  glm::vec3 getNormal(glm::vec3 point);

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

private:
  glm::vec3 m_pos;
  double m_size;
};
