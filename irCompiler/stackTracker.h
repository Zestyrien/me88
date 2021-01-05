#pragma once

#include <string>

struct Frame {
  unsigned short varCount;
  std::string functionName;
};

class StackTracker {
public:
  bool AddFrame(std::string const& name);
  void Push(unsigned short const size);
  Frame GetFrame();
  bool RemoveFrame(std::string const& name);
private:
  Frame m_frame;
};