#include "TAO.hpp"

TAO::TAO() : taomin(0), taomax(0), nmin(glm::vec3(0)), nmax(glm::vec3(0)), materialmin(NULL), materialmax(NULL) {}
TAO::TAO(double taomin, double taomax, glm::vec3 nmin, glm::vec3 nmax) :
        taomin(taomin), taomax(taomax), nmin(nmin), nmax(nmax), materialmin(NULL), materialmax(NULL) {}
TAO::TAO(double taomin, double taomax, glm::vec3 nmin, glm::vec3 nmax, Material *mmin, Material *mmax) :
        taomin(taomin), taomax(taomax), nmin(nmin), nmax(nmax), materialmin(mmin), materialmax(mmax) {}
