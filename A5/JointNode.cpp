#include <iostream>
#include "JointNode.hpp"

using namespace std;

JointNode::JointNode(const std::string& name, OPERATION op) : SceneNode(name), op(op) {
	m_nodeType = NodeType::JointNode;
}

JointNode::~JointNode() {}

TAO* JointNode::intersect(Ray ray) {
	TAO *A, *B;

	ray.o = glm::vec3(get_inverse() * glm::vec4(ray.o, 1));
	ray.d = glm::vec3(get_inverse() * glm::vec4(ray.d, 0));

	if (children.size() == 0) return NULL;

	A = children.front()->intersect(ray);

	list<SceneNode*>::iterator it = children.begin();

	for (it++; it != children.end(); it++) {
		B = (*it)->intersect(ray);
		A = join(A, B);
		if (B) delete B;
	}

	return A;
}

TAO* JointNode::join(TAO *B, TAO* C) {
	TAO *mintao = NULL;
	TAO *maxtao = NULL;

	if (!B && !C) return NULL;

	switch (op) {

		case UNION:

			if (B && !C) {
				mintao = maxtao = B;
				goto join;
			}

			if (C && !B) {
				mintao = maxtao = C;
				goto join;
			}

			if (B->taomin < C->taomin) mintao = B;
			else mintao = C;

			if (B->taomax > C->taomax) maxtao = B;
			else maxtao = C;

			break;

		case INTERSECTION:

			if (!B || !C) return NULL;

			if (C->taomin < B->taomin && C->taomax > B->taomin) mintao = B;
			else if (B->taomin < C->taomin && B->taomax > C->taomin) mintao = C;
			else mintao = NULL;

			if (C->taomax < B->taomax && C->taomax > B->taomin) maxtao = C;
			else if (B->taomax < C->taomax && B->taomax > C->taomin) maxtao = B;
			else maxtao = NULL;

			break;

		case DIFFERENCE:

			if (C && !B) return NULL;

			if (B && !C) {
				mintao = maxtao = B;
				goto join;
			}

			if (B->taomin < C->taomin) mintao = B;
			else if (C->taomax < B->taomax) mintao = new TAO(C->taomax, C->taomax, -C->nmax, -C->nmax, C->materialmin, C->materialmax);
			else mintao = NULL;

			if (B->taomax > C->taomax) maxtao = B;
			else if (B->taomin < C->taomin) maxtao = new TAO(C->taomin, C->taomin, -C->nmin, -C->nmin, C->materialmin, C->materialmax);
			else maxtao = NULL;

			break;

		default:
			std::cout << "ERROR: joint operation not supported!" << std::endl;
			return NULL;
	}

	join:
		if (mintao) {
			mintao->nmin = glm::transpose(glm::mat3(get_inverse())) * mintao->nmin;
			mintao->nmax = glm::transpose(glm::mat3(get_inverse())) * mintao->nmax;
		}

		if (maxtao) {
			maxtao->nmax = glm::transpose(glm::mat3(get_inverse())) * maxtao->nmax;
			maxtao->nmin = glm::transpose(glm::mat3(get_inverse())) * maxtao->nmin;
		}

		if (mintao && maxtao) return new TAO(mintao->taomin, maxtao->taomax, mintao->nmin, maxtao->nmax, mintao->materialmin, maxtao->materialmax);

	return NULL;
}
