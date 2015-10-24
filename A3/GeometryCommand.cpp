#include "GeometryCommand.hpp"

GeometryCommand::GeometryCommand(GeometryNode &node, glm::mat4 T) : Command(T), node(node) {}

void GeometryCommand::redo() {}
void GeometryCommand::undo() {}
