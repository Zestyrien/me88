#include "microPC.h"
#include <fstream>
#include <iostream>
#include "processor.h"
#include "bus.h"
#include "memdevice.h"
#include "printer.h"

#define RAM_ONE_START 0x00000
#define RAM_ONE_END 0x9FFFF

#define VID_MEM_START 0xA0000
#define VID_MEM_END 0xAFFFF

#define RAM_TWO_START 0xB000
#define RAM_TWO_END 0xEFFFF

#define EPROM_START 0xF0000
#define EPROM_END 0xFFFFF

std::vector<int> LoadProgram(const std::string &filename)
{
  std::vector<int> eprom;
  std::ifstream file;
  file.open(filename);
  if (file.is_open())
  {
    std::string codeline;
    while (std::getline(file, codeline))
    {
      eprom.push_back(std::stoi(codeline, 0, 2));
    }
  }

  return eprom;
}

std::unordered_map<int, std::bitset<8>> LoadInterrupts()
{
  std::unordered_map<int, std::bitset<8>> interrupts;
  //TO DO make something scalable
  //interrupt table at the top of the RAM
  //interrupt type 0
  interrupts[0] = 0x00;       //IP high part for the interrupt program
  interrupts[1] = 0x00;       //IP low part for the interrupt program
  interrupts[2] = 0x00;       //CS high part for the interrupt program
  interrupts[3] = 0b00000001; //CS low part for the interrupt program

  auto int0Program = LoadProgram("../../programs/type0interrupt.asm.bin");

  auto address = (interrupts[1].to_ulong() << 4) + interrupts[0].to_ulong();
  for (int i = 0; i < int0Program.size(); i++)
  {
    interrupts[address + i] = int0Program[i];
  }

  return interrupts;
}

void microPC::PowerOn(bool debugging)
{
  Bus bus;

  MemDevice eprom(EPROM_START, EPROM_END, true, false, false, LoadProgram("../programs/eprom.F7.bin"));
  MemDevice ramOne(RAM_ONE_START, RAM_ONE_END, true, true, false);
  MemDevice ramTwo(RAM_TWO_START, RAM_TWO_END, true, true, false);
  MemDevice vidMem(VID_MEM_START, VID_MEM_END, false, true, true);

  bus.RegisterDevice(eprom);
  bus.RegisterDevice(ramOne);
  bus.RegisterDevice(vidMem);
  bus.RegisterDevice(ramTwo);

  auto processor = Processor(bus);

  auto printer = Printer(processor, ramOne, ramTwo, vidMem, eprom);
  processor.OnReset();
  bool end = false;
  while (!end)
  {
    printer.Print();
    if (debugging)
    {
      auto c = getchar();
      if (c == 'q')
      {
        end = true;
      }
    }

    processor.OnClock();
  }
}