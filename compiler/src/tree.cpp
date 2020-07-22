#include "tree.h"
#include <iostream>
#include "symboltable.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

void Tree::AddNode(std::shared_ptr<Node> &node)
{
	if (node == nullptr)
	{
		return;
	}

	m_nodes.push_back(node);
}

const std::vector<std::shared_ptr<Node>> &Tree::GetNodes() const
{
	return m_nodes;
}

void Tree::SetScope(int scope)
{
	m_scopeid = scope;
}

int Tree::GetScopeId() const
{
	return m_scopeid;
}

void Tree::Print() const
{
	for (const auto &node : m_nodes)
	{
		std::cout << std::endl;
		node->Print();
		std::cout << std::endl;
	}
}

Node::Node() {}

Node::Node(const NodeType tp, const std::string &val, int line) : m_type(tp), m_value(val), m_line(line)
{
}

const std::shared_ptr<Node> &Node::GetLeft()
{
	return m_left;
}

const std::shared_ptr<Node> &Node::GetRight()
{
	return m_right;
}

const std::shared_ptr<Tree> &Node::GetTree()
{
	return m_tree;
}

NodeType Node::GetType()
{
	return m_type;
}

std::string Node::GetValue()
{
	return m_value;
}

void Node::SetType(NodeType tp)
{
	m_type = tp;
}

void Node::SetLeft(std::shared_ptr<Node> &node)
{
	m_left = node;
}

void Node::SetRight(std::shared_ptr<Node> &node)
{
	m_right = node;
}

void Node::SetValue(const std::string &val)
{
	m_value = val;
}

void Node::SetTree(std::shared_ptr<Tree> &tree)
{
	m_tree = tree;
}

int Node::GetLine()
{
	return m_line;
}

void Node::Print() const
{
	std::cout << "[ ";
	std::cout << m_value;
	std::cout << " ]";

	if (m_tree != nullptr)
	{
		std::cout << std::endl;
		std::cout << "{T-> ";
		m_tree->Print();
		std::cout << "} ";
	}

	if (m_left != nullptr)
	{
		std::cout << std::endl;
		std::cout << "{L-> ";
		m_left->Print();
		std::cout << "} ";
	}

	if (m_right != nullptr)
	{
		std::cout << std::endl;
		std::cout << "{R-> ";
		m_right->Print();
		std::cout << "} ";
	}
}