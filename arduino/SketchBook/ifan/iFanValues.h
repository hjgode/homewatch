#ifndef IFANVALUES_H
#define IFANVALUES_H
#include <inttypes.h>
#include <stdlib.h>

#define byte uint8_t
#define sbyte int8_t

class iFanValues{
    public:
        iFanValues();
        virtual ~iFanValues();
        char* dumpValues(char*);
        void addValue(uint8_t);
        byte getAverage();

    protected:
    private:
        uint8_t maxLen;
        uint8_t* values;
        int8_t iTop;
        byte minVal();
        byte maxVal();
        byte getAverage(byte bStart, byte bEnd);
        float getSigma();
        byte getAccel();
        sbyte getTendence();
        float getVariance();
};
#endif
