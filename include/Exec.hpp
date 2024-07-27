#pragma once
#include "Arithmetics.hpp"

static bool execNext = true;

AST::NodeList GetCommaSeparatedNodes(std::shared_ptr<AST::Node> node, AST::NodeList found = {});

std::shared_ptr<Variable> GetReturn(std::shared_ptr<AST::Node> node, std::vector<VarMap>& scopes);

void GetReturnIterative(std::shared_ptr<AST::Node> root, std::vector<VarMap>& scopes);
