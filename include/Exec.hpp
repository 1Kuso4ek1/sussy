#pragma once
#include "Function.hpp"

static bool execNext = true;

Lexer::Token GetReturn(std::shared_ptr<AST::Node> node, VarMap& vars);
