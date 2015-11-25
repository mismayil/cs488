#include <glm/ext.hpp>
#include "A5.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "util.hpp"

using namespace std;

TAO* intersect(SceneNode *node, glm::vec4 eye, glm::vec4 ray) {
	TAO *mintao = NULL;

	if (node->m_nodeType == NodeType::GeometryNode) {
		GeometryNode *gnode = static_cast<GeometryNode *>(node);
		Primitive *prim = gnode->m_primitive;
		TAO *tao = prim->intersect(glm::vec3(node->get_inverse() * eye), glm::vec3(node->get_inverse() * ray));
		tao->n = glm::transpose(glm::mat3(node->get_inverse())) * tao->n;
		tao->node = node;
		return tao;
	}

	for (SceneNode *child : node->children) {
		TAO *tao = intersect(child, node->get_inverse() * eye, node->get_inverse() * ray);
		if (tao && tao->hit && ((mintao == NULL) || (tao->tao < mintao->tao))) mintao = tao;
	}

	if (mintao) mintao->n = glm::transpose(glm::mat3(node->get_inverse())) * mintao->n;
	return mintao;
}

glm::vec3 trace(SceneNode *root, glm::vec3 source, glm::vec3 ray, list<Light *> &lights, const glm::vec3 & ambient, int depth) {
	glm::vec3 colour = glm::vec3(0);
	glm::vec3 kd, ks;

	if (depth == MAX_DEPTH) return colour;

	TAO *ptao = intersect(root, glm::vec4(source, 1), glm::vec4(ray, 0));

	if (!ptao || ptao->node == NULL) return colour;

	glm::vec3 point = source + ptao->tao * ray;
	glm::vec3 normal = glm::normalize(ptao->n);
	GeometryNode *gnode = static_cast<GeometryNode *>(ptao->node);
	PhongMaterial *pmaterial = static_cast<PhongMaterial *>(gnode->m_material);
	kd = pmaterial->getkd();
	ks = pmaterial->getks();
	//double shininess = pmaterial->getsh();

	for (Light *light : lights) {
		glm::vec3 lightSource = light->position;
		glm::vec3 lightRay = glm::normalize(lightSource - point);
		glm::vec3 shadowRay = (EPS + ptao->tao) * lightRay;

		TAO *stao = intersect(root, glm::vec4(point, 1), glm::vec4(shadowRay, 0));

		if (!stao || !stao->hit) {
			//glm::vec3 reflection = glm::normalize(-lightRay + 2.0f * glm::dot(lightRay, normal) * normal);
			double distance = glm::length(lightSource - point);
			glm::vec3 intensity = light->colour / (float) (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance);
			glm::vec3 diffuse = kd * (float) MAX(glm::dot(lightRay, normal), 0.0) * intensity;
			//glm::vec3 specular =  ks * pow((float) MAX(glm::dot(reflection, glm::normalize(source - point)), 0.0), shininess) * intensity;
			//colour += diffuse + specular;
			colour += diffuse;
		}
	}

	glm::vec3 reflectionRay = glm::normalize((point - source) + 2 * MAX(glm::dot((point - source), normal), 0.0) * normal);
	glm::vec3 reflection = ks * trace(root, point, (EPS + ptao->tao) * reflectionRay, lights, ambient, depth + 1);
	//cout << reflection.x << " " << reflection.y << " " << reflection.z << endl;

	colour += kd * ambient + reflection;

	return colour;
}

void A4_Render(
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

  std::cout << "Calling A4_Render(\n" <<
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

	view = glm::normalize(view);
	up = glm::normalize(up);
	glm::vec3 left = glm::normalize(glm::cross(view, up));

	int progress = 0;
	cout << "progress: " << progress << " %"<< endl;

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			GeometryNode *seenNode = NULL;

			glm::vec3 ray = glm::normalize(view + (-1 + 2 * (0.5 + y) / h) * tan(RAD(fovy / 2)) * -up + (-1 + 2 * (0.5 + x)  / w) * tan(RAD(fovy / 2)) * left);

			TAO *ptao = intersect(root, glm::vec4(eye, 1), glm::vec4(ray, 0));

			image(x, y, 0) = 3 * y * 0.7 / h;
			image(x, y, 1) = 4 * x * 0.2 / w;
			image(x, y, 2) = 5 * (x + y) * 0.9 / (h + w);

			if (!ptao || ptao->node == NULL) continue;

			glm::vec3 colour = glm::vec3(0);

			for (int i = 0; i < SAMPLE; i++) {
				for (int j = 0; j < SAMPLE; j++) {
					ray = glm::normalize(view + (-1 + 2 * (y + (0.5 + i) / SAMPLE) / h) * tan(RAD(fovy / 2)) * -up + (-1 + 2 * (x + (0.5 + j) / SAMPLE) / w) * tan(RAD(fovy / 2)) * left);
					colour += trace(root, eye, ray, lights, ambient, 0);
				}
			}

			image(x, y, 0) = MIN(colour.x / (SAMPLE * SAMPLE), 1.0);
			image(x, y, 1) = MIN(colour.y / (SAMPLE * SAMPLE), 1.0);
			image(x, y, 2) = MIN(colour.z / (SAMPLE * SAMPLE), 1.0);
			// colour += trace(root, eye, ray, lights, ambient, 0);
			// image(x, y, 0) = MIN(colour.x, 1.0);
			// image(x, y, 1) = MIN(colour.y, 1.0);
			// image(x, y, 2) = MIN(colour.z, 1.0);

			if (100 * (y+1) * (x+1) / ((h+1) * (w+1)) > (progress + 10)) {
				progress += 10;
				cout << "progress: " << progress << " %"<< endl;
			}
		}
	}

	cout << "progress: 100 %" << endl;

}
