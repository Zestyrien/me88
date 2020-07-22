#pragma once
#include <bitset>
#include "memdevice.h"
#include <vector>

class Bus
{
public:
	Bus();
	void RegisterDevice(MemDevice &device);
	void Write(int to, std::bitset<8> data);
	std::bitset<8> Read(int from);

private:
	std::vector<MemDevice *> m_devices;
	MemDevice *GetDevice(int address);
};