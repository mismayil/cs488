#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
	OPERATION op;
public:
	JointNode(const std::string & name, OPERATION op);
	virtual ~JointNode();
	TAO *intersect(Ray ray);
	TAO *join(TAO *B, TAO* C);
};
