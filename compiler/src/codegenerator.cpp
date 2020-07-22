#include "codegenerator.h"
#include <unordered_map>
#include <bitset>
#include <iostream>
#include "../../common/opcode.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace CodeGen
{
	std::shared_ptr<std::unordered_map<std::string, std::bitset<16>>> varToAddr;
	std::shared_ptr<std::bitset<16>> sp;

	Code::Code(std::shared_ptr<std::unordered_map<std::string, std::bitset<16>>> &varToAddr,
						 std::shared_ptr<std::bitset<16>> &sp) : m_varToAddr(varToAddr),
																										 m_sp(sp) {}

	std::size_t Code::Size() const
	{
		return m_code.size();
	}

	std::vector<std::bitset<8>> Code::GetCode() const
	{
		return m_code;
	}

	void Code::Concat(const Code &othercode)
	{
		for (auto line : othercode.GetCode())
		{
			m_code.push_back(line);
		}
	}

	void Code::Print() const
	{
	}

	void Code::Hlt()
	{
		m_code.push_back((int)Opcode::htl_code);
	}

	void Code::Push()
	{
		*m_sp = m_sp->to_ulong() - 1;
		m_code.push_back((int)Opcode::push_al);
	}

	void Code::AddToStack(const std::string &variable)
	{
		Push();
		(*m_varToAddr)[variable] = *m_sp;
	}

	void Code::Pop()
	{
		m_code.push_back((int)Opcode::pop_al);
		*m_sp = m_sp->to_ulong() + 1;
	}

	void Code::RemoveFromStack(const std::string &variable)
	{
		m_varToAddr->erase(variable);
		Pop();
	}

	void Code::ChangeSign()
	{
		m_code.push_back((int)Opcode::not_al);
		Add(1);
	}

	void Code::Assignement(const std::string &variable)
	{
		LoadVariableDSDI(variable);
		m_code.push_back((int)Opcode::mov_al_ds$di);
		LoadAL(1);
	}

	void Code::LoadVariableDSDI(const std::string &variable)
	{
		Push(); //save al content

		//set ds
		m_code.push_back((int)Opcode::mov_ss_ax);
		m_code.push_back((int)Opcode::mov_ax_ds);

		//set di
		LoadAX((*m_varToAddr)[variable]);
		m_code.push_back((int)Opcode::mov_ax_di);

		Pop(); //restore al content
	}

	void Code::LoadAX(std::bitset<16> imm)
	{
		auto h = imm.to_ulong() >> 8;
		LoadAL(h);
		m_code.push_back((int)Opcode::mov_al_ah);
		auto l = imm.to_ulong();
		LoadAL(l);
	}

	void Code::LoadAL(std::bitset<8> imm)
	{
		m_code.push_back((int)Opcode::mov_operand_al);
		LoadOperand(imm);
	}

	void Code::LoadAL(const std::string &variable)
	{
		LoadVariableDSDI(variable);
		m_code.push_back((int)Opcode::mov_ds$di_al);
	}

	void Code::LoadOperand(std::bitset<8> op)
	{
		m_code.push_back(op);
	}

	void Code::LoadOffset(std::bitset<16> offset)
	{
		//+2 because we have to consider the offset itself
		int off = offset.to_ulong() + 2;
		m_code.push_back(off);
		m_code.push_back(off >> 8);
	}

	void Code::AddDSDI(const std::string &variable)
	{
		LoadVariableDSDI(variable);
		m_code.push_back((int)Opcode::add_ds$di_al);
	}

	void Code::SubDSDI(const std::string &variable)
	{
		LoadVariableDSDI(variable);
		m_code.push_back((int)Opcode::sub_ds$di_al);
	}

	void Code::CmpDSDI(const std::string &variable)
	{
		LoadVariableDSDI(variable);
		m_code.push_back((int)Opcode::cmp_ds$di_al);
	}

	void Code::SetALEval()
	{
		//Eval code looks like
		//...
		// ja/jb/je true_
		//false_:
		//mov 0,al
		//jmp end_
		//true_:
		//mov 1,al
		//end_:
		//...

		//if the condition is matched
		//skip mov 0,al (2 bytes) and skip jmp offset (3 bytes)
		LoadOffset(Size() + 5);
		LoadAL((std::bitset<8>)0);
		//skip mov 1,al (2 bytes)
		m_code.push_back((int)Opcode::jmp_cs_offset);
		LoadOffset(Size() + 2); //skip load al
		LoadAL(1);
	}

	void Code::EvalAboveDSDI(const std::string &variable)
	{
		CmpDSDI(variable);
		m_code.push_back((int)Opcode::ja_cs_offset);
		SetALEval();
	}

	void Code::Add(int op)
	{
		m_code.push_back((int)Opcode::add_operand_al);
		LoadOperand(op);
	}

	void Code::Sub(int op)
	{
		m_code.push_back((int)Opcode::sub_operand_al);
		LoadOperand(op);
	}

	void Code::Cmp(int op)
	{
		m_code.push_back((int)Opcode::cmp_operand_al);
		LoadOperand(op);
	}

	void Code::JmpNE(int offset)
	{
		m_code.push_back((int)Opcode::jne_cs_offset);
		LoadOffset(offset);
	}

	void Code::Jmp(int offset)
	{
		m_code.push_back((int)Opcode::jmp_cs_offset);
		LoadOffset(offset);
	}

	void Code::EvalAbove(int op)
	{
		Cmp(op);
		m_code.push_back((int)Opcode::jb_cs_offset);
		SetALEval();
	}

	void Code::CondBodies(const Code &condTrue, const Code &condFalse)
	{
		Cmp(1);
		if (condFalse.Size() > 0 && condTrue.Size() > 0)
		{
			m_code.push_back((int)Opcode::je_cs_offset);
			//if we don't jump the condition is false
			LoadOffset(Size() + condFalse.Size() + 3);
			Concat(condFalse);

			//skip true if the condition was false
			m_code.push_back((int)Opcode::jmp_cs_offset);

			LoadOffset(Size() + condTrue.Size());

			//if the condition is true we jump here
			Concat(condTrue);
		}
		else if (condFalse.Size() == 0 && condTrue.Size() > 0)
		{
			m_code.push_back((int)Opcode::jne_cs_offset);
			//if it is equal don't jump and execute the true
			LoadOffset(Size() + condTrue.Size());
			Concat(condTrue);
		}
		else if (condFalse.Size() > 0 && condTrue.Size() == 0)
		{
			m_code.push_back((int)Opcode::je_cs_offset);
			//if it not is equal don't jump and execute the false
			LoadOffset(Size() + condFalse.Size());
			Concat(condFalse);
		}
	}

	void ParseExpression8Bits(const std::shared_ptr<Node> &node, Code &code)
	{
		//expressions leave the result in al
		auto ndvalue = node->GetValue();
		auto ndtype = node->GetType();
		if (ndtype == NodeType::Number)
		{
			code.LoadAL(std::stoi(ndvalue));
			return;
		}

		if (ndtype == NodeType::Variable)
		{
			code.LoadAL(ndvalue);
			return;
		}

		auto right = node->GetRight();
		if (right != nullptr)
		{
			//evaluate the right expression
			ParseExpression8Bits(right, code);
		}

		auto left = node->GetLeft();
		//Assignement
		if (ndvalue == ":=")
		{
			//assignement must have a variable on the left
			//and an expression on the right
			code.Assignement(left->GetValue());
			return;
		}

		//this is dependent of what is on the right
		//whatever was in the left was evaluated and
		//it is sitting in al
		auto lftype = left->GetType();
		auto lfvalue = left->GetValue();
		if (lftype == NodeType::Variable)
		{
			if (ndvalue == "+")
			{
				code.AddDSDI(lfvalue);
			}
			else if (ndvalue == "-")
			{
				code.SubDSDI(lfvalue);
			}
			else if (ndvalue == ">") 
			{
				code.EvalAboveDSDI(lfvalue);
			}
		}
		else
		{
			if (ndvalue == "+")
			{
				code.Add(std::stoi(lfvalue));
			}
			else if (ndvalue == "-")
			{
				code.Sub(std::stoi(lfvalue));
			}
			else if (ndvalue == ">") 
			{
				code.EvalAbove(std::stoi(lfvalue));
			}
		}
	}

	void ParseScope(const std::shared_ptr<Tree> &tree, const SymbolTable &symbols, Code &code);

	void ParseWhile(const std::shared_ptr<Node> &node, const SymbolTable &symbols, Code &code)
	{
		auto whilebody = node->GetTree();

		Code whilebodycode(varToAddr, sp);
		if (whilebody != nullptr)
		{
			ParseScope(whilebody, symbols, whilebodycode);
		}

		int conditionoffset = code.Size();
		//condition will be evaluated and put in al
		ParseExpression8Bits(node->GetLeft(), code);

		code.Cmp(1);
		code.JmpNE(code.Size() + whilebodycode.Size() + 3); //3 is the jump back instruction

		code.Concat(whilebodycode);

		code.Jmp(conditionoffset - 2); //TO DO Why do I have to subtract 2?

		code.LoadOffset(conditionoffset - 2);
	}

	void ParseIf(const std::shared_ptr<Node> &node, const SymbolTable &symbols, Code &code)
	{
		auto ndbodies = node->GetRight();
		auto ifbody = ndbodies->GetLeft()->GetTree();

		Code ifbodycode(varToAddr, sp);
		if (ifbody != nullptr)
		{
			ParseScope(ifbody, symbols, ifbodycode);
		}

		Code elsebodycode(varToAddr, sp);
		auto ndbodieselse = ndbodies->GetRight();
		if (ndbodieselse != nullptr)
		{
			auto elsebody = ndbodieselse->GetTree();

			if (elsebody != nullptr)
			{
				ParseScope(elsebody, symbols, elsebodycode);
			}
		}

		//condition will be evaluated and put in al
		ParseExpression8Bits(node->GetLeft(), code);

		code.CondBodies(ifbodycode, elsebodycode);
	}

	void ParseNode(const std::shared_ptr<Node> &node, const SymbolTable &symbols, Code &code)
	{
		switch (node->GetType())
		{
		case NodeType::Variable:
			//var declaration ignore
			break;
		case NodeType::Operator:
			//TO DO add more ParseExpression based on types
			ParseExpression8Bits(node, code);
			break;
		case NodeType::If:
			ParseIf(node, symbols, code);
			break;
		case NodeType::While:
			ParseWhile(node, symbols, code);
			break;
		default:
			spdlog::error("Invalid node type in tree: type {} value {}", node->GetType(), node->GetValue());
			break;
		}
	}

	void ParseScope(const std::shared_ptr<Tree> &tree, const SymbolTable &symbols, Code &code)
	{
		auto scopeId = tree->GetScopeId();
		auto entries = symbols.GetEntriesInScope(scopeId);

		for (auto entry : entries)
		{
			//add variables in the scope
			code.AddToStack(entry.name);
		}

		for (auto node : tree->GetNodes())
		{
			ParseNode(node, symbols, code);
		}

		for (auto entry : entries)
		{
			//remove variables from the scope
			code.RemoveFromStack(entry.name);
		}
	}

	std::vector<std::bitset<8>> GenerateCode(const Tree &program, const SymbolTable &symbols)
	{
		varToAddr = std::make_shared<std::unordered_map<std::string, std::bitset<16>>>();
		sp = std::make_shared<std::bitset<16>>();
		auto code = Code(varToAddr, sp);
		ParseScope(std::make_shared<Tree>(program), symbols, code);
		code.Hlt();

		code.Print();

		return code.GetCode();
	}

	void Print(const std::vector<std::bitset<8>>& code)
	{
		std::cout << "££££££££££££££££ Code Debug ££££££££££££££££" << std::endl;
		int count = 0;
		for (const auto& st : code)
		{
			std::cout << count << "\t" << st << "\t" << st.to_ulong() << std::endl;
			count++;
		}

		std::cout << "££££££££££££££££££££££££££££££££££££££££££££" << std::endl;
	}
} // namespace CodeGen