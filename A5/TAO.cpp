#include "TAO.hpp"

TAO::TAO() : tao(0), hit(false), n(glm::vec3(0)), material(NULL) {}
TAO::TAO(double tao, bool hit, glm::vec3 n) : tao(tao), hit(hit), n(n), material(NULL) {}
