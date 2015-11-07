#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{}

PhongMaterial::~PhongMaterial()
{}

glm::vec3 PhongMaterial::getkd() {
	return m_kd;
}

glm::vec3 PhongMaterial::getks() {
	return m_ks;
}

double PhongMaterial::getsh() {
	return m_shininess;
}
