#pragma once

#include <ncurses.h>
#include "processor.h"

class Printer
{
public:
	Printer(const Processor &proc, const MemDevice &ramOne,
					const MemDevice &ramTwo, const MemDevice &videoMem,
					const MemDevice &eprom);
	~Printer();
	void Print();

private:
	WINDOW *m_win;
	const Processor &m_proc;
	const MemDevice &m_ramOne;
	const MemDevice &m_ramTwo;
	const MemDevice &m_eprom;
	const MemDevice &m_videoMem;
};
