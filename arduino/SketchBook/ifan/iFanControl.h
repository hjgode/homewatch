#ifndef IFANCONTROL_H
#define IFANCONTROL_H
#include "iFanValues.h"

class iFanControl
{
    public:
        iFanControl();
        void addValue(byte b);
        void addVals(byte* bArr);
        virtual ~iFanControl();
        char* dumpValues(char*);
        char* dumpValues10(char*);
    protected:
    private:
		iFanValues fan1;
		iFanValues fan10;
		int iCount;
};

#endif // IFANCONTROL_H
