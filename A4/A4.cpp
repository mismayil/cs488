#include <glm/ext.hpp>
#include "A4.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include <math.h>

#define PI 3.1415926535
#define max(a, b) (a < b ? b : a)
#define min(a, b) (a < b ? a : b)
#define rad(a) (a * PI / 180)

using namespace std;

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
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
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

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			GeometryNode *seenNode = NULL;

			glm::vec3 pixel = eye + view + (-1 + 2 * (0.5 + y) / h) * tan(rad(fovy / 2)) * -up + (-1 + 2 * (0.5 + x)  / w) * tan(rad(fovy / 2)) * left;

			glm::vec3 ray = pixel - eye;
			double mintao = 0;

			for (SceneNode *node : root->children) {

				if (node->m_nodeType == NodeType::GeometryNode) {

					GeometryNode *gnode = static_cast<GeometryNode *>(node);
					Primitive *prim = gnode->m_primitive;
					double tao = prim->intersect(eye, ray);

					if (tao >= 0 && ((seenNode == NULL) || (tao < mintao))) {
						seenNode = gnode;
						mintao = tao;
					}
				}
			}

			if (seenNode == NULL) {

				// Red: increasing from top to bottom
				image(x, y, 0) = (double)y / h;
				// Green: increasing from left to right
				image(x, y, 1) = (double)x / w;
				// Blue: in lower-left and upper-right corners
				image(x, y, 2) = ((y < h/2 && x < w/2) || (y >= h/2 && x >= w/2)) ? 1.0 : 0.0;

			} else {

				glm::vec3 point = eye + mintao * ray;
				glm::vec3 colour = glm::vec3(0.0f, 0.0f, 0.0f);
				PhongMaterial *pmaterial = static_cast<PhongMaterial *>(seenNode->m_material);
				glm::vec3 kd = pmaterial->getkd();
				glm::vec3 ks = pmaterial->getks();
				double shininess = pmaterial->getsh();

				for (Light *light : lights) {
					glm::vec3 lightSource = light->position;
					glm::vec3 lightRay = glm::normalize(lightSource - point);
					glm::vec3 normal = seenNode->m_primitive->getNormal(point);
					glm::vec3 reflection = glm::normalize(-lightRay + 2.0f * glm::dot(lightRay, normal) * normal);
					double distance = glm::length(lightSource - point);
					glm::vec3 intensity = light->colour / (float) (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance);
					glm::vec3 diffuse = kd * (float) max(glm::dot(lightRay, normal), 0.0) * intensity;
					glm::vec3 specular =  ks * pow((float) max(glm::dot(reflection, glm::normalize(eye - point)), 0.0), shininess) * intensity;

					colour += diffuse + specular;
				}

				colour += kd * ambient;
				image(x, y, 0) = min(colour.x, 1.0);
				image(x, y, 1) = min(colour.y, 1.0);
				image(x, y, 2) = min(colour.z, 1.0);
			}
		}
	}

}
