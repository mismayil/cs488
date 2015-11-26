#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflectiveness, double refractiveness);
  virtual ~PhongMaterial();
  glm::vec3 getkd();
  glm::vec3 getks();
  double getShininess();
  double getReflectiveness();
  double getRefractiveness();
private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_reflectiveness;
  double m_refractiveness;
};
