#include "symboltable.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

Scope::Scope(int id, int parent) : m_id(id), m_parent(parent)
{
}

std::tuple<bool, Entry*> Scope::FindSymbol(const std::string &name)
{
	if (m_symbols.find(name) == m_symbols.end())
	{
		return {false, nullptr};
	}
	else
	{
		return {true, &m_symbols[name]};
	}
}

int Scope::GetParentId()
{
	return m_parent;
}

bool Scope::AddVariable(const std::string &type, const std::string &name)
{
	//Don't look for variables in parents scope
	auto [success, entry] = FindSymbol(name);
	if (!success)
	{
		m_symbols[name] = Entry();
		m_symbols[name].name = name;
		m_symbols[name].type = type;
		m_symbols[name].occurrences = 0; //0 = declared but never used
		return true;
	}

	spdlog::error("{} is already declared in this scope.", name);
	return false;
}

bool Scope::AddVarOccurrence(const std::string &name, bool assignement)
{

	auto [success, entry] = FindSymbol(name);
	if (success)
	{
		if (assignement)
		{
			entry->assigned = true;
		}
		else
		{
			entry->occurrences = entry->occurrences + 1;
		}

		return true;
	}

	return false;
}

std::vector<Entry> Scope::GetSymbols()
{
	std::vector<Entry> symbols;
	for(auto sy : m_symbols)
	{
		symbols.push_back(sy.second);
	}
	return symbols;
}

void Scope::Print() const
{
	std::cout << "----------------------Scope " << m_id
						<< "------------------------------------" << std::endl;
	std::cout << "Parent: [" << m_parent << "]" << std::endl;
	std::cout << m_symbols.size()
						<< " symbols defined in the scope." << std::endl;

	if (m_symbols.size() > 0)
	{
		for (const auto &sy : m_symbols)
		{
			std::cout << "******************"
								<< "Name: [" << sy.second.name << "] "
								<< "************************************" << std::endl;
			std::cout << "Type: [" << sy.second.type << "] ";
			std::cout << "Name: [" << sy.second.name << "] ";
			std::cout << "Occurrences: [" << sy.second.occurrences << "] ";
			std::string assigned = sy.second.assigned ? "true" : "false";
			std::cout << "Assigned: [" << assigned << "]" << std::endl;
		}
	}
}

SymbolTable::SymbolTable() : m_currentScope(0), m_uid(0)
{
}

int SymbolTable::NewScope()
{
	m_scopes.emplace( m_uid, Scope(m_uid, m_currentScope));
	m_currentScope = m_uid;
	m_uid++;
	return m_currentScope;
}

int SymbolTable::CloseScope()
{
	auto scope = GetScope(m_currentScope);
	if(scope == nullptr)
	{ 
		spdlog::warn("Invalid close scope.");
		return 0;
	}

	m_currentScope = scope->GetParentId();
	return m_currentScope;
}

Scope* SymbolTable::GetScope(int scopeId)
{
	for (auto &sc : m_scopes)
	{
		if (sc.first == scopeId)
		{
			return &sc.second;
		}
	}
	return nullptr;
}

std::tuple<bool, Scope> SymbolTable::GetScope(int scopeId) const
{
	for (auto &sc : m_scopes)
	{
		if (sc.first == scopeId)
		{
			return {true, sc.second};
		}
	}
	return {false, Scope(0,0)};
}

bool SymbolTable::AddVariable(const std::string &type, const std::string &name, const int scopeId)
{

	auto scope = GetScope(scopeId);
	if (scope != nullptr)
	{
		return scope->AddVariable(type, name);
	}

	return false;
}

bool SymbolTable::AddVarOccurrence(const std::string &name, const int scopeId, bool assignment)
{
	auto scope = GetScope(scopeId);
	if (scope != nullptr)
	{
		auto [success, entry] = scope->FindSymbol(name);

		if (success)
		{
			return scope->AddVarOccurrence(name, assignment);
		}

		auto parent = scope->GetParentId();
		if (parent != scopeId && scopeId != 0)
		{
			return AddVarOccurrence(name, parent, assignment);
		}
	}

	spdlog::error("{} in scope {} not declared.", name, scopeId);

	return false;
}

bool SymbolTable::IsDeclared(const int scopeId, const std::string &name) const
{
	auto [valid, scope] = GetScope(scopeId);
	if (!valid)
	{
		return false;
	}

	auto [success, entry] = scope.FindSymbol(name);
	if (success)
	{
		return true;
	}

	return IsDeclared(scope.GetParentId(), name);
}

bool SymbolTable::IsAssigned(const int scopeId, const std::string &name) const
{
	auto [valid, scope] = GetScope(scopeId);
	if (!valid)
	{
		return false;
	}

	auto [success, entry] = scope.FindSymbol(name);
	if (success)
	{
		return entry->assigned;
	}

	return IsAssigned(scope.GetParentId(), name);
}

std::vector<Entry> SymbolTable::GetEntriesInScope(int scopeid) const
{
	auto [success, scope] = GetScope(scopeid);
	if(!success)
	{
		spdlog::warn("GetEntriesInScope scope not found.");
		return std::vector<Entry>();
	}

	return scope.GetSymbols();
}

std::string SymbolTable::GetType(const int scopeId, const std::string &name) const
{
	auto [valid, scope] = GetScope(scopeId);
	if (!valid)
	{
		return "Unknown";
	}

	auto [success, entry] = scope.FindSymbol(name);
	if (!success && scopeId == 0)
	{
		spdlog::error("Variable not found in the scope.");
		return "Unknown";
	}
	else if(!success)
	{
		return GetType(scope.GetParentId(), name);
	}
	
	return entry->type;
}

void SymbolTable::Print() const
{
	std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << "-------------------SYMBOLS  TABLE-------------------" << std::endl;
	for (const auto &sc : m_scopes)
	{
		sc.second.Print();
	}
	std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}