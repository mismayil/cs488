#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Material.hpp"
#include "PerlinNoise.hpp"
#include "util.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflectiveness, double refractiveness, double bumpness, double transparency);
  virtual ~PhongMaterial();
  virtual glm::vec3 getkd();
  glm::vec3 getks();
  double getShininess();
  double getReflectiveness();
  double getRefractiveness();
  double getBumpness();
  double getTransparency();
  glm::vec3 bump(glm::vec3 n, glm::vec3 point);
  PerlinNoise *getpn();
private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_reflectiveness;
  double m_refractiveness;
  double m_bumpness;
  double m_transparency;
  PerlinNoise *perlin;
};
