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

	glm::vec3 point = ray.o + ptao->taomin * ray.d;
	PhongMaterial *pmaterial = static_cast<PhongMaterial *>(ptao->materialmin);
	glm::vec3 normal = normalize(pmaterial->bump(ptao->nmin, point));
	glm::vec3 kd = pmaterial->getkd();
	glm::vec3 ks = pmaterial->getks();
	double shininess = pmaterial->getShininess();
	double reflectiveness = pmaterial->getReflectiveness();
	double refractiveness = pmaterial->getRefractiveness();
	Ray eyeRay(point, normalize(ray.o - point));
	Ray viewRay(ray.o, normalize(point - ray.o));

	for (Light *light : lights) {
		vector<Ray> lightRays = light->getRays(point);
		glm::vec3 tmpcolor = glm::vec3(0);

		for (Ray lightRay : lightRays) {

			TAO *stao = root->intersect(lightRay);

			if (stao) continue;

			Ray halfRay(point, normalize(eyeRay.d + lightRay.d));
			glm::vec3 intensity = light->getIntensity(lightRay);
			glm::vec3 diffuse = kd * (float) MAX(glm::dot(lightRay.d, normal), 0.0) * intensity;
			glm::vec3 specular = ks * pow((float) MAX(glm::dot(normal, halfRay.d), 0.0), shininess) * intensity;
			tmpcolor += diffuse + specular;
		}

		colour += tmpcolor / light->getSamples();
	}

	colour += kd * ambient;

	if (depth == MAX_DEPTH) return colour;

	glm::vec3 reflection = glm::vec3(0);
	glm::vec3 refraction = glm::vec3(0);
	glm::vec3 direction = normalize(viewRay.d - 2 * glm::dot(viewRay.d, normal) * normal);
	Ray reflectionRay(point + EPS * direction, direction);

	if (reflectiveness > 0) reflection = ks * trace(root, reflectionRay, lights, ambient, depth + 1) * reflectiveness;

	if (refractiveness > 1 || refractiveness < 1) {
		double costheta = 0;
		double eta = AIR_REF_INDEX / refractiveness;

		if (glm::dot(viewRay.d, normal) < 0) {
			if (!refract(viewRay.d, normal, eta, direction)) return colour + reflection;
			Ray refractionRay(point + EPS * direction, direction);
			refraction = trace(root, refractionRay, lights, ambient, depth + 1);
			costheta = -glm::dot(viewRay.d, normal);
		} else {
			if (!refract(viewRay.d, -normal, 1.0 / eta, direction)) return reflection;
			Ray refractionRay(point + EPS * direction, direction);
			refraction = trace(root, refractionRay, lights, ambient, depth + 1);
			costheta = glm::dot(refractionRay.d, normal);
		}

		double R0 = pow(eta - 1, 2) / pow(eta + 1, 2);
		double R = R0 + (1 - R0) * pow(1 - costheta, 5);
		return R * reflection + (1 - R) * refraction;
	}

	return colour + reflection;
}

glm::vec3 process(pixel p, glm::vec3 eye, glm::vec3 view, glm::vec3 up, glm::vec3 left, SceneNode *root, Ray ray, list<Light *> &lights, const glm::vec3 &ambient, size_t w, size_t h, int depth) {
	vector<glm::vec3> colours;
	glm::vec3 colour = glm::vec3(0);

	for (int i = 0; i < SAMPLE; i++) {
		for (int j = 0; j < SAMPLE; j++) {
			double nx = random(p.x + i * p.offset / SAMPLE, p.x + (i + 1) * p.offset / SAMPLE);
			double ny = random(p.y + i * p.offset / SAMPLE, p.y + (j + 1) * p.offset / SAMPLE);
			glm::vec3 d = view + (-1 + 2 * (float)ny / h) * up + (-1 + 2 * (float)nx / w) * left;
			//focus
			// double aperture = 16;
			// double focalDepth = 5;
			// glm::vec3 apOffset = glm::vec3((-1 + 2 * (float)ny / h) * aperture, (-1 + 2 * (float)nx / w) * aperture, 0.0f);
			// eye += apOffset;
			// d = focalDepth * d;
			// d -= apOffset;
			glm::vec3 direction = normalize(d);
			Ray ray(eye, direction);
			// double f = 2;
			// double dEyeImage = abs(eye.z);
			// double dEyePixel = glm::length(d);
			// glm::vec3 fp = eye + ((float)dEyePixel * (float)(dEyeImage + f) / (float)dEyeImage) * direction;
			// double m = 4;
			// ny -= m / 2;
			// nx -= m / 2;
			// glm::vec3 c = glm::vec3(0);
			//
			// for (int a = 0; a < SAMPLE; a++) {
			// 	for (int b = 0; b < SAMPLE; b++) {
			// 		double npx = random(nx + a * m / SAMPLE, nx + (a + 1) * m / SAMPLE);
			// 		double npy = random(ny + b * m / SAMPLE, ny + (b + 1) * m / SAMPLE);
			// 		d = view + (-1 + 2 * (float)npy / h) * up + (-1 + 2 * (float)npx / w) * left;
			// 		glm::vec3 pxl = eye + d;
			// 		direction = normalize(fp - pxl);
			// 		Ray fray(pxl, direction);
			// 		glm::vec3 fc = trace(root, fray, lights, ambient, 0);
			// 		c += fc;
			// 	}
			// }
			// c /= SAMPLE * SAMPLE;
			// //end focus
			glm::vec3 c = trace(root, ray, lights, ambient, 0);
			colours.push_back(c);
			colour += c;
		}
	}

	if (depth == ADAPTIVE_DEPTH) goto stop;

	for (int k = 0; k < colours.size(); k++) {
		for (int m = k; m < colours.size(); m++) {
			glm::vec3 diff = glm::abs(colours[k] - colours[m]);
			if (diff.x > THRESHOLD || diff.y > THRESHOLD || diff.z > THRESHOLD)
#ifdef ADAPTIVE_FALSE_COLOR
			return glm::vec3(0);
#else
			goto adapt;
#endif
		}
	}

#ifdef ADAPTIVE_FALSE_COLOR
	return glm::vec3(1);
#else
	stop: return colour / (SAMPLE * SAMPLE);
#endif

	adapt:
		colour = glm::vec3(0);
		double noffset = p.offset / 2;
		double nx = p.x + noffset;
		double ny = p.y + noffset;
		pixel p1 = {p.x, p.y, noffset};
		pixel p2 = {nx, p.y, noffset};
		pixel p3 = {p.x, ny, noffset};
		pixel p4 = {nx, ny, noffset};
		colour += process(p1, eye, view, up, left, root, ray, lights, ambient, w, h, depth + 1);
		colour += process(p2, eye, view, up, left, root, ray, lights, ambient, w, h, depth + 1);
		colour += process(p3, eye, view, up, left, root, ray, lights, ambient, w, h, depth + 1);
		colour += process(p4, eye, view, up, left, root, ray, lights, ambient, w, h, depth + 1);

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

	up = tan(RAD(fovy / 2)) * -up;
	left = tan(RAD(fovy / 2)) * left;

	int percent = 0;
	cout << "progress: " << percent << " %"<< endl;

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			pixel p = {(double) x, (double) y, 1};

			Ray ray(eye, normalize(view + (-1 + 2 * (0.5 + y) / h) * up + (-1 + 2 * (0.5 + x) / w) * left));

			TAO *ptao = root->intersect(ray);

			image(x, y, 0) = (double) x / w;
			image(x, y, 1) = (double) y / h;
			image(x, y, 2) = (double) (x + y) / (h + w);

			if (!ptao) continue;

			glm::vec3 colour = process(p, eye, view, up, left, root, ray, lights, ambient, w, h, 0);

			image(x, y, 0) = MIN(colour.x, 1.0);
			image(x, y, 1) = MIN(colour.y, 1.0);
			image(x, y, 2) = MIN(colour.z, 1.0);

			progress(percent, x, y, w, h);
		}
	}

	cout << "progress: 100 %" << endl;
}
