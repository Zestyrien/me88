#include "parser.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

std::vector<std::string>
ParseExpression8Bits(std::shared_ptr<Node> const node) {

  std::vector<std::string> expression;
  auto ndvalue = node->GetValue();
  auto ndtype = node->GetType();

  if (ndtype == NodeType::Number) {
    expression.push_back("# load number in al");
    expression.push_back("mov_operand_al");
    expression.push_back("%" + ndvalue);
    return expression;
  }

  if (ndtype == NodeType::Variable) {
    expression.push_back("# load " + ndvalue + " in AL");
    // set ds
    expression.push_back("mov_ss_ax");
    expression.push_back("mov_ax_ds");
    // set di
    expression.push_back("mov_bp_ax");
    // sub displacement to base pointer
    // TO DO handle function arguments case at some point
    expression.push_back("sub_operand_al");
    expression.push_back("^" + ndvalue);
    expression.push_back("mov_ax_di");
    // move variable
    expression.push_back("mov_ds$di_al");
    expression.push_back("# loaded " + ndvalue + " in AL");
    return expression;
  }

  auto right = node->GetRight();
  if (right != nullptr) {
    // evaluate the right expression
    auto const rightExp = ParseExpression8Bits(right);
    expression.insert(expression.end(), rightExp.begin(), rightExp.end());
  }

  auto left = node->GetLeft();
  // Assignement
  if (ndvalue == ":=") {
    // assignement must have a variable on the left
    // and an expression on the right
    // the expression on the right is in al already
    expression.push_back("# assignement expression for " + left->GetValue());
    // save al
    expression.push_back("push_al");
    // set ds
    expression.push_back("mov_ss_ax");
    expression.push_back("mov_ax_ds");
    // set di
    expression.push_back("mov_bp_ax");
    // sub displacement to base pointer
    // TO DO handle function arguments case at some point
    expression.push_back("sub_operand_al");
    expression.push_back("^" + left->GetValue());
    // load address in di
    expression.push_back("mov_ax_di");
    // restore al
    expression.push_back("pop_al");
    // assign al
    expression.push_back("# move value in variable");
    expression.push_back("mov_al_ds$di");
    // expression successful set al to 1
    expression.push_back("mov_operand_al");
    expression.push_back("%1");
    expression.push_back("");
    return expression;
  }

  // this is dependent of what is on the right
  // whatever was on the left was evaluated and
  // it is sitting in al
  auto lftype = left->GetType();
  auto lfvalue = left->GetValue();
  if (lftype == NodeType::Variable) {
    if (ndvalue == "+") {
      expression.push_back("# operand + ds_di");
      // code.AddDSDI(lfvalue);
    } else if (ndvalue == "-") {
      expression.push_back("# operand - ds_di");
      // code.SubDSDI(lfvalue);
    } else if (ndvalue == ">") {
      expression.push_back("# operand > ds_di");
      // code.EvalBelowDSDI(lfvalue);
    } else if (ndvalue == "<") {
      expression.push_back("# operand < ds_di");
      // code.EvalAboveDSDI(lfvalue);
    } else if (ndvalue == "==") {
      expression.push_back("# operand == ds_di");
      // code.EvalEqualDSDI(lfvalue);
    } else if (ndvalue == "!=") {
      expression.push_back("# operand != ds_di");
      // code.EvalNotEqualDSDI(lfvalue);
    }
  } else {
    if (ndvalue == "+") {
      expression.push_back("# operand +");
      // code.Add(std::stoi(lfvalue));
    } else if (ndvalue == "-") {
      expression.push_back("# operand -");
      // code.Sub(std::stoi(lfvalue));
    } else if (ndvalue == ">") {
      expression.push_back("# operand >");
      // code.EvalBelow(std::stoi(lfvalue));
    } else if (ndvalue == "<") {
      expression.push_back("# operand <");
      // code.EvalAbove(std::stoi(lfvalue));
    } else if (ndvalue == "==") {
      expression.push_back("# operand ==");
      // code.EvalEqual(std::stoi(lfvalue));
    } else if (ndvalue == "!=") {
      expression.push_back("# operand !=");
      // code.EvalNotEqual(std::stoi(lfvalue));
    }
  }
  return expression;
}

std::vector<std::string> ParseScope(std::shared_ptr<Tree> const &tree,
                                    SymbolsTable const &symbols) {
  std::vector<std::string> scope;

  const auto scopeId = tree->GetScopeId();
  const auto [success, entries] = symbols.GetSymbolsInScope(scopeId);

  if (!success) {
    spdlog::error("Congratulazioni! This message should never be seen, please "
                  "open a issue on github this is a bug for sure! Please "
                  "attach your F7 code.");
    return scope;
  }

  scope.push_back("# start scope push variables");

  size_t countPush = 0;
  for (const auto entry : entries) {
    if (entry->symbolTp == SymbolType::Variable && entry->isAssigned &&
        entry->occurrences >= 1) {
      // push in the stack variables in the scope
      scope.push_back("push_al");
      countPush++;
    }
  }
  scope.push_back("");

  for (auto const node : tree->GetNodes()) {
    // ParseNode(node, symbols, code);
    switch (node->GetType()) {
    case NodeType::Variable:
      // var declaration ignore
      break;
    case NodeType::Operator: {
      scope.push_back("# parse expression from line " +
                      std::to_string(node->GetLine()));
      auto const expression = ParseExpression8Bits(node);
      scope.insert(scope.end(), expression.begin(), expression.end());
    } break;
    case NodeType::If:
      // ParseIf(node, symbols, code);
      break;
    case NodeType::While:
      // ParseWhile(node, symbols, code);
      break;
    case NodeType::Function:
      //  ParseFunctionDefinition(node, symbols, code);
      break;
    case NodeType::FunctionCall:
      //  ParseFunctionCall(node, symbols, code);
      break;
    default:
      spdlog::error("Invalid node type in tree: type {} value {}",
                    node->GetType(), node->GetValue());
      break;
    }
  }

  scope.push_back("# end scope clean stack");
  for (size_t i = 0; i < countPush; i++) {
    scope.push_back("pop_al");
  }
  scope.push_back("");

  return scope;
}

std::vector<std::string> Parser::ParseIR(std::string const &filename,
                                         Tree const &program,
                                         SymbolsTable const &symbols) {
  std::vector<std::string> ir;
  ir.push_back("#");
  ir.push_back("# " + filename);
  ir.push_back("#");
  ir.push_back("");

  auto tree = std::make_shared<Tree>(program);
  auto const scope = ParseScope(tree, symbols);

  ir.insert(ir.end(), scope.begin(), scope.end());

  ir.push_back("# end program");
  ir.push_back("htl");
  return ir;
}
