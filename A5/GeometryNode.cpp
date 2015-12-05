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

GeometryNode::~GeometryNode() {
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

	if (!tao) return NULL;

	glm::vec3 point = ray.o + (float) tao->taomin * ray.d;

	TextureMaterial *tmaterial = dynamic_cast<TextureMaterial *>(m_material);

	if (tmaterial) tmaterial->setuv(m_primitive->mapuv(point, tao->nmin, tmaterial->getTexture()));

	tao->materialmin = m_material;
	tao->materialmax = m_material;

	tao->nmin = glm::transpose(glm::mat3(get_inverse())) * tao->nmin;
	tao->nmax = glm::transpose(glm::mat3(get_inverse())) * tao->nmax;

	return tao;
}
