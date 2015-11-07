#include <glm/ext.hpp>
#include "A4.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include <math.h>

#define PI 3.14
#define max(a, b) (a < b ? b : a)

using namespace std;

glm::vec3 meti(glm::vec3 a, glm::vec3 b) {
	return glm::vec3(a.x * b.x, a.y * b.y, a.z * b.z);
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
	glm::vec3 right = glm::normalize(glm::cross(view, up));

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			GeometryNode *seenNode = NULL;

			glm::vec3 pixel = eye + view + (-1 + 2 * (0.5 + y) / h) * tan(fovy / 2 * PI / 180.0) * -up + (-1 + 2 * (0.5 + x)  / w) * tan(fovy / 2 * PI / 180.0) * right;

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
					glm::vec3 lightRay = point - lightSource;
					double tao = seenNode->m_primitive->intersect(lightSource, lightRay);

					if (tao >= 0) {
						glm::vec3 normal = seenNode->m_primitive->getNormal(point);
						glm::vec3 reflection = -1.0f * lightRay + 2.0f * glm::dot(lightRay, normal) * normal;
						glm::vec3 intensity = light->colour;
						glm::vec3 first = meti(kd * (float) max(glm::dot(lightRay, normal), fovy * PI / 180), intensity);
						glm::vec3 second =  meti(ks * pow((float) max(glm::dot(reflection, ray), (180 - fovy) * PI / 180), shininess), intensity);
						cout << "first" << first.x << " " << first.y << " " << first.z << endl;
						cout << "second" <<  second.x << " " << second.y << " " << second.z << endl;

						colour += first + second;
					}
				}

				colour += meti(kd, ambient);
				colour /= 255;
				//std::cout << colour.x << " " << colour.y << " " << colour.z << std::endl;
				image(x, y, 0) = colour.x;
				image(x, y, 1) = colour.y;
				image(x, y, 2) = colour.z;
			}
		}
	}

}
