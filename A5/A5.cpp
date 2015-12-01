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
			glm::vec3 specular = ks * pow((float) MAX(glm::dot(reflectionRay.d, normalize(ray.o - point)), 0.0), shininess) * intensity;
			colour += diffuse + specular;
		}
	}

	Ray viewRay(ray.o, normalize(point - ray.o));
	glm::vec3 reflection = glm::vec3(0);
	glm::vec3 refraction = glm::vec3(0);
	colour += kd * ambient;

	if (depth == MAX_DEPTH) return colour;

	Ray reflectionRay(point, (EPS + ptao->tao) * normalize(viewRay.d - 2 * glm::dot(viewRay.d, normal) * normal));
	if (reflectiveness > 0) reflection = ks * trace(root, reflectionRay, lights, ambient, depth + 1) * reflectiveness;

	if (refractiveness > 1 || refractiveness < 1) {
		double costheta = 0;
		double eta = AIR_REF_INDEX / refractiveness;
		glm::vec3 t;
		glm::vec3 tmp = glm::vec3(0);

		if (glm::dot(viewRay.d, normal) < 0) {
			if (!refract(viewRay.d, normal, eta, t)) return colour + reflection;
			Ray refractionRay(point, (EPS + ptao->tao) * t);
			refraction = ks * trace(root, refractionRay, lights, ambient, depth + 1);
			costheta = -glm::dot(viewRay.d, normal);
			tmp = colour;
		} else {
			if (!refract(viewRay.d, -normal, 1.0 / eta, t)) return reflection;
			Ray refractionRay(point, (EPS + ptao->tao) * t);
			refraction = ks * trace(root, refractionRay, lights, ambient, depth + 1);
			costheta = glm::dot(refractionRay.d, normal);
		}

		double R0 = pow(eta - 1, 2) / pow(eta + 1, 2);
		double R = R0 + (1 - R0) * pow(1 - costheta, 5);
		return tmp + R * reflection + (1 - R) * refraction;
	}

	return colour + reflection;
}

glm::vec3 process(pixel p, glm::vec3 eye, glm::vec3 view, glm::vec3 up, glm::vec3 left, SceneNode *root, Ray ray, list<Light *> &lights, const glm::vec3 &ambient, size_t w, size_t h) {
	vector<glm::vec3> colours;
	glm::vec3 colour = glm::vec3(0);

	for (int i = 0; i < SAMPLE; i++) {
		for (int j = 0; j < SAMPLE; j++) {
			glm::vec3 direction = normalize(view + (p.l + (p.r - p.l) * (p.y + (0.5 + i) / SAMPLE) / h) * up + (p.b + (p.t - p.b) * (p.x + (0.5 + j) / SAMPLE) / w) * left);
			Ray ray(eye, direction);
			glm::vec3 c = trace(root, ray, lights, ambient, 0);
			colours.push_back(c);
			colour += c;
		}
	}

	for (int k = 0; k < colours.size(); k++) {
		for (int m = 0; m < colours.size(); m++) {
			glm::vec3 diff = glm::abs(colours[k] - colours[m]);
			if (diff.x > THRESHOLD && diff.y > THRESHOLD && diff.z > THRESHOLD) goto adapt;
		}
	}

	stop: return colour / (SAMPLE * SAMPLE);

	adapt:
		colour = glm::vec3(0);
		double nl = p.l + (p.r - p.l) / 2;
		double nr = p.l + (p.r - p.l) / 2;
		double nb = p.b + (p.t - p.b) / 2;
		double nt = p.b + (p.t - p.b) / 2;
		pixel p1 = {p.x, p.y, p.l, nr, nb, p.t};
		pixel p2 = {p.x, p.y, nl, p.r, nb, p.t};
		pixel p3 = {p.x, p.y, p.l, nr, p.b, nt};
		pixel p4 = {p.x, p.y, nl, p.r, p.b, nt};
		colour += process(p1, eye, view, up, left, root, ray, lights, ambient, w, h);
		colour += process(p2, eye, view, up, left, root, ray, lights, ambient, w, h);
		colour += process(p3, eye, view, up, left, root, ray, lights, ambient, w, h);
		colour += process(p4, eye, view, up, left, root, ray, lights, ambient, w, h);

		return colour / 4;
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

	int percent = 0;
	cout << "progress: " << percent << " %"<< endl;

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			pixel p = {x, y, -1, 1, -1, 1};

			Ray ray(eye, normalize(view + (p.l + (p.r - p.l) * (0.5 + p.y) / h) * tan(RAD(fovy / 2)) * -up + (p.b + (p.t - p.b) * (0.5 + p.x)  / w) * tan(RAD(fovy / 2)) * left));

			TAO *ptao = root->intersect(ray);

			image(x, y, 0) = 0.5 * x / w;
			image(x, y, 1) = 0.5 * y / h;
			image(x, y, 2) = 0.5 * (x + y) / (h + w);

			if (!ptao) continue;

			glm::vec3 colour = process(p, eye, view, tan(RAD(fovy / 2)) * -up, tan(RAD(fovy / 2)) * left, root, ray, lights, ambient, w, h);

			image(x, y, 0) = MIN(colour.x, 1.0);
			image(x, y, 1) = MIN(colour.y, 1.0);
			image(x, y, 2) = MIN(colour.z, 1.0);

			progress(percent, x, y, w, h);
		}
	}

	cout << "progress: 100 %" << endl;
}
