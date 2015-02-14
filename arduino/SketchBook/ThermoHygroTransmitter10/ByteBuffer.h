#include <stdlib.h>

#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#define byte unsigned char

class ByteBuffer
{
public:
	ByteBuffer();

	// This method initializes the datastore of the buffer to a certain sizem the buffer should NOT be used before this call is made
	void init(unsigned int buf_size);

	// This method resets the buffer into an original state (with no data)
	void clear();

	// This releases resources for this buffer, after this has been called the buffer should NOT be used
	void deAllocate();

	// Returns how much space is left in the buffer for more data
	int getSize();

	// Returns the maximum capacity of the buffer
	int getCapacity();

	// This method returns the byte that is located at index in the buffer but doesn't modify the buffer like the get methods (doesn't remove the retured byte from the buffer)
	byte peek(unsigned int index);

	//
	// Put methods, either a regular put in back or put in front
	//
	int putInFront(byte in);
	int put(byte in);

	int push(byte in);	//HGO
	byte getMedian();	//HGO
	byte getMax();	//HGO
	byte getMin();	//HGO
	byte getState();	//HGO
	void setTreshold(byte t);	//HGO
	byte getTreshold();	//HGO
    int getDirection();
    int getStateDuration();

	int putIntInFront(int in);
	int putInt(int in);

	int putLongInFront(long in);
	int putLong(long in);

	int putFloatInFront(float in);
	int putFloat(float in);

	//
	// Get methods, either a regular get from front or from back
	//
	byte get();
	byte getFromBack();

	int getInt();
	int getIntFromBack();

	long getLong();
	long getLongFromBack();

	float getFloat();
	float getFloatFromBack();

    void setMaxDuration(int);

protected:
	byte* data;

	unsigned int capacity;
	unsigned int position;
	unsigned int length;
	byte state;
	byte treshold;

    int stateDuration;  //start if state=1 for
    int maxDuration;    //max duration (ie 5)
    int direction;
};

#endif // BYTEBUFFER_H
