//ifanbuffer.h

#ifndef _IFANBUFFER_H_
#define _IFANBUFFER_H_

#include "Arduino.h"

const int buf_length = 5;

class iFanBuffer{
  private:
    int interval10;
    byte buffer1[buf_length];
    byte buffer10[buf_length];
    unsigned int iCount;
    unsigned int iCount10;
    void dumpBuffer(byte*, byte);
    void add10(byte b);
    void initBuffer(byte* buf, int cnt);
    
  public:
    iFanBuffer();
    ~iFanBuffer();
    void add(byte b);
    byte getOnOff();
    void dump1();
    void dump10();
};

#endif
