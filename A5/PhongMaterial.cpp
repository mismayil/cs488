#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflectiveness, double refractiveness)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_reflectiveness(reflectiveness)
	, m_refractiveness(refractiveness)
	, p(new PerlinNoise())
{}

PhongMaterial::~PhongMaterial()
{}

glm::vec3 PhongMaterial::getkd() {
	return m_kd;
}

glm::vec3 PhongMaterial::getks() {
	return m_ks;
}

double PhongMaterial::getShininess() {
	return m_shininess;
}

double PhongMaterial::getReflectiveness() {
	return m_reflectiveness;
}

double PhongMaterial::getRefractiveness() {
	return m_refractiveness;
}

PerlinNoise* PhongMaterial::getpn() {
	return p;
}
