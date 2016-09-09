
#include "m3reader.h"
#include <helium/vector/operations.h>
#include <helium/os/time.h>
enum Cmd{
  HELP,ERASE,APPEND,WRITE,DUMP,READRAM,WRITERAM,CMDS
};

static const int VARIANTSIZE=4;
typedef const char * Variants[VARIANTSIZE];



Variants commands[CMDS]={
  
  {"h","help","printhelp","HELP"},
    {"e","erase","eraserom","eraseonly"},
    {"a","append","noerase","writeony"},
    {"w","write","writerom","writeanderase"},
    {"d","dump","r","read"},
    {"m","readram","dumpram","ramread"},
    {"M","writeram","ramwrite","uploadram"}
};
  
Cmd getCmd(const char* s){
  for (int i=0;i<CMDS;i++){
    for (int j=0;j<4;j++){
      if (strcmp(s,commands[i][j])==0){
	return (Cmd)i;
      }
    }
  }
  throw helium::exc::InvalidOperation(std::string("Unknown command:")+s);
}


class M3Operations{

  M3Reader m3;

  const std::string progName;
  
public:  
  M3Operations(const std::string& name):progName(name){
  }


 
  void printBasicHelp(){
    std::cout<<"Usage "<<progName<<" cmd [options] params"<<std::endl<<std::endl;
    std::cout<<"Available cmds:"<<std::endl;
    for (int i=0;i<CMDS;i++){
      for (int j=0;j<VARIANTSIZE;j++){
	std::cout<<commands[i][j]<<"/";
      }
      std::cout<<std::endl;
    }

  }
  

  void printCommandHelp(Cmd cmd){
    switch(cmd){
    case HELP:
      std::cout<<commands[cmd][1];
      for (int i=0;i<CMDS;i++){
	std::cout<<" ["<<commands[i][1]<<"]";
      }
      std::cout<<std::endl;
      break;    
    case ERASE:
      std::cout<<commands[cmd][1]<<": erases the flash"<<std::endl;
      std::cout<<commands[cmd][1]<<" [sector A15..A8] "<<std::endl;
      break;
    case APPEND:
    case WRITE:
      std::cout<<commands[cmd][1]<<" [nomapper] filename" <<std::endl;
      break;
    case DUMP:
      std::cout<<commands[cmd][1]<<" [nomapper] filename filesize in kilobytes" <<std::endl;
      break;
    case WRITERAM:
    case READRAM:
      std::cout<<commands[cmd][1]<<" filename"<<std::endl;
      break;
    case CMDS:
      break;
    }
  }

  void erase(){
    std::cout<<"erasing..."<<std::flush;
    m3.erase();
    std::cout<<"ok"<<std::endl;
  }

  void eraseSector(unsigned char s){
    std::cout<<"erasing sector "<<(int)s<<"..."<<std::flush;
    m3.eraseSector(s);
    std::cout<<"ok"<<std::endl;
  }

  
  void append(const std::string& filename,bool withmapper,int skip=0){
    helium::Time t=helium::getSystemMillis();
    std::ifstream in(filename.c_str(),std::ios::binary|std::ios::in);
    if (!in){
      throw helium::exc::FileOpenRead(filename);
    }
    helium::Array<unsigned char,M3Reader::BANKSIZE> bankData;
    std::cout<<"writing "<<filename<<" assuming "<<(withmapper?"SEGA":"no")<<" mapper"<<std::endl;
    int bankNum=0;
    while(!in.eof()){
      helium::setAll(bankData,0xFF);
      in.read((char*)bankData.data,M3Reader::BANKSIZE);
      if (in.gcount()&&(bankNum>=skip)){
	std::cout<<"writing bank"<<bankNum<<std::endl;
	m3.write(bankNum,bankData.data,withmapper?M3Reader::SEGA_ROM:M3Reader::NOMAPPER);
      }    
      bankNum++;
    }
    std::cout<<"writing completed in "<<(helium::getElapsedMillis(t)/1000)<<"s"<<std::endl;
  }


  void dump(const std::string& filename,bool withmapper,int bankNum){
    std::ofstream f(filename.c_str(),std::ios::binary|std::ios::out);
    std::cout<<"Dumping to "<<filename<<" assuming "<<(withmapper?"SEGA":"no")<<" mapper"<<std::endl;
    for (int i=0;i<bankNum;i++){
      std::cout<<"bank "<<i<< " of "<<bankNum<<std::endl;
      m3.read(f,i,withmapper?M3Reader::SEGA_ROM:M3Reader::NOMAPPER);
    }
    std::cout<<"read completed"<<std::endl;
  }


  void readRam(const std::string& filename){
    std::ofstream f(filename.c_str(),std::ios::binary|std::ios::out);
    std::cout<<"Dumping SRAM to "<<filename<<std::endl;
    for (int i=0;i<2;i++){
      std::cout<<"bank "<<i<< " of 2"<<std::endl;
      m3.read(f,i,M3Reader::SEGA_RAM);
    }
    std::cout<<"read completed"<<std::endl;
  }

  void writeRam(const std::string& filename){
    std::ifstream in(filename.c_str(),std::ios::binary|std::ios::in);
    if (!in){
      throw helium::exc::FileOpenRead(filename);
    }
    helium::Array<unsigned char,M3Reader::BANKSIZE> bankData;
    helium::setAll(bankData,0xFF);
    std::cout<<"Writing "<<filename<<" to the SRAM"<<std::endl;
    int bankNum=0;
    while(!in.eof()){
      in.read((char*)bankData.data,M3Reader::BANKSIZE);
      if (in){
	std::cout<<"writing bank"<<bankNum<<std::endl;
	m3.write(bankNum,bankData.data,M3Reader::SEGA_RAM);
	bankNum++;
      }    
    }
    std::cout<<"writing completed."<<std::endl;
  
  }

};//M3Operations

int getFileAndMapper( std::string& filename, bool& withmapper,int argc,char** argv,const std::string& cmd){
  withmapper=true;
  if (argc<3){
    throw helium::exc::InvalidOperation(cmd+": No ROM file specified");      
  }
  int skipPosition=-1;
  if (strcmp(argv[2],"nomapper")==0){
    withmapper=false;
    if (argc<4){
      throw helium::exc::InvalidOperation(cmd+": No ROM file specified");      
    }
    filename=argv[3];	
    skipPosition=4;
  }else{
    filename=argv[2];
    skipPosition=3;
  }
  if (argc>skipPosition){
    return helium::convert<int>(argv[skipPosition]);
  }else{
    return 0;
  }
}


int main(int argc,char** argv){

  M3Operations op(argv[0]);
    
  if (argc<2){
    op.printBasicHelp();
    return 0;
  }
  
  Cmd cmd=getCmd(argv[1]);
  switch(cmd){    
  case HELP:
    if (argc<3){
      op.printBasicHelp();
    }else{
      op.printCommandHelp(getCmd(argv[2]));
    }
    break;    
  case ERASE:
    if (argc<3){
      op.erase();
    }else{
      op.eraseSector(static_cast<unsigned char>(helium::convert<int>(argv[2])));
    }
    break;
  case WRITE:
  case APPEND:
    {
      std::string filename;
      bool withmapper;
      int toskip=getFileAndMapper(filename,withmapper,argc,argv,cmd==WRITE?"writing":"appending");
      if (cmd==WRITE){
	op.erase();
      }
      if (toskip){
	std::cout<<"skipping "<<toskip<<"banks"<<std::endl;
      }
      op.append(filename,withmapper,toskip);
    }
    break;
  case DUMP:{
    std::string filename;
    bool withmapper;
    int bankNum=(std::max)(1,helium::convert<int>(argv[argc-1])/16);  
    getFileAndMapper(filename,withmapper,argc-1,argv,"dumping");//we assume the last is the size

    op.dump(filename,withmapper,bankNum);
  }
    break;
  case READRAM:
    if (argc<3){
      throw helium::exc::InvalidOperation(cmd+": No destination file for ram specified"); 
    }
    op.readRam(argv[2]);
    break;
  case WRITERAM:
    if (argc<3){
      throw helium::exc::InvalidOperation(cmd+": No destination file for ram specified"); 
    }
    op.writeRam(argv[2]);
    break;
  case CMDS:
    break;
  }



}//int main


  

   

