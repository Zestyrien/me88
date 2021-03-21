#include "memdevice.h"
#include <ctime>
#include <sstream>
#include <vector>
#include <algorithm>

MemDevice::MemDevice(int from, int to, bool read, bool write, bool io, const std::vector<int> &mem) : m_addFrom(from), m_addTo(to), m_readable(read), m_writeable(write), m_IO(io)
{
  for (int i = 0; i < mem.size(); i++)
  {
    m_memory[from + i] = mem[i];
  }
}

MemDevice::MemDevice(int from, int to, bool read, bool write, bool io, const std::unordered_map<int, std::bitset<8>> &mem) : m_addFrom(from), m_addTo(to), m_readable(read), m_writeable(write), m_IO(io), m_memory(mem)
{
}

void MemDevice::Write(int to, const std::bitset<8> &data)
{

  if (!m_writeable || !IsAddressInRange(to))
    return;

  m_memory[to] = data;
}

std::bitset<8> MemDevice::Read(int from)
{

  if (!m_readable || !IsAddressInRange(from))
  {
    return std::rand() % 255;
  }

  auto it = m_memory.find(from);
  if (it != m_memory.end())
  {
    return m_memory[from];
  }
  else
  {
    std::srand(std::time(nullptr));
    m_memory[from] = std::rand() % 255;
    return m_memory[from];
  }
}

bool MemDevice::IsReadOnly()
{
  return !m_writeable;
}

bool MemDevice::IsWriteOnly()
{
  return !m_readable;
}

bool MemDevice::IsIO()
{
  return m_IO;
}

bool MemDevice::IsAddressInRange(int add) const
{
  return add >= m_addFrom && add <= m_addTo;
}

std::string MemDevice::Dump(std::string title, bool caracters) const
{
  std::vector<std::pair<int, std::bitset<8>>> vect(m_memory.begin(), m_memory.end());
  std::sort(vect.begin(), vect.end(),
            [](std::pair<int, std::bitset<8>> a, std::pair<int, std::bitset<8>> b) {
              return a.first > b.first;
            });

  std::stringstream stream;
  stream << "Dump " << title << ": ";
  for (const auto &loc : vect)
  {
    if (!caracters)
    {
      stream << "[" << loc.first << "] ";
      stream << loc.second.to_ulong() << " ";
    }
    else
    {
      stream << loc.second.to_ulong();
    }
  }
  stream << "\n";
  return stream.str();
}