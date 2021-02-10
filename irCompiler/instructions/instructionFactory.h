#pragma once

#include "alloca.h"
#include "store.h"
#include "load.h"
#include "sext.h"
#include "functionDefinition.h"

namespace InstructionFactory {
 std::vector<Instruction*> MakeInstructions(std::vector<std::string> const& instructionsString);
}