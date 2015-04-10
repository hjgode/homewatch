#include "iFanControl.h"
#include "iFanValues.h"
//for statistics lib see also http://playground.arduino.cc//Main/Statistics

		void iFanControl::addValue(byte b){
			fan1.addValue(b);
			iCount++;
			if(iCount==10){
				fan10.addValue(fan1.getAverage());
				iCount=0;
			}
		}
		char* iFanControl::dumpValues(char* str){
			return fan1.dumpValues(str);
		}
		char* iFanControl::dumpValues10(char* str){
			return fan10.dumpValues(str);
		}
        void iFanControl::addVals(byte* bArr){
            const byte *pb=&bArr[0];
            do{
                addValue(*pb);
                pb++;
            }while (*pb!=0);
		}

iFanControl::iFanControl()
{
    //ctor
    iCount=0;
}

iFanControl::~iFanControl()
{
    //dtor
}
