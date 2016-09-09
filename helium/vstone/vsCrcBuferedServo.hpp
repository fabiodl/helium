
void send(int id,int addr,const unsigned char* data,int size){
  writes.waitReady(id);   
  unsigned char msg[3+32+1];//header+maxsize+crc
  msg[0]=PACKETSTART|id;
  msg[1]=WRF|size;
  msg[2]=addr;
  memcpy(msg+3,data,size);
  msg[3+size]=0x7f&getCrc(msg,3+size);
  s.send(msg,3+size+1);
  
  unsigned char remCrc;
  Time t=getSystemMillis();
  s.recvAll(&remCrc,1,ADDITIONALTIME);
  std::cout<<"waited reply"<<getElapsedMillis(t)<<std::endl;
  if (remCrc!=msg[3+size]){
    throw exc::CRCError<false,true>(id,addr,size,msg[3+size],remCrc);
  }
}
