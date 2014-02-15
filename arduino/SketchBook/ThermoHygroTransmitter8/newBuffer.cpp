#include "newBuffer.h"

/** setup two stack buffers with overflow
 *
 * the second buffer will only be filled by push/put
 * on every x iteration
 * the idea is to fill buf1 every minute and buf2 every 5 minutes
 * if max-min>treshold then state=1
 * use buf1 to watch for fast changes
 * use buf2 to watch for slow changes and get median values
 */
newBuffer::newBuffer()
{
    //ctor
    iCount=0;
    treshold=5;
    bytebuffer1.setTreshold(treshold);
    bytebuffer2.setTreshold(treshold);
    maxDuration=5;
}

newBuffer::~newBuffer()
{
    //dtor
}

void newBuffer::init(int s){
    capacity=s;
    bytebuffer1.init(s);
    bytebuffer2.init(s);
}

int newBuffer::put(byte in){
    push(in);
	return 0;
}

int newBuffer::push(byte in){
    //static byte lastByte;
    iCount++;
    bytebuffer1.push(in);
    if(iCount==capacity){
        bytebuffer2.push(in);
        iCount=0;
    }
    return 0;
}


byte newBuffer::getState1(){
    return bytebuffer1.getState();
}
/** state is 1 if max-min > treshold
 *
 * state=1 if max - min> treshold
 */
byte newBuffer::getState2(){
    return bytebuffer2.getState();
}


int newBuffer::getStateDuration1(){
    return bytebuffer1.getStateDuration();
}
int newBuffer::getStateDuration2(){
    return bytebuffer2.getStateDuration();
}

int newBuffer::getDirection1(){
    return bytebuffer1.getDirection();
}
int newBuffer::getDirection2(){
    return bytebuffer2.getDirection();
}

byte newBuffer::getMedian1(){
    return bytebuffer1.getMedian();
}
byte newBuffer::getMedian2(){
    return bytebuffer2.getMedian();
}

byte newBuffer::getMax1(){
    return bytebuffer1.getMax();
}
byte newBuffer::getMax2(){
    return bytebuffer2.getMax();
}

byte newBuffer::getMin1(){
    return bytebuffer1.getMin();
}
byte newBuffer::getMin2(){
    return bytebuffer2.getMin();
}

int newBuffer::getSize1(){
    return bytebuffer1.getSize();
}
int newBuffer::getSize2(){
    return bytebuffer2.getSize();
}

byte newBuffer::peek1(unsigned int index){
    return bytebuffer1.peek(index);
}
byte newBuffer::peek2(unsigned int index){
    return bytebuffer2.peek(index);
    }

void newBuffer::setTreshold(byte in){
	treshold=in;
	bytebuffer1.setTreshold(in);
	bytebuffer2.setTreshold(in);
}

byte newBuffer::getTreshold1(){
	return bytebuffer1.getTreshold();
}
byte newBuffer::getTreshold2(){
	return bytebuffer2.getTreshold();
}

void newBuffer::setMaxDuration1(int in){
    bytebuffer1.setMaxDuration(in);
}
void newBuffer::setMaxDuration2(int in){
    bytebuffer2.setMaxDuration(in);
}
