#include "ifanbuffer.h"

void iFanBuffer::initBuffer(byte* buf, int cnt){
  int i=0;
  while(i<cnt)
    buf[i]=0;
}

iFanBuffer::iFanBuffer(void){
  interval10=0;
//  buffer1 = (byte*)malloc(sizeof(byte)*buf_length);
  initBuffer(buffer1, buf_length);
//  buffer10 = (byte*)malloc(sizeof(byte)*buf_length);
  initBuffer(buffer10, buf_length);
  iCount=0;
  iCount10=0;
}

iFanBuffer::~iFanBuffer(){
//  free (buffer1);
//  free (buffer10);
}

void iFanBuffer::add10(byte b){
  if(iCount10<buf_length){
    buffer10[iCount10]=b;
    iCount++;
  }
  else{
    int i=0;
    for(i=1; i<iCount10-1; i++)
      buffer10[i-1]=buffer10[i];
    buffer10[iCount10-1]=b;
  }
}

void iFanBuffer::add(byte b){
  if(iCount<buf_length){
    buffer1[iCount]=b;
    iCount++;
  }
  else{
    int i=0;
    for(i=1; i<iCount-1; i++)
      buffer1[i-1]=buffer1[i];
    buffer1[iCount-1]=b;
  }
  interval10++;
  if(interval10 % buf_length == 0){
    add10(b);
    interval10=0;
  }
}

void iFanBuffer::dump1(){
  dumpBuffer(buffer1, buf_length);  
}

void iFanBuffer::dump10(){
  dumpBuffer(buffer10, buf_length);
}

void iFanBuffer::dumpBuffer(byte* b, byte cnt){
  Serial.println("");
  int i;
  for(i=0; i<cnt; i++)
    Serial.print("["); Serial.print(b[i]); Serial.print("]");
  Serial.println("");
}


