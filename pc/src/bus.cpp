#include "bus.h"
#include <ctime>

Bus::Bus()
{
}

void Bus::RegisterDevice(MemDevice& device)
{
	m_devices.push_back(&device);
}

void Bus::Write(int to, std::bitset<8> data)
{
	auto dev = GetDevice(to);
	
	if (dev != nullptr)
	{
		dev->Write(to, data);
	}
}

std::bitset<8> Bus::Read(int from)
{
	auto dev = GetDevice(from);
	if (dev != nullptr)
	{
		return dev->Read(from);
	}

	std::srand(std::time(nullptr));
	return std::rand() % 255;
}

MemDevice* Bus::GetDevice(int address)
{
	for (auto dev : m_devices)
	{
		if (dev->IsAddressInRange(address))
			return dev;
	}

	return nullptr;
}
