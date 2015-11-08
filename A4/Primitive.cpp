#include "Primitive.hpp"
#include <vector>
#include <iostream>

using namespace std;



Primitive::~Primitive()
{
}

TAO* Primitive::intersect(glm::vec3 eye, glm::vec3 ray) {
    return new TAO(0, false, glm::vec3(0));
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

TAO* NonhierSphere::intersect(glm::vec3 eye, glm::vec3 ray) {
    double tao[2];
    double A = glm::dot(ray, ray);
    double B = glm::dot(2.0f * (eye - m_pos), ray);
    double C = glm::dot(eye - m_pos, eye - m_pos) - m_radius * m_radius;
    size_t res = quadraticRoots(A, B, C, tao);
    if (res == 0) return new TAO(0, false, glm::vec3(0));
    if (res == 1) return new TAO(tao[0], tao[0] < 0 ? false : true, (eye + (float) tao[0] * ray) - m_pos);
    double mintao = MIN(tao[0], tao[1]);
    return new TAO(mintao, mintao < 0 ? false : true, (eye + (float) mintao * ray) - m_pos);
}

NonhierBox::~NonhierBox()
{
}

TAO* NonhierBox::intersect(glm::vec3 eye, glm::vec3 ray) {
    vector<slab> slabs;
    vector<glm::vec3> v;   // box vertices

    for (float i = m_pos.x; i < m_pos.x + 2 * m_size; i += m_size) {
        for (float j = m_pos.y; j < m_pos.y + 2 * m_size; j += m_size) {
            for (float k = m_pos.z; k < m_pos.z + 2 * m_size; k += m_size) {
                v.push_back(glm::vec3(i, j, k));
            }
        }
    }

    plane p1 = getplane(v[0], v[1], v[2]);
    plane p2 = getplane(v[4], v[5], v[6]);
    slabs.push_back(slab(p1, p2, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

    plane p3 = getplane(v[0], v[2], v[4]);
    plane p4 = getplane(v[1], v[3], v[5]);
    slabs.push_back(slab(p3, p4, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

    plane p5 = getplane(v[0], v[1], v[4]);
    plane p6 = getplane(v[2], v[3], v[6]);
    slabs.push_back(slab(p5, p6, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    vector<TAO> mintaos;
    vector<TAO> maxtaos;

    for (int i = 0; i < slabs.size(); i++) {
        double tao1 = intersectPLane(slabs[i].p1, eye, ray);
        double tao2 = intersectPLane(slabs[i].p2, eye, ray);
        double mintao = MIN(tao1, tao2);
        double maxtao = MAX(tao1, tao2);
        mintaos.push_back({mintao, true, tao1 < tao2 ? slabs[i].n1 : slabs[i].n2});
        maxtaos.push_back({maxtao, true, tao1 > tao2 ? slabs[i].n1 : slabs[i].n2});
    }

    TAO m = mintaos[0];
    for(int i = 0; i < mintaos.size(); i++) {
        if (m.tao < mintaos[i].tao) m = mintaos[i];
    }

    TAO M = maxtaos[0];
    for(int i = 0; i < maxtaos.size(); i++) {
        if (M.tao > maxtaos[i].tao) M = maxtaos[i];
    }

    if (m.tao <= M.tao) return new TAO(m.tao, m.hit, m.n);

    return new TAO(0, false, glm::vec3(0));
}
