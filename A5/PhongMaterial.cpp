#include "PhongMaterial.hpp"
#include <iostream>

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflectiveness, double refractiveness, double transparency, double bumpness)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_reflectiveness(reflectiveness)
	, m_refractiveness(refractiveness)
	, m_transparency(transparency)
	, m_bumpness(bumpness)
	, perlin(new PerlinNoise())
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

double PhongMaterial::getBumpness() {
	return m_bumpness;
}

double PhongMaterial::getTransparency() {
	return m_transparency;
}

glm::vec3 PhongMaterial::bump(glm::vec3 n, glm::vec3 point) {
	if (m_bumpness > 0) {
		glm::vec3 nn, pp;
		pp = 0.1f * point;
		double noiseCoefx = perlin->noise(pp.x, pp.y, pp.z);
		double noiseCoefy = perlin->noise(pp.y, pp.z, pp.x);
		double noiseCoefz = perlin->noise(pp.z, pp.x, pp.y);
		nn.x = (1.0f - m_bumpness) * nn.x + m_bumpness * noiseCoefx;
		nn.y = (1.0f - m_bumpness) * nn.y + m_bumpness * noiseCoefy;
		nn.z = (1.0f - m_bumpness) * nn.z + m_bumpness * noiseCoefz;
		double tmp = glm::dot(nn, nn);
		if (eq(tmp, 0.0)) return nn;
		tmp = 1.0 / sqrt(tmp);
		return (float) tmp + nn;
	}

	return n;
}

PerlinNoise* PhongMaterial::getpn() {
	return perlin;
}
