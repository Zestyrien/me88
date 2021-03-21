#include "parser.h"
#include "../../common/opcode.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

// TO DO make something a bit more structured
std::unordered_map<int, std::unordered_map<std::string, int>> stackDisplacement;
std::unordered_map<std::string, std::unordered_map<std::string, int>>
    argumentDisplacement;

std::unordered_map<std::string, int> poundDisplacement;

size_t CountOffset(std::vector<std::string> const &code)
{
  // return the actual size of the code without comments and empy lines
  size_t count = 0;
  for (auto const &line : code)
  {
    if (line == "")
    {
      continue;
    }

    if (line.rfind("#", 0) == 0)
    {
      // skip comment
      continue;
    }

    if (line.rfind("@", 0) == 0)
    {
      // skip function entry point placeholder
      continue;
    }

    count++;
  }
  return count;
}

std::vector<std::string> ParseScope(std::shared_ptr<Tree> const &tree,
                                    SymbolsTable const &symbols);

std::vector<std::string> ParseExpression8Bits(std::shared_ptr<Node> const &node,
                                              SymbolsTable const &symbols,
                                              int const &scopeId)
{

  std::vector<std::string> expression;

  auto const ndvalue = node->GetValue();
  auto const ndtype = node->GetType();

  auto const ParseLoadVariableAL = [symbols,
                                    scopeId](std::vector<std::string> &code,
                                             std::string const &variable) {
    // save al
    code.push_back("push_al");
    // set ds
    code.push_back("mov_ss_ax");
    code.push_back("mov_ax_ds");
    // set di
    code.push_back("mov_bp_ax");
    // sub displacement to base pointer if it's a new varialble in the scope
    // add it if it's a function argument
    size_t displacement = 0;
    if (symbols.IsFunctionArgument(variable, scopeId))
    {
      // function arguments are above bp
      // 1 is the minimum required
      // 2 is the size of the return address
      //  TO DO double check if need to save more stuff on function call
      displacement =
          symbols.GetFunctionArgumentIndex(variable, scopeId) + 1 + 2;
      code.push_back("add_operand_al");
    }
    else
    {
      // variable declared in the scope are under bp
      code.push_back("sub_operand_al");
      displacement = stackDisplacement[scopeId][variable];
    }

    code.push_back("# displacement for " + variable);
    code.push_back("^" + std::to_string(displacement));
    code.push_back("mov_ax_di");
    // restore al
    code.push_back("pop_al");
  };

  if (ndtype == NodeType::Number)
  {
    expression.push_back("# load number in al");
    expression.push_back("mov_operand_al");
    expression.push_back("%" + ndvalue);
    return expression;
  }

  if (ndtype == NodeType::Variable)
  {
    ParseLoadVariableAL(expression, ndvalue);
    // move variable
    expression.push_back("mov_ds$di_al");
    expression.push_back("# loaded " + ndvalue + " in AL");
    return expression;
  }

  auto const right = node->GetRight();
  if (right != nullptr)
  {
    // evaluate the right expression
    auto const rightExp = ParseExpression8Bits(right, symbols, scopeId);
    expression.insert(expression.end(), rightExp.begin(), rightExp.end());
  }

  auto const left = node->GetLeft();
  // Assignement
  if (ndvalue == ":=")
  {
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

  if (lftype == NodeType::Variable)
  {
    if (ndvalue == "+")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("add_ds$di_al");
    }
    else if (ndvalue == "-")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("sub_ds$di_al");
    }
    else if (ndvalue == ">")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("cmp_ds$di_al");
      expression.push_back("jb_cs_offset");
      ParseEval(expression);
    }
    else if (ndvalue == "<")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("cmp_ds$di_al");
      expression.push_back("ja_cs_offset");
      ParseEval(expression);
    }
    else if (ndvalue == "==")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("cmp_ds$di_al");
      expression.push_back("je_cs_offset");
      ParseEval(expression);
    }
    else if (ndvalue == "!=")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      ParseLoadVariableAL(expression, lfvalue);
      expression.push_back("cmp_ds$di_al");
      expression.push_back("jne_cs_offset");
      ParseEval(expression);
    }
  }
  else
  {
    if (ndvalue == "+")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("add_operand_al");
      expression.push_back("%" + lfvalue);
    }
    else if (ndvalue == "-")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("sub_operand_al");
      expression.push_back("%" + lfvalue);
    }
    else if (ndvalue == ">")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("cmp_operand_al");
      expression.push_back("%" + lfvalue);
      expression.push_back("jb_cs_offset");
      ParseEval(expression);
    }
    else if (ndvalue == "<")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("cmp_operand_al");
      expression.push_back("%" + lfvalue);
      expression.push_back("ja_cs_offset");
      ParseEval(expression);
    }
    else if (ndvalue == "==")
    {
      expression.push_back("# expression " + ndvalue + " " + lfvalue);
      expression.push_back("cmp_operand_al");
      expression.push_back("%" + lfvalue);
      expression.push_back("je_cs_offset");
      ParseEval(expression);
    }
    else if (ndvalue == "!=")
    {
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
                                 SymbolsTable const &symbols,
                                 int const &scopeId)
{
  auto const ndbodies = node->GetRight();
  auto const ifbodyTree = ndbodies->GetLeft()->GetTree();
  auto const ndbodieselse = ndbodies->GetRight();
  auto const elsebodyTree =
      ndbodieselse != nullptr ? ndbodieselse->GetTree() : nullptr;

  std::vector<std::string> ifcode;
  // condition will be evaluated and put in al
  auto const exp = ParseExpression8Bits(node->GetLeft(), symbols, scopeId);
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
  if (elsebodyTree != nullptr)
  {
    // when there is an else body we need to add a jmp + offset to skip the else
    // scope when we execute the if scope
    offset += 3;
  }

  ifcode.push_back("off+" + std::to_string(offset));
  ifcode.push_back("off+" + std::to_string(offset));

  // add if scope code
  ifcode.insert(ifcode.end(), ifscopecode.begin(), ifscopecode.end());

  if (elsebodyTree != nullptr)
  {
    // when the if is executed we need to skip the else
    ifcode.push_back("jmp_cs_offset");
    // parse the scope code
    auto const elsescodescope = ParseScope(elsebodyTree, symbols);
    // get the size of the scope code to know how far to jump
    auto const elseOff = std::to_string(CountOffset(elsescodescope));
    ifcode.push_back("off+" + elseOff);
    ifcode.push_back("off+" + elseOff);

    ifcode.insert(ifcode.end(), elsescodescope.begin(), elsescodescope.end());
  }

  return ifcode;
}

std::vector<std::string> ParseWhile(std::shared_ptr<Node> const &node,
                                    SymbolsTable const &symbols,
                                    int const &scopeId)
{
  std::vector<std::string> whilecode;

  whilecode.push_back("# while loop");
  // while will be something like
  // expression
  // cmp
  // jne to outside the loop
  // loop body
  // jump to expression

  // expression will be evaluated and put in al
  auto const exp = ParseExpression8Bits(node->GetLeft(), symbols, scopeId);
  auto const expCount = CountOffset(exp);

  whilecode.insert(whilecode.end(), exp.begin(), exp.end());
  // if the condition is true there will be 1 in AL
  whilecode.push_back("cmp_operand_al");
  whilecode.push_back("%1");

  auto const whilebody = node->GetTree();

  if (whilebody != nullptr)
  {
    // if the condition is meet we don't jump and execute the  while body
    whilecode.push_back("jne_cs_offset");

    auto const whilebodyCode = ParseScope(whilebody, symbols);
    auto const whilebodyCount = CountOffset(whilebodyCode);

    // add three for the jump back that will be at the end of the body
    whilecode.push_back("off+" + std::to_string(whilebodyCount + 3));
    whilecode.push_back("off+" + std::to_string(whilebodyCount + 3));

    whilecode.insert(whilecode.end(), whilebodyCode.begin(),
                     whilebodyCode.end());

    // need to jump back to the expression
    whilecode.push_back("jmp_cs_offset");

    // from the top
    // +2 cmp_operand_al
    // +3 jne_cs_offset
    // +3 jne_cs_offset
    auto const offToTop = expCount + whilebodyCount + 8;
    whilecode.push_back("off-" + std::to_string(offToTop));
    whilecode.push_back("off-" + std::to_string(offToTop));
  }

  return whilecode;
}

std::vector<std::string>
ParseFunctionDefinition(std::shared_ptr<Node> const &node,
                        SymbolsTable const &symbols)
{
  std::vector<std::string> definitioncode;

  auto const ndFunName = node->GetLeft();
  auto const funName = ndFunName->GetValue();

  auto const ndFunType = ndFunName->GetLeft();
  auto const funType = ndFunType->GetValue();

  auto const treeArgs = ndFunName->GetTree();
  auto const ndArgs = treeArgs->GetNodes();

  definitioncode.push_back("# function definition for " + funName);

  // function must be skipped if it's not a call
  definitioncode.push_back("jmp_cs_offset");
  auto const treeBody = node->GetTree();

  auto funscope = ParseScope(treeBody, symbols);
  auto const offcount = CountOffset(funscope);
  // add 1 for return near
  definitioncode.push_back("off+" + std::to_string(offcount + 1));
  definitioncode.push_back("off+" + std::to_string(offcount + 1));
  // this is the entry point for the function call
  definitioncode.push_back("@" + funName);
  definitioncode.insert(definitioncode.end(), funscope.begin(), funscope.end());
  // return near
  definitioncode.push_back("retn");
  return definitioncode;
}

std::vector<std::string> ParseFunctionCall(std::shared_ptr<Node> const &node,
                                           SymbolsTable const &symbols,
                                           int const &scopeId)
{
  std::vector<std::string> callcode;

  auto const funName = node->GetValue();
  auto const argTree = node->GetTree();
  auto const argNodes = argTree->GetNodes();

  callcode.push_back("# function call for " + funName);

  // push arguments in reverse so they are easier to address
  for (size_t i = argNodes.size(); i > 0; i--)
  {
    auto const exp = ParseExpression8Bits(argNodes[i - 1], symbols, scopeId);
    callcode.insert(callcode.end(), exp.begin(), exp.end());
    callcode.push_back("push_al");
  }

  // call near
  callcode.push_back("call_cs_offset");
  callcode.push_back("!" + funName);
  callcode.push_back("!" + funName);

  // pop arguments in reverse so they are easier to address
  for (auto const &a : argNodes)
  {
    callcode.push_back("pop_al");
  }

  return callcode;
}

std::vector<std::string> ParseScope(std::shared_ptr<Tree> const &tree,
                                    SymbolsTable const &symbols)
{
  std::vector<std::string> scope;

  auto const scopeId = tree->GetScopeId();
  auto const [success, entries] = symbols.GetSymbolsInScope(scopeId);

  if (!success)
  {
    spdlog::error("Congratulazioni! This message should never be seen, please "
                  "open a issue on github this is a bug for sure! Please "
                  "attach your F7 code.");
    return scope;
  }

  scope.push_back("# start scope push variables");

  size_t countPush = 0;
  for (auto const entry : entries)
  {
    if (entry->symbolTp == SymbolType::Variable && entry->isAssigned &&
        entry->occurrences >= 1)
    {
      // push in the stack variables in the scope
      scope.push_back("push_al");
      countPush++;
      stackDisplacement[scopeId][entry->name] = countPush;
    }
  }
  scope.push_back("");

  for (auto const node : tree->GetNodes())
  {
    // ParseNode(node, symbols, code);
    switch (node->GetType())
    {
    case NodeType::Variable:
      // var declaration ignore
      break;
    case NodeType::Operator:
    {
      scope.push_back("# parse expression from line " +
                      std::to_string(node->GetLine()));
      auto const expression = ParseExpression8Bits(node, symbols, scopeId);
      scope.insert(scope.end(), expression.begin(), expression.end());
    }
    break;
    case NodeType::If:
    {
      scope.push_back("# parse if statement from line " +
                      std::to_string(node->GetLine()));
      auto const ifcode = ParseIf(node, symbols, scopeId);
      scope.insert(scope.end(), ifcode.begin(), ifcode.end());
    }
    break;
    case NodeType::While:
    {
      scope.push_back("# parse while statement from line " +
                      std::to_string(node->GetLine()));
      auto const whilecode = ParseWhile(node, symbols, scopeId);
      scope.insert(scope.end(), whilecode.begin(), whilecode.end());
    }
    break;
    case NodeType::Function:
    {
      scope.push_back("# parse function definition from line " +
                      std::to_string(node->GetLine()));
      auto const definitioncode = ParseFunctionDefinition(node, symbols);
      scope.insert(scope.end(), definitioncode.begin(), definitioncode.end());
    }
    break;
    case NodeType::FunctionCall:
    {
      scope.push_back("# parse function call from line " +
                      std::to_string(node->GetLine()));
      auto const callcode = ParseFunctionCall(node, symbols, scopeId);
      scope.insert(scope.end(), callcode.begin(), callcode.end());
    }
    break;
    default:
      spdlog::error("Invalid node type in tree: type {} value {}",
                    node->GetType(), node->GetValue());
      break;
    }
  }

  scope.push_back("# end scope clean stack");
  for (size_t i = 0; i < countPush; i++)
  {
    scope.push_back("pop_al");
  }
  scope.push_back("");

  stackDisplacement[scopeId].clear();

  return scope;
}

std::vector<std::string> Parser::ParseIR(std::string const &filename,
                                         Tree const &program,
                                         SymbolsTable const &symbols)
{
  stackDisplacement.clear();

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

std::vector<std::bitset<8>>
Parser::ParseMachineCode(std::vector<std::string> const &ir)
{
  std::vector<std::bitset<8>> machinecode;

  bool skipNext = false;
  for (auto const &entry : ir)
  {
    if (skipNext)
    {
      skipNext = false;
      continue;
    }

    if (entry == "" || entry.rfind("#", 0) == 0)
    {
      // skip
      continue;
    }

    if (entry.rfind("%", 0) == 0)
    {
      // immediate operand
      machinecode.push_back(atoi(entry.substr(1, entry.size()).c_str()));
      continue;
    }

    if (entry.rfind("off", 0) == 0)
    {
      // offset is 16 bits
      auto offset = machinecode.size() - 1;
      if (entry[3] == '+')
      {
        offset += atoi(entry.substr(4, entry.size()).c_str());
      }
      else
      {
        // +2 because it needs to consider that offset was counted without the actual offset
        // while this doesn't matter if we are jumping ahead it matter if we go back
        // TO DO validate the -1
        offset -= (atoi(entry.substr(4, entry.size()).c_str()) - 2 - 1);
      }

      machinecode.push_back(offset >> 8);
      machinecode.push_back(offset);
      skipNext = true;
      continue;
    }

    if (entry.rfind("^", 0) == 0)
    {
      machinecode.push_back(atoi(entry.substr(1, entry.size()).c_str()));
      continue;
    }

    if (entry.rfind("@", 0) == 0)
    {
      poundDisplacement[entry.substr(1, entry.size())] = machinecode.size() - 1;
      continue;
    }

    if (entry.rfind("!", 0) == 0)
    {
      auto const offset = poundDisplacement[entry.substr(1, entry.size())];
      machinecode.push_back(offset >> 8);
      machinecode.push_back(offset);
      skipNext = true;
      continue;
    }

    auto const [success, opcode] = StringToOpcode(entry);
    if (success)
    {
      machinecode.push_back((int)opcode);
    }
    else
    {
      spdlog::error("Unsupported instruction: {}", entry);
    }
  }

  return machinecode;
}