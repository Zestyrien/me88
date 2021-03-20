#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

enum class NodeType
{
  Empty = 0,
  Type,
  Variable,
  Function,
  FunctionArgs,
  FunctionBody,
  FunctionCall,
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

class Tree
{
private:
  std::vector<std::shared_ptr<Node>> m_nodes;
  int m_scopeId;
  int m_parentScopeId;

public:
  Tree() = delete;
  Tree(int parentScope);
  void AddNode(std::shared_ptr<Node> &node);
  const std::vector<std::shared_ptr<Node>> &GetNodes() const;
  int GetScopeId() const;
  int GetParentScopeId() const;
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
  const std::shared_ptr<Node> &GetLeft();
  const std::shared_ptr<Node> &GetRight();
  const std::shared_ptr<Tree> &GetTree();
  NodeType GetType();
  std::string GetValue();
  void SetType(NodeType tp);
  void SetLeft(std::shared_ptr<Node> &node);
  void SetRight(std::shared_ptr<Node> &node);
  void SetTree(std::shared_ptr<Tree> &scope);
  void SetValue(const std::string &val);
  int GetLine();
  void Print() const;
};
