#pragma once
#include "Function.hpp"

static bool execNext = true;

AST::NodeList GetCommaSeparatedNodes(std::shared_ptr<AST::Node> node, AST::NodeList found = {});

std::shared_ptr<Variable> GetReturn(std::shared_ptr<AST::Node> node, VarMap& vars);

void GetReturnIterative(std::shared_ptr<AST::Node> root, VarMap& globalVars);
