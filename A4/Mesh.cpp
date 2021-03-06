#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}

	aabb = new BoundedBox(m_vertices);
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*

  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

TAO* Mesh::intersect(glm::vec3 eye, glm::vec3 ray) {

#ifdef OPT
	TAO *btao = aabb->intersect(eye, ray);
#ifdef DRAW_AABB
	return aabb->intersect(eye, ray);
#else
	if (!btao->hit) return new TAO(0, false, glm::vec3(0));
#endif
#endif

	TAO *mintao = NULL;

	for (int i = 0; i < m_faces.size(); i++) {
		TAO *tao = intersectTriangle(eye, ray, m_vertices[m_faces[i].v1], m_vertices[m_faces[i].v2], m_vertices[m_faces[i].v3]);
		if (tao->hit && (!mintao || tao->tao < mintao->tao)) mintao = tao;
	}

	if (mintao) return mintao;

	return new TAO(0, false, glm::vec3(0));
}
