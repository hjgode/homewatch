#include "newByteBuffer.h"
#include "ByteBuffer.h"

newByteBuffer::newByteBuffer()
{
    //ctor
    iCount=0;
    bytebuffer.init(5);
}

newByteBuffer::~newByteBuffer()
{
    //dtor
}

int newByteBuffer::push(byte in){
    iCount++;
    if(iCount==length){
        iCount=0;
        bytebuffer.push(in);
        if(length < capacity){
            // save data byte at end of buffer
            data[(position+length) % capacity] = in;
            // increment the length
            length++;
            return 1;
        }
        else{
            //move all one down
            unsigned int i=0;
            for(i=0; i<length-1; i++)
                data[i]=data[i+1];
            data[length-1]=in;
        }
	}
	return 0;
}
