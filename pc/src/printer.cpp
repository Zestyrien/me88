#include "printer.h"

Printer::Printer(const Processor &proc, const MemDevice &ramOne, const MemDevice &ramTwo,
                 const MemDevice &videoMem, const MemDevice &eprom)
    : m_proc(proc), m_ramOne(ramOne), m_ramTwo(ramTwo),
      m_eprom(eprom), m_videoMem(videoMem)
{
  initscr();
  m_win = newwin(500, 500, 0, 0);
}

Printer::~Printer()
{
  endwin();
}

void Printer::Print()
{
  wmove(m_win, 0, 0);

  auto status = m_proc.GetStatus();

  wprintw(m_win,
          "STAR = %i MJR = %i \n\n",
          status.star, status.mjr);
  wprintw(m_win,
          "CS = %i IP = %i OPCODE = %i SOURCE = %i AL = %i\nDEST_SEL = %i DEST_OFF = %i\n\n",
          status.cs, status.ip, status.opcode, status.source, status.al, status.destSel, status.destOff);
  wprintw(m_win,
          "SS = %i SP = %i BP = %i DS = %i DI = %i d7_d0 = %i\n\n",
          status.ss, status.sp, status.bp, status.ds, status.di, status.d7d0);
  wprintw(m_win,
          "CF = %d OF = %d SF = %d ZF = %d\n\n",
          status.cf, status.of, status.sf, status.zf);
  wprintw(m_win,
          std::string("MAR = " + status.mar + " MBR = %i MR_ = %i MW_ = %i\n\n").c_str(),
          status.mbr, status.mr_, status.mw_);

  wprintw(m_win, m_eprom.Dump("Eprom").c_str());
  wprintw(m_win, m_ramOne.Dump("RAMOn").c_str());
  wprintw(m_win, m_ramTwo.Dump("RAMTW").c_str());
  wprintw(m_win, m_videoMem.Dump("Video", true).c_str());

  wrefresh(m_win);
}
