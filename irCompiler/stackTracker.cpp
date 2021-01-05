#include "stackTracker.h"

bool StackTracker::AddFrame(std::string const &name) {
  m_frame = Frame();
  m_frame.varCount = 0;
  m_frame.functionName = name;
  return true;
}

void StackTracker::Push(unsigned short const size) {
  m_frame.varCount += size;
}

Frame StackTracker::GetFrame() {
  return m_frame;
}

bool StackTracker::RemoveFrame(std::string const &name) {
  return true;
}