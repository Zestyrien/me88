#include "parser.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

size_t CountOffset(std::vector<std::string> const &code) {
  // return the actual size of the code without comments and empy lines
  size_t count = 0;
  for (auto const &line : code) {
    if (line == "") {
      continue;
    }

    if (line.rfind("#", 0) == 0) {
      // skip comment
      continue;
    }

    count++;
  }
  return count;
}

std::vector<std::string> ParseScope(std::shared_ptr<Tree> const &tree,
                                    SymbolsTable const &symbols);

std::vector<std::string>
ParseExpression8Bits(std::shared_ptr<Node> const &node) {

  std::vector<std::string> expression;

  auto const ndvalue = node->GetValue();
  auto const ndtype = node->GetType();

  auto const ParseLoadVariableAL = [](std::vector<std::string> &code,
                                      std::string const &variable) {
    // save al
    code.push_back("push_al");
    // set ds
    code.push_back("mov_ss_ax");
    code.push_back("mov_ax_ds");
    // set di
    code.push_back("mov_bp_ax");
    // sub displacement to base pointer
    // TO DO handle function arguments case at some point
    code.push_back("sub_operand_al");
    code.push_back("^" + variable);
    code.push_back("mov_ax_di");
    // restore al
    code.push_back("pop_al");
  };

  if (ndtype == NodeType::Number) {
    expression.push_back("# load number in al");
    expression.push_back("mov_operand_al");
    expression.push_back("%" + ndvalue);
    return expression;
  }

  if (ndtype == NodeType::Variable) {
    ParseLoadVariableAL(expression, ndvalue);
    // move variable
    expression.push_back("mov_ds$di_al");
    expression.push_back("# loaded " + ndvalue + " in AL");
    return expression;
  }

  auto const right = node->GetRight();
  if (right != nullptr) {
    // evaluate the right expression
    auto const rightExp = ParseExpression8Bits(right);
    expression.insert(expression.end(), rightExp.begin(), rightExp.end());
  }

  auto const left = node->GetLeft();
  // Assignement
  if (ndvalue == ":=") {
    // assignement must have a variable on the left
    // and an expression on the right
    // the expression on the right is in al already
    expression.push_back("# assignement expression for " + left->GetValue());

    ParseLoadVariableAL(expression, left->GetValue());

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
  auto const lftype = left->GetType();
  auto const lfvalue = left->GetValue();

  auto const ParseEval = [](std::vector<std::string> &code) {
    // add 4 to the offset so if the condition is met will skipp false
    code.push_back("off+5");
    code.push_back("off+5");
    // condition will jump to true if it is met
    code.push_back("# condition false");
    // if the condition is false set move 0 to al to signal false
    code.push_back("mov_operand_al");
    code.push_back("%0");
    // if it is false jump and skip the true section
    code.push_back("jmp_cs_offset");
    code.push_back("off+2");
    code.push_back("off+2");

    code.push_back("# condition true");
    // if the condition is true set move 1 to al to signal true
    code.push_back("mov_operand_al");
    code.push_back("%1");
    code.push_back("");
  };

  if (lftype == NodeType::Variable) {
    if (ndvalue == "+") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("add_ds$di_al");
    } else if (ndvalue == "-") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("sub_ds$di_al");
    } else if (ndvalue == ">") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("cmp_ds$di_al");
      expression.push_back("jb_cs_offset");
      ParseEval(expression);
    } else if (ndvalue == "<") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("cmp_ds$di_al");
      expression.push_back("ja_cs_offset");
      ParseEval(expression);
    } else if (ndvalue == "==") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("cmp_ds$di_al");
      expression.push_back("je_cs_offset");
      ParseEval(expression);
    } else if (ndvalue == "!=") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("cmp_ds$di_al");
      expression.push_back("jne_cs_offset");
      ParseEval(expression);
    }
  } else {
    if (ndvalue == "+") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("add_operand_al");
      expression.push_back("%" + lfvalue);
    } else if (ndvalue == "-") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("sub_operand_al");
      expression.push_back("%" + lfvalue);
    } else if (ndvalue == ">") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("cmp_operand_al");
      expression.push_back("%" + lfvalue);
      expression.push_back("jb_cs_offset");
      ParseEval(expression);
    } else if (ndvalue == "<") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("cmp_operand_al");
      expression.push_back("%" + lfvalue);
      expression.push_back("ja_cs_offset");
      ParseEval(expression);
    } else if (ndvalue == "==") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("cmp_operand_al");
      expression.push_back("%" + lfvalue);
      expression.push_back("je_cs_offset");
      ParseEval(expression);
    } else if (ndvalue == "!=") {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("cmp_operand_al");
      expression.push_back("%" + lfvalue);
      expression.push_back("jne_cs_offset");
      ParseEval(expression);
    }
  }

  return expression;
}

std::vector<std::string> ParseIf(std::shared_ptr<Node> const &node,
                                 SymbolsTable const &symbols) {
  auto const ndbodies = node->GetRight();
  auto const ifbodyTree = ndbodies->GetLeft()->GetTree();
  auto const ndbodieselse = ndbodies->GetRight();
  auto const elsebodyTree =
      ndbodieselse != nullptr ? ndbodieselse->GetTree() : nullptr;

  std::vector<std::string> ifcode;
  // condition will be evaluated and put in al
  auto const exp = ParseExpression8Bits(node->GetLeft());
  ifcode.insert(ifcode.end(), exp.begin(), exp.end());
  // if the condition is true there will be 1 in AL
  ifcode.push_back("# check expression result");
  ifcode.push_back("cmp_operand_al");
  ifcode.push_back("%1");
  // need to skip the if block when the condition is not met
  ifcode.push_back("jne_cs_offset");

  // parse the scope code
  auto const ifscopecode = ParseScope(ifbodyTree, symbols);
  // get the size of the scope code to know how far to jump the the condition is
  // not met
  auto offset = CountOffset(ifscopecode);
  if (elsebodyTree != nullptr) {
    // when there is an else body we need to add a jmp + offset to skip the else
    // scope when we execute the if scope
    offset += 3;
  }

  ifcode.push_back("%" + std::to_string(offset));

  // add if scope code
  ifcode.insert(ifcode.end(), ifscopecode.begin(), ifscopecode.end());

  if (elsebodyTree != nullptr) {
    // when the if is executed we need to skip the else
    ifcode.push_back("jmp_cs_offset");
    // parse the scope code
    auto const elsescodescope = ParseScope(elsebodyTree, symbols);
    // get the size of the scope code to know how far to jump
    ifcode.push_back("%" + std::to_string(CountOffset(elsescodescope)));

    ifcode.insert(ifcode.end(), elsescodescope.begin(), elsescodescope.end());
  }

  return ifcode;
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
    case NodeType::If: {
      scope.push_back("# parse if statement from line " +
                      std::to_string(node->GetLine()));
      auto const ifcode = ParseIf(node, symbols);
      scope.insert(scope.end(), ifcode.begin(), ifcode.end());
    } break;
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
