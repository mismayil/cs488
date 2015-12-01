#include <glm/ext.hpp>
#include "A5.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "TextureMaterial.hpp"
#include "TAO.hpp"
#include "Ray.hpp"
#include "util.hpp"

using namespace std;

glm::vec3 trace(SceneNode *root, Ray ray, list<Light *> &lights, const glm::vec3 &ambient, int depth) {
	glm::vec3 colour = glm::vec3(0);

	if (depth == MAX_DEPTH) return colour;

	TAO *ptao = root->intersect(ray);

	if (!ptao) return colour;

	glm::vec3 point = ray.o + ptao->tao * ray.d;
	PhongMaterial *pmaterial = static_cast<PhongMaterial *>(ptao->material);
	glm::vec3 normal = normalize(pmaterial->bump(ptao->n, point));
	glm::vec3 kd = pmaterial->getkd();
	glm::vec3 ks = pmaterial->getks();
	double shininess = pmaterial->getShininess();
	double reflectiveness = pmaterial->getReflectiveness();
	double refractiveness = pmaterial->getRefractiveness();

	for (Light *light : lights) {
		Ray lightRay(light->position, normalize(light->position - point));
		Ray shadowRay(point, (EPS + ptao->tao) * lightRay.d);

		TAO *stao = root->intersect(shadowRay);

		if (!stao || !stao->hit) {
			Ray reflectionRay(point, normalize(-lightRay.d + 2.0f * glm::dot(lightRay.d, normal) * normal));
			double distance = glm::length(lightRay.o - point);
			glm::vec3 intensity = light->colour / (float) (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance);
			glm::vec3 diffuse = kd * (float) MAX(glm::dot(lightRay.d, normal), 0.0) * intensity;
			glm::vec3 specular =  ks * pow((float) MAX(glm::dot(reflectionRay.d, normalize(ray.o - point)), 0.0), shininess) * intensity;
			colour += diffuse + specular;
		}
	}

	Ray viewRay(ray.o, normalize(point - ray.o));
	glm::vec3 reflection = glm::vec3(0);
	glm::vec3 refraction = glm::vec3(0);
	colour += kd * ambient;

	Ray reflectionRay(point, (EPS + ptao->tao) * normalize(viewRay.d - 2 * glm::dot(viewRay.d, normal) * normal));
	if (reflectiveness > 0) reflection = ks * trace(root, reflectionRay, lights, ambient, depth + 1) * reflectiveness;

	if (refractiveness > 1 || refractiveness < 1) {
		double costheta = 0;
		double eta = AIR_REF_INDEX / refractiveness;
		glm::vec3 t;

		if (glm::dot(viewRay.d, normal) < 0) {
			if (!refract(viewRay.d, normal, eta, t)) return colour + reflection;
			Ray refractionRay(point, (EPS + ptao->tao) * t);
			refraction = ks * trace(root, refractionRay, lights, ambient, depth + 1);
			costheta = -glm::dot(viewRay.d, normal);
		} else {
			if (!refract(viewRay.d, -normal, 1.0 / eta, t)) return colour + reflection;
			Ray refractionRay(point, (EPS + ptao->tao) * t);
			refraction = ks * trace(root, refractionRay, lights, ambient, depth + 1);
			costheta = glm::dot(refractionRay.d, normal);
		}

		double R0 = pow(eta - 1, 2) / pow(eta + 1, 2);
		double R = R0 + (1 - R0) * pow(1 - costheta, 5);

		return colour + R * reflection + (1 - R) * refraction;
	}

	return colour + reflection;
}

void A5_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		glm::vec3 & eye,
		glm::vec3 & view,
		glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		std::list<Light *> & lights
) {

  std::cout << "Calling A5_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std::cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

	view = normalize(view);
	up = normalize(up);
	glm::vec3 left = normalize(glm::cross(view, up));

	int progress = 0;
	cout << "progress: " << progress << " %"<< endl;

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			GeometryNode *seenNode = NULL;

			Ray ray(eye, normalize(view + (-1 + 2 * (0.5 + y) / h) * tan(RAD(fovy / 2)) * -up + (-1 + 2 * (0.5 + x)  / w) * tan(RAD(fovy / 2)) * left));

			TAO *ptao = root->intersect(ray);

			image(x, y, 0) = 0.5 * x / w;
			image(x, y, 1) = 0.5 * y / h;
			image(x, y, 2) = 0.5 * (x + y) / (h + w);

			if (!ptao) continue;

			glm::vec3 colour = glm::vec3(0);

			for (int i = 0; i < SAMPLE; i++) {
				for (int j = 0; j < SAMPLE; j++) {
					Ray ray(eye, normalize(view + (-1 + 2 * (y + (0.5 + i) / SAMPLE) / h) * tan(RAD(fovy / 2)) * -up + (-1 + 2 * (x + (0.5 + j) / SAMPLE) / w) * tan(RAD(fovy / 2)) * left));
					colour += trace(root, ray, lights, ambient, 0);
				}
			}

			image(x, y, 0) = MIN(colour.x / (SAMPLE * SAMPLE), 1.0);
			image(x, y, 1) = MIN(colour.y / (SAMPLE * SAMPLE), 1.0);
			image(x, y, 2) = MIN(colour.z / (SAMPLE * SAMPLE), 1.0);

			if (100 * (y+1) * (x+1) / ((h+1) * (w+1)) > (progress + 10)) {
				progress += 10;
				cout << "progress: " << progress << " %"<< endl;
			}
		}
	}

	cout << "progress: 100 %" << endl;

}
