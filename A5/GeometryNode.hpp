#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim,
		Material *mat = nullptr );
	~GeometryNode();
	void setMaterial( Material *material );
	TAO *intersect(Ray ray);
	Material *m_material;
	Primitive *m_primitive;
};
