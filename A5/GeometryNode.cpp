#include "GeometryNode.hpp"
#include "TextureMaterial.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

TAO* GeometryNode::intersect(Ray ray) {
	ray.o = glm::vec3(get_inverse() * glm::vec4(ray.o, 1));
	ray.d = glm::vec3(get_inverse() * glm::vec4(ray.d, 0));

	TAO *tao = m_primitive->intersect(ray);
	glm::vec3 point = ray.o + (float) tao->tao * ray.d;

	TextureMaterial *tmaterial = dynamic_cast<TextureMaterial *>(m_material);

	if (tmaterial) tmaterial->setuv(m_primitive->mapuv(point, tao->n, tmaterial->getTexture()));

	tao->material = m_material;
	tao->n = glm::transpose(glm::mat3(get_inverse())) * tao->n;

	return tao;
}
