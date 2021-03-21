#pragma once
#include "memdevice.h"
#include <bitset>
#include <unordered_map>
#include <vector>

class MemDevice
{
public:
  MemDevice() = delete;
  MemDevice(int from, int to, bool read, bool write, bool io, const std::vector<int> &mem = std::vector<int>());
  MemDevice(int from, int to, bool read, bool write, bool io, const std::unordered_map<int, std::bitset<8>> &mem);
  void Write(int to, const std::bitset<8> &data);
  std::bitset<8> Read(int from);
  bool IsReadOnly();
  bool IsWriteOnly();
  bool IsIO();
  bool IsAddressInRange(int add) const;
  std::string Dump(std::string title, bool caracters = false) const;

private:
  int m_addFrom;
  int m_addTo;
  bool m_readable;
  bool m_writeable;
  bool m_IO;
  std::unordered_map<int, std::bitset<8>> m_memory;
};
