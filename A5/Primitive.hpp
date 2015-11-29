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
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  TAO *intersect(Ray ray);
  int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);

private:
  glm::vec3 m_pos;
  double m_radius;
};

class BoundedBox : public Primitive {
    double xmin, xmax, ymin, ymax, zmin, zmax;
public:
    BoundedBox(std::vector<glm::vec3> v);
    TAO *intersect(Ray ray);
};

class NonhierBox : public Primitive {
    BoundedBox *box;
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
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
  virtual ~NonhierBox();
  TAO *intersect(Ray ray);
  int *mapuv(glm::vec3 point, glm::vec3 n, Image *texture);

private:
  glm::vec3 m_pos;
  double m_size;
};

class Sphere : public Primitive {
  NonhierSphere *nsphere;
public:
  Sphere();
  virtual ~Sphere();
  TAO *intersect(Ray ray);
};

class Cube : public Primitive {
  NonhierBox *nbox;
public:
  Cube();
  virtual ~Cube();
  TAO *intersect(Ray ray);
};
