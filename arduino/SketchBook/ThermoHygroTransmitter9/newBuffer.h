#ifndef NEWBUFFER_H
#define NEWBUFFER_H

#include "ByteBuffer.h"


class newBuffer
{
    public:
        newBuffer();
        ~newBuffer();
        void init(int s);
        ByteBuffer bytebuffer1;
        ByteBuffer bytebuffer2;

        byte peek1(unsigned int index);
        byte peek2(unsigned int index);
        int getSize1();
        int getSize2();
       	byte getMedian1();	//HGO
        byte getMax1();	//HGO
        byte getMin1();	//HGO
        byte getState1();	//HGO
        byte getTreshold1();	//HGO

       	byte getMedian2();	//HGO
        byte getMax2();	//HGO
        byte getMin2();	//HGO
        byte getState2();	//HGO
        void setTreshold(byte t);	//HGO
        byte getTreshold2();	//HGO

        int getDirection1();
        int getStateDuration1();
        int getDirection2();
        int getStateDuration2();
        void setMaxDuration1(int);
        void setMaxDuration2(int);

        int put(byte in);
        int push(byte);

    protected:
        int iCount;
        byte treshold;
        int capacity;

        int maxDuration;    //max duration (ie 5)
    private:
};

#endif // NEWBUFFER_H
