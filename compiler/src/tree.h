#pragma once

#include <string>
#include <memory>
#include <vector>
#include <tuple>

enum class NodeType
{
	Empty = 0,
	VariableType,
	Variable,
	Assign,
	Number,
	Operator,
	If,
	Bodies,
	IfBody,
	ElseBody,
	While,
	WhileBody
};

class Node;
class Scope;

class Tree
{
private:
	std::vector<std::shared_ptr<Node>> m_nodes;
	int m_scopeid;

public:
	void AddNode(std::shared_ptr<Node>& node);
	const std::vector<std::shared_ptr<Node>>& GetNodes() const;
	void SetScope(int scope);
	int GetScopeId() const;
	void Print() const;
};

class Node
{
private:
	std::shared_ptr<Node> m_left;
	std::shared_ptr<Node> m_right;
	std::shared_ptr<Tree> m_tree;
	NodeType m_type;
	std::string m_value;
	int m_line;

public:
	Node();
	Node(const NodeType tp, const std::string &val, int line);
	const std::shared_ptr<Node>& GetLeft();
	const std::shared_ptr<Node>& GetRight();
	const std::shared_ptr<Tree>& GetTree();
	NodeType GetType();
	std::string GetValue();
	void SetType(NodeType tp);
	void SetLeft(std::shared_ptr<Node>& node);
	void SetRight(std::shared_ptr<Node>& node);
	void SetValue(const std::string &val);
	void SetTree(std::shared_ptr<Tree>& tree);
	int GetLine();
	void Print() const;
};