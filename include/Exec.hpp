#pragma once
#include "Function.hpp"

static bool execNext = true;

AST::NodeList GetCommaSeparatedNodes(std::shared_ptr<AST::Node> node, AST::NodeList found = {});

Lexer::Token GetReturn(std::shared_ptr<AST::Node> node, VarMap& vars);
