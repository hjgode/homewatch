#ifndef NEWBYTEBUFFER_H
#define NEWBYTEBUFFER_H

#include "ByteBuffer.h"

class newByteBuffer:ByteBuffer
{
    public:
        newByteBuffer();
        virtual ~newByteBuffer();
        int push(byte in);
    protected:
    private:
        int iCount;
        ByteBuffer bytebuffer;
};

#endif // NEWBYTEBUFFER_H
