#include "JointCommand.hpp"

JointCommand::JointCommand(JointNode &node, glm::mat4 T) : Command(T), node(node) {}

void JointCommand::redo() {}
void JointCommand::undo() {}
