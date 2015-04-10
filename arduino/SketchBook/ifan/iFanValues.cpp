
#include "iFanValues.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define byte uint8_t
#define sbyte int8_t

		byte iFanValues::minVal(){
			byte bMin=100;
			for(byte b=0; b <= iTop; b++){
				if(values[b] < bMin)
					bMin=values[b];
			}
			return bMin;
		}
		byte iFanValues::maxVal(){
			byte bMax=0;
			for(byte b=0; b <= iTop; b++){
				if(values[b] > bMax)
					bMax=values[b];
			}
			return bMax;
		}

		byte iFanValues::getAccel(){
			byte bAcc=0;
			bAcc=(byte)( maxVal()-minVal() );
			return bAcc;
		}
		byte iFanValues::getAverage(byte bStart, byte bEnd){
			if(iTop==-1)
				return 0;
			if(bStart==bEnd)
				return values[bStart];
			uint16_t bAvg=0;	//need more than a byte to store sum
			for(byte b=bStart; b <= bEnd; b++){
				bAvg += values[b];
			}
			bAvg=(uint16_t)(bAvg/(iTop+1));
			return (byte)bAvg;
		}
		byte iFanValues::getAverage(){
			if(iTop == -1)
				return 0;
			return getAverage(0, (byte)iTop);
		}

		float iFanValues::getVariance(){
		    //Mittelwert m
		    float m=0;
			int N=iTop+1; //number of vals
			if(N<2)
				return 0;
			byte* x = values;
		    for (int i = 0; i < N; i++)
		    {
		        m += x[i];
		    }
		    m /= N;

		    //Varianz var
		    float variance = 0;
		    for (int i = 0; i < N; i++)
		    {
		        variance += (x[i] - m) * (x[i] - m);
		    }
		    variance /= ( N-1 );

		    //Standardabweichung sigma
		    // double sigma = System.Math.Sqrt( variance );

			return variance;
		}

		float iFanValues::getSigma(){
		    //Mittelwert m
		    float m=0;
			int N=iTop+1; //number of vals
			if(N<2)
				return 0;
			byte* x = values;
		    for (int i = 0; i < N; i++)
		    {
		        m += x[i];
		    }
		    m /= N;

		    //Varianz var
		    float variance = 0;
		    for (int i = 0; i < N; i++)
		    {
		        variance += (x[i] - m) * (x[i] - m);
		    }
		    variance /= ( N-1 );

		    //Standardabweichung sigma
		    float sigma = sqrt( variance );

			return sigma;
		}

		sbyte iFanValues::getTendence(){
			if(iTop<1)
				return 0;
			byte bPrev=values[iTop-1];//  getAverage(0, iTop-1);
			byte bLast=values[iTop];// getAverage((byte)(iTop/2), (byte)iTop);

			if(bLast>bPrev)
				return +1;
			else if(bLast==bPrev)
				return 0;
			else // if(bLast<bPrev)
				return -1;
		}


iFanValues::iFanValues()
{
    //ctor
    maxLen=10;
    iTop=-1;
    values = (byte*)malloc(maxLen);// new uint8_t[maxLen];
}
void iFanValues::addValue(uint8_t b){
    if(iTop==maxLen-1){
        //move all vals one down
        for(int i=0; i < iTop; i++){
            values[i]=values[i+1];
        }
        //set top value
        values[iTop]=b;
        iTop=(uint8_t)( maxLen-1 );
    }
    else{
        iTop++;
        values[iTop]=b;
    }
}

char* iFanValues::dumpValues(char* str){
    char* temp = (char*)malloc(255);// new char[255];
    memset(temp,0,255);
    for(byte i=0; i<maxLen; i++){
//        sOut += (values[i].ToString("000")+ " ");
        sprintf(temp, "%03i ", values[i]);
        strcat(str, temp);
    }
    sprintf(temp,
    "\n\t min=%03i max=%03i avg=%03i acc=%03i ten=%03i sig=%f var=%f",
    minVal(), maxVal(),
    getAverage(), getAccel(), getTendence(),
    (double)getSigma(), (double)getVariance());
    strcat(str, temp);
/*
    sOut += "\n\t min="+min().ToString("000")+
                  " max="+max().ToString("000")+
                  " avg="+getAverage().ToString("000")+
                  " acc="+getAccel().ToString("000") +
                  " ten="+getTendence().ToString() +
                  " sig="+getSigma().ToString("0.00") +
                  " var="+getVariance().ToString("0.00");
    return sOut;
*/
    free (temp);
    return str;
}

iFanValues::~iFanValues()
{
    //dtor
    //delete (values);
    free (values);
}
