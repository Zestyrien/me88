#pragma once
#include <vector>
#include <string>
#include <memory>
#include <tuple>
#include <unordered_map>

struct Entry
{
	std::string name;
	int occurrences;
	bool assigned;
	std::string type;
};

class Scope
{
private:
	int m_id;
	int m_parent;
	std::unordered_map<std::string, Entry> m_symbols;

public:
	Scope() = delete;
	Scope(int id, int parent);
	std::tuple<bool, Entry*> FindSymbol(const std::string &name);
	int GetParentId();
	bool AddVariable(const std::string &type, const std::string &name);
	bool AddVarOccurrence(const std::string &name, bool assignement = false);
	std::vector<Entry> GetSymbols();
	void Print() const;
};

class SymbolTable
{
private:
	int m_currentScope;
	int m_uid;
	std::unordered_map<int, Scope> m_scopes;
	Scope* GetScope(int scopeId);
	std::tuple<bool, Scope> GetScope(int scopeId) const;

public:
	SymbolTable();
	int NewScope();
	int CloseScope();
	bool AddVariable(const std::string &type, const std::string &name, const int scope);
	bool AddVarOccurrence(const std::string &name, const int scope, bool assignment = false);
	bool IsDeclared(const int scope, const std::string &name) const;
	bool IsAssigned(const int scope, const std::string &name) const;
	std::vector<Entry> GetEntriesInScope(int scopeid) const;
	std::string GetType(const int scope, const std::string &name) const;
	void Print() const;
	//std::string AddFunction(const std::string& funName, const std::vector<std::string>& args);
};
