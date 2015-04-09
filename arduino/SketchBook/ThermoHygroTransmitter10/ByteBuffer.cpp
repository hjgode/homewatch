#include "stdlib.h"
#include "ByteBuffer.h"

ByteBuffer::ByteBuffer(){

}

void ByteBuffer::init(unsigned int buf_length){
	data = (byte*)malloc(sizeof(byte)*buf_length);
	capacity = buf_length;
	position = 0;
	length = 0;
	state=0;
	stateDuration=0;
	maxDuration=buf_length;
}

void ByteBuffer::deAllocate(){
	free(data);
}

void ByteBuffer::clear(){
	position = 0;
	length = 0;
}

int ByteBuffer::getSize(){
	return length;
}

int ByteBuffer::getCapacity(){
	return capacity;
}

byte ByteBuffer::peek(unsigned int index){
	byte b = data[(position+index)%capacity];
	return b;
}

int ByteBuffer::put(byte in){
	if(length < capacity){
		// save data byte at end of buffer
		data[(position+length) % capacity] = in;
		// increment the length
		length++;
		return 1;
	}
	// return failure
	return 0;
}

int ByteBuffer::push(byte in){
	if(length < capacity){
		// save data byte at end of buffer
		data[(position+length) % capacity] = in;
		// increment the length
		length++;
//		return 1;
	}
	else{
		//move all one down
		unsigned int i=0;
		for(i=0; i<length-1; i++)
			data[i]=data[i+1];
		data[length-1]=in;

	}
    //check if to switch FAN on and keep for nexxt 5 iteratrions
    if( ((getMax()-getMin())>treshold) && (getDirection()>0) ){
        //set state=1 only if stateDuration is less then maxDuration
        if(stateDuration<maxDuration){
            state=1;
        }
    }
    if(state==1 || stateDuration>0)
        stateDuration++;    //increment stateDuration
    if(stateDuration>maxDuration){
        if(state==1){
            state=0;
        }
        stateDuration=0;
    }
 	return 0;
}

int ByteBuffer::getStateDuration(){
    return stateDuration;
}

int ByteBuffer::getDirection(){
    int last=getSize();
    if(last<2)
        return 0;
    if(peek(last-1)>peek(last-2))
        return 1;
    else if (peek(last-1)==peek(last-2))
        return 0;
    else //if(bytebuffer1.peek(last)<bytebuffer1.peek(last-1))
        return -1;
}

byte ByteBuffer::getMedian(){
	unsigned int i=0;
	float f=0;
	for(i=0; i < length; i++)
		f+=data[i];
	f=f/length;
	return (byte) f;
}

byte ByteBuffer::getMax(){
	unsigned int i=0;
	byte f=0;
	for(i=0; i < length; i++)
		if(data[i]>f)
			f=data[i];
	return f;
}

byte ByteBuffer::getMin(){
	unsigned int i=0;
	byte f=255;
	for(i=0; i < length; i++)
		if(data[i]<f)
			f=data[i];
	return f;
}

void ByteBuffer::setTreshold(byte in){
	treshold=in;
}

byte ByteBuffer::getTreshold(){
	return treshold;
}

byte ByteBuffer::getState(){
//	if(getMax() - getMin() > treshold)
//		state=1;
//	else
//		state=0;
	return state;
}

int ByteBuffer::putInFront(byte in){
	if(length < capacity){
		// save data byte at end of buffer
		if( position == 0 )
			position = capacity-1;
		else
			position = (position-1)%capacity;
		data[position] = in;
		// increment the length
		length++;
		return 1;
	}
	// return failure
	return 0;
}

byte ByteBuffer::get(){
	byte b = 0;


	if(length > 0){
		b = data[position];
		// move index down and decrement length
		position = (position+1)%capacity;
		length--;
	}

	return b;
}

void ByteBuffer::setMaxDuration(int in){
    maxDuration=in;
}
byte ByteBuffer::getFromBack(){
	byte b = 0;
	if(length > 0){
		b = data[(position+length-1)%capacity];
		length--;
	}

	return b;
}

//
// Ints
//

int ByteBuffer::putIntInFront(int in){
    byte *pointer = (byte *)&in;
	putInFront(pointer[0]);
	putInFront(pointer[1]);
	return in;
}

int ByteBuffer::putInt(int in){
    byte *pointer = (byte *)&in;
	put(pointer[1]);
	put(pointer[0]);
	return in;
}


int ByteBuffer::getInt(){
	int ret;
    byte *pointer = (byte *)&ret;
	pointer[1] = get();
	pointer[0] = get();
	return ret;
}

int ByteBuffer::getIntFromBack(){
	int ret;
    byte *pointer = (byte *)&ret;
	pointer[0] = getFromBack();
	pointer[1] = getFromBack();
	return ret;
}

//
// Longs
//

int ByteBuffer::putLongInFront(long in){
    byte *pointer = (byte *)&in;
	putInFront(pointer[0]);
	putInFront(pointer[1]);
	putInFront(pointer[2]);
	putInFront(pointer[3]);
	return (int)in;
}

int ByteBuffer::putLong(long in){
    byte *pointer = (byte *)&in;
	put(pointer[3]);
	put(pointer[2]);
	put(pointer[1]);
	put(pointer[0]);
	return 0;
}


long ByteBuffer::getLong(){
	long ret;
    byte *pointer = (byte *)&ret;
	pointer[3] = get();
	pointer[2] = get();
	pointer[1] = get();
	pointer[0] = get();
	return ret;
}

long ByteBuffer::getLongFromBack(){
	long ret;
    byte *pointer = (byte *)&ret;
	pointer[0] = getFromBack();
	pointer[1] = getFromBack();
	pointer[2] = getFromBack();
	pointer[3] = getFromBack();
	return ret;
}


//
// Floats
//

int ByteBuffer::putFloatInFront(float in){
    byte *pointer = (byte *)&in;
	putInFront(pointer[0]);
	putInFront(pointer[1]);
	putInFront(pointer[2]);
	putInFront(pointer[3]);
	return 0;
}

int ByteBuffer::putFloat(float in){
    byte *pointer = (byte *)&in;
	put(pointer[3]);
	put(pointer[2]);
	put(pointer[1]);
	put(pointer[0]);
	return 0;
}

float ByteBuffer::getFloat(){
	float ret;
    byte *pointer = (byte *)&ret;
	pointer[3] = get();
	pointer[2] = get();
	pointer[1] = get();
	pointer[0] = get();
	return ret;
}

float ByteBuffer::getFloatFromBack(){
	float ret;
    byte *pointer = (byte *)&ret;
	pointer[0] = getFromBack();
	pointer[1] = getFromBack();
	pointer[2] = getFromBack();
	pointer[3] = getFromBack();
	return ret;
}

