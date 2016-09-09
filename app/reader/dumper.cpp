#include "m3reader.h"

int main(int argc,char** argv){ 
  if (argc<3){
    std::cout<<"Usage "<<argv[0]<<" filename size(Kbyte)"<<std::endl;
    return 0;
  }
  
  std::ofstream f(argv[1],std::ios::binary|std::ios::out);
  int bankNum=helium::convert<int>(argv[2])/16;  
  M3Reader m3;
  helium::Array<unsigned char,M3Reader::BANKSIZE> bankData;  
  for (int i=0;i<bankNum;i++){
    std::cout<<"bank "<<i<< " of "<<bankNum<<std::endl;
    m3.read(f,i);
  }
  std::cout<<"read completed"<<std::endl;

  
}
