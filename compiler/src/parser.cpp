#include "parser.h"
#include <tuple>
#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

/*************************** Tree generation ***************************/

std::tuple<bool, std::shared_ptr<Node>> ParseStatement(const std::vector<Token> &tokens, std::vector<Token>::iterator &it);

bool ParseTree(const std::vector<Token> &tokens, Tree &tree, std::vector<Token>::iterator &it)
{
	auto [validnode, node] = ParseStatement(tokens, it);
	if (validnode)
	{
		tree.AddNode(node);
	}
	else
	{
		return false;
	}

	return true;
}

std::tuple<bool, std::shared_ptr<Node>> ReverseParseExpression(const std::vector<Token> &tokens, std::vector<Token>::iterator &it)
{
	std::shared_ptr<Node> expression = nullptr;
	std::shared_ptr<Node> lastOperand = nullptr;
	std::shared_ptr<Node> lastOperator = nullptr;
	while (it != tokens.end())
	{
		auto itType = it->GetType();
		if (itType == TokenType::Number || itType == TokenType::Variable)
		{
			if (lastOperand == nullptr)
			{
				lastOperand = std::make_shared<Node>(
						it->GetType() == TokenType::Number ? NodeType::Number : NodeType::Variable,
						it->GetValue(),
						it->GetLine());
			}
			else if (lastOperator != nullptr)
			{
				auto operand = std::make_shared<Node>(
						it->GetType() == TokenType::Number ? NodeType::Number : NodeType::Variable,
						it->GetValue(),
						it->GetLine());

				lastOperator->SetLeft(operand);

				if (expression == nullptr)
				{
					lastOperator->SetRight(lastOperand);
				}
				else
				{
					lastOperator->SetRight(expression);
				}

				expression = lastOperator;
				lastOperand = nullptr;
				lastOperator = nullptr;
			}
			else
			{
				spdlog::error("Invalid expression, unexpected token {} at line {}", it->GetValue(), it->GetLine());
				it++;
				return {false, nullptr};
			}
		}
		else if (itType == TokenType::Operator)
		{
			lastOperator = std::make_shared<Node>(NodeType::Operator, it->GetValue(), it->GetLine());
		}
		else if (itType == TokenType::Semicol || itType == TokenType::Then || itType == TokenType::Loop)
		{
			if (lastOperand != nullptr && lastOperator != nullptr)
			{
				lastOperator->SetLeft(lastOperand);

				if (expression == nullptr)
				{
					lastOperator->SetRight(lastOperand);
				}
				else
				{
					lastOperator->SetRight(expression);
				}

				expression = lastOperator;

				lastOperand = nullptr;
				lastOperator = nullptr;
			}

			return {true, expression};
		}
		else
		{
			break;
		}

		it++;
	}

	spdlog::error("Invalid expression, unexpected token {} at line {}", it->GetValue(), it->GetLine());
	it++;
	return {false, nullptr};
}

std::tuple<bool, std::shared_ptr<Node>> ParseExpression(const std::vector<Token> &tokens, std::vector<Token>::iterator &it)
{

	if (it->GetType() != TokenType::Number || it->GetType() != TokenType::Variable)
	{
		auto firstOp = std::make_shared<Node>(
				it->GetType() == TokenType::Number ? NodeType::Number : NodeType::Variable,
				it->GetValue(),
				it->GetLine());

		it++;

		if (it->GetType() == TokenType::Semicol ||
				it->GetType() == TokenType::Then ||
				it->GetType() == TokenType::Loop)
		{
			return {true, firstOp};
		}

		if (it->GetType() == TokenType::Assign || it->GetType() == TokenType::Operator)
		{
			auto op = std::make_shared<Node>(NodeType::Operator, it->GetValue(), it->GetLine());
			bool ret = false;
			if (it->GetType() == TokenType::Assign)
			{

				op->SetLeft(firstOp);
				it++;

				auto [success, expression] = ParseExpression(tokens, it);
				ret = success;
				if (success)
				{
					op->SetRight(expression);
				}
			}
			else
			{
				//TO DO FIX ME
				//this is because of an error in the design of the tree
				it--;
				auto [success, expression] = ReverseParseExpression(tokens, it);
				ret = success;

				op = expression;
			}

			return {ret, op};
		}
	}

	spdlog::error("Invalid expression, unexpected token {} at line {}", it->GetValue(), it->GetLine());
	it++;
	return {false, nullptr};
}

std::tuple<bool, std::shared_ptr<Node>> ParseDeclaration(const std::vector<Token> &tokens, std::vector<Token>::iterator &it)
{

	auto varType = std::make_shared<Node>(NodeType::VariableType, it->GetValue(), it->GetLine());
	it++;

	if (it->GetType() != TokenType::Variable)
	{
		spdlog::error("Variable must follow variable type, unexpected token {} at line {}", it->GetValue(), it->GetLine());
		it++;
		return {false, nullptr};
	}

	it++;
	if (it->GetType() == TokenType::Assign || it->GetType() == TokenType::Semicol)
	{
		//basically turning:
		// 	i8 x := 5;
		//into:
		//	i8 x;
		//	x = 5;
		//by re-parsing the var name
		it = it - 1;
	}
	else
	{
		spdlog::error("Assigment or semicolumn must follow a declaration, unexpected token {} at line {}", it->GetValue(), it->GetLine());
		return {false, nullptr};
	}

	auto varName = std::make_shared<Node>(NodeType::Variable, it->GetValue(), it->GetLine());
	varName->SetLeft(varType);
	return {true, varName};
}

std::tuple<bool, std::shared_ptr<Node>> ParseWhile(const std::vector<Token> &tokens, std::vector<Token>::iterator &it)
{
	auto whilenode = std::make_shared<Node>(NodeType::While, it->GetValue(), it->GetLine());
	it++;

	auto [validCond, condnode] = ParseExpression(tokens, it);
	if (!validCond)
	{
		spdlog::error("Failed to parse while condition at line {}", it->GetLine());
		return {false, nullptr};
	}

	whilenode->SetLeft(condnode);

	auto whilebodytree = Tree();

	while (it->GetType() != TokenType::EndLoop)
	{
		if (!ParseTree(tokens, whilebodytree, it))
		{
			return {false, nullptr};
		}

		if (it == tokens.end())
		{
			spdlog::error("Malformed while statement, missing endloop");
			return {false, nullptr};
		}
	}

	auto treeptr = std::make_shared<Tree>(whilebodytree);
	whilenode->SetTree(treeptr);

	return {true, whilenode};
}

std::tuple<bool, std::shared_ptr<Node>> ParseIf(const std::vector<Token> &tokens, std::vector<Token>::iterator &it)
{
	auto ifnode = std::make_shared<Node>(NodeType::If, it->GetValue(), it->GetLine());
	it++;
	auto [validCond, condnode] = ParseExpression(tokens, it);
	if (!validCond)
	{
		spdlog::error("Failed to parse if condition at line {}", it->GetLine());
		return {false, nullptr};
	}

	ifnode->SetLeft(condnode);

	auto bodies = std::make_shared<Node>(NodeType::IfBody, "Bodies", -1);
	ifnode->SetRight(bodies);

	auto ifbody = std::make_shared<Node>(NodeType::IfBody, "If body", -1);
	bodies->SetLeft(ifbody);

	auto ifbodyTree = Tree();
	while (it->GetType() != TokenType::Else && it->GetType() != TokenType::EndIf)
	{
		if (!ParseTree(tokens, ifbodyTree, it))
		{
			return {false, nullptr};
		}

		if (it == tokens.end())
		{
			spdlog::error("Malformed if statement, missing endif");
			return {false, nullptr};
		}
	}

	auto treeptr = std::make_shared<Tree>(ifbodyTree);
	ifbody->SetTree(treeptr);

	if (it->GetType() == TokenType::Else)
	{
		auto elsebody = std::make_shared<Node>(NodeType::ElseBody, "Else body", it->GetLine());
		bodies->SetRight(elsebody);

		auto elsebodyTree = Tree();
		while (it->GetType() != TokenType::EndIf)
		{
			if (!ParseTree(tokens, elsebodyTree, it))
			{
				return {false, nullptr};
			}

			if (it == tokens.end())
			{
				spdlog::error("Malformed if statement, missing endif");
				return {false, nullptr};
			}
		}

		treeptr = std::make_shared<Tree>(elsebodyTree);
		elsebody->SetTree(treeptr);
	}

	return {true, ifnode};
}

std::tuple<bool, std::shared_ptr<Node>> ParseStatement(const std::vector<Token> &tokens, std::vector<Token>::iterator &it)
{
	//Statements can only begin with
	// 1- variable type: variable declaration
	// 2- variable name: expression
	// 3- semicolumn: is ignored
	// 4- if: beginning of if statement
	// 5- while: beginning of while loop

	switch (it->GetType())
	{
	case TokenType::VariableType:
		return ParseDeclaration(tokens, it);
	case TokenType::Variable:
	case TokenType::Number:
		return ParseExpression(tokens, it);
	case TokenType::Semicol:
		//just move forward
		it++;
		return {true, nullptr};
	case TokenType::If:
		return ParseIf(tokens, it);
	case TokenType::While:
		return ParseWhile(tokens, it);
	case TokenType::Else:
	case TokenType::EndIf:
	case TokenType::Loop:
	case TokenType::EndLoop:
	case TokenType::Then:
		it++;
		//else and endif are at the end of the scope
		return {true, nullptr};

	case TokenType::Assign:
	case TokenType::Operator:
	case TokenType::Unknown:
	default:
		spdlog::error("Unexpected token {} at line {}", it->GetValue(), it->GetLine());
		it++;
		return {false, nullptr};
	}
}

std::tuple<bool, Tree> Parser::ParseProgram(std::vector<Token> &tokens)
{
	auto tree = Tree();
	auto it = tokens.begin();
	while (it != tokens.end())
	{
		if (!ParseTree(tokens, tree, it))
		{
			return {false, tree};
		}
	}

	return {true, tree};
}

/*************************** End tree generation ***************************/

/*************************** Symbols table generation ***************************/

bool IsVarType(const std::shared_ptr<Node> &node)
{
	if (node == nullptr)
	{
		return false;
	}

	return node->GetType() == NodeType::VariableType;
}

bool IsVarName(const std::shared_ptr<Node> &node)
{
	if (node == nullptr)
	{
		return false;
	}

	return node->GetType() == NodeType::Variable;
}

bool IsAssignment(const std::shared_ptr<Node> &node)
{
	if (node == nullptr)
	{
		return false;
	}

	return node->GetType() == NodeType::Assign;
}

bool IsOperator(const std::shared_ptr<Node> &node)
{
	if (node == nullptr)
	{
		return false;
	}

	return node->GetType() == NodeType::Operator;
}

bool IsDeclared(const std::shared_ptr<Node> &node, int scope, const SymbolTable &table)
{
	if (node == nullptr)
	{
		return false;
	}

	return table.IsDeclared(scope, node->GetValue());
}

bool IsAssigned(const std::shared_ptr<Node> &node, int scope, const SymbolTable &table)
{
	if (node == nullptr)
	{
		return false;
	}

	return table.IsAssigned(scope, node->GetValue());
}

bool IsUndefined(const std::shared_ptr<Node> &node, int scope, const SymbolTable &table)
{
	if (node == nullptr)
	{
		return false;
	}

	if (IsVarName(node))
	{
		auto val = node->GetValue();
		return !table.IsDeclared(scope, val) || !table.IsAssigned(scope, val);
	}

	if (node->GetType() == NodeType::Number)
	{
		return false;
	}

	auto left = node->GetLeft();
	auto right = node->GetRight();

	return IsUndefined(node->GetLeft(), scope, table) && IsUndefined(node->GetRight(), scope, table);
}

std::string GetExpressionType(const std::shared_ptr<Node> &node, int scope, const SymbolTable &table)
{
	//TO DO add more types
	auto nodeType = node->GetType();
	if (nodeType == NodeType::VariableType)
	{
		return node->GetValue();
	}

	if (nodeType == NodeType::Variable)
	{
		return table.GetType(scope, node->GetValue());
	}

	if (nodeType == NodeType::Number)
	{
		return "u8";
	}

	auto left = node->GetLeft();
	if (left != nullptr)
	{
		return GetExpressionType(left, scope, table);
	}

	return "Unknown";
}

bool ExpressionCheck(const std::shared_ptr<Node> &node, int scope, SymbolTable &table, bool isAssignment = false)
{
	auto left = node->GetLeft();
	auto right = node->GetRight();

	if (left == nullptr)
	{
		if (right != nullptr)
		{
			spdlog::warn("Malformed expression will ignore right operand {} at line {}", right->GetValue(), right->GetLine());
		}

		return true;
	}

	if (left == nullptr && right == nullptr)
	{
		return true;
	}

	bool leftUndef = IsUndefined(left, scope, table);
	bool rightUndef = IsUndefined(right, scope, table);
	if (leftUndef || rightUndef)
	{
		spdlog::error("Malformed expression type of '{}' is undefined at line {}",
									leftUndef ? left->GetValue() : right->GetValue(),
									leftUndef ? left->GetLine() : right->GetLine());
		return false;
	}

	auto lType = GetExpressionType(left, scope, table);
	auto rType = GetExpressionType(right, scope, table);

	if (std::strcmp(lType.c_str(), rType.c_str()) != 0 || std::strcmp(lType.c_str(), "Unknown") == 0)
	{
		spdlog::error("Invalid expression types mismatch {} {} {} {} at line {}",
									lType, left->GetValue(), rType, right->GetValue(),
									left->GetLine());
		return false;
	}

	return true;
}

std::tuple<bool, int> GenerateSymbolsForTree(const std::vector<std::shared_ptr<Node>> &nodes, SymbolTable &table);

bool GenerateSymbolsForNode(const std::shared_ptr<Node> &node, int scope, SymbolTable &table)
{
	if (node == nullptr)
	{
		return true;
	}

	auto left = node->GetLeft();
	if (left != nullptr)
	{
		if (!GenerateSymbolsForNode(left, scope, table))
		{
			return false;
		}
	}

	auto right = node->GetRight();
	if (right != nullptr)
	{
		if (!GenerateSymbolsForNode(right, scope, table))
		{
			return false;
		}
	}

	auto tree = node->GetTree();
	if (tree != nullptr)
	{
		auto nodes = tree->GetNodes();
		auto [success, scope] = GenerateSymbolsForTree(nodes, table);
		if (!success)
		{
			return false;
		}

		tree->SetScope(scope);
	}

	if (IsVarType(node))
	{
		//var declaration parent node will take care of it
		return true;
	}

	if (IsVarType(left) && IsVarName(node))
	{
		if (!table.AddVariable(left->GetValue(), node->GetValue(), scope))
		{
			spdlog::error("Line {}", node->GetLine());
			return false;
		}

		return true;
	}

	if (IsVarName(node))
	{
		if (!table.AddVarOccurrence(node->GetValue(), scope))
		{
			spdlog::error("Line {}", node->GetLine());
			return false;
		}
	}

	if (IsAssignment(node) || IsOperator(node))
	{
		if (IsVarName(left))
		{
			if (!table.AddVarOccurrence(left->GetValue(), scope, true))
			{
				spdlog::error("Line {}", node->GetLine());
				return false;
			}
		}

		return ExpressionCheck(left, scope, table, IsAssignment(node)) && ExpressionCheck(right, scope, table);
	}

	return true;
}

std::tuple<bool, int> GenerateSymbolsForTree(const std::vector<std::shared_ptr<Node>> &nodes, SymbolTable &table)
{
	auto scope = table.NewScope();
	if (nodes.size() == 0)
	{
		table.CloseScope();
		return {true, scope};
	}

	bool success = true;
	for (const auto &node : nodes)
	{
		success = GenerateSymbolsForNode(node, scope, table);
		if (!success)
		{ 
			table.CloseScope();
			return {false, 0};
		}
	}

	table.CloseScope();
	return {true, scope};
}

std::tuple<bool, SymbolTable> Parser::GenerateSymbolTable(Tree &tree)
{
	auto table = SymbolTable();

	auto nodes = tree.GetNodes();
	auto [success, scope] = GenerateSymbolsForTree(nodes, table);
	if (!success)
	{
		return {false, table};
	}

	tree.SetScope(scope);
	return {true, table};
}

void Parser::PrintTree(const Tree &tree)
{
	std::cout << "8888888888888888888888888888888888888888888888888888" << std::endl;
	std::cout << "------------------------TREE------------------------" << std::endl;
	tree.Print();
	std::cout << "8888888888888888888888888888888888888888888888888888" << std::endl;
}

/*************************** End symbols table generation ***************************/