#include "m3reader.h"


int main(int argc,char** argv){

  if (argc<2){
    std::cout<<"Usage "<<argv[0]<<" romname"<<std::endl;
    return 0;
  }
    
  M3Reader m3;
  
  std::ifstream in(argv[1],std::ios::binary|std::ios::in);

  
  if (!in){
    throw helium::exc::FileOpenRead(argv[1]);
  }

  std::cout<<"erasing.."<<std::endl;
  m3.erase();
  std::cout<<"ok"<<std::endl;
  

  
  helium::Array<unsigned char,M3Reader::BANKSIZE> bankData;

  std::cout<<"writing "<<argv[1]<<std::endl;
  int bankNum=0;
  while(!in.eof()){
    in.read((char*)bankData.data,M3Reader::BANKSIZE);
    if (in){
      std::cout<<"writing bank"<<bankNum<<std::endl;
      m3.write(bankNum,bankData.data);
      bankNum++;
    }    
  }
  std::cout<<"writing completed."<<std::endl;

}
