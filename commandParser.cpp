#include <stdexcept>
#include "commandParser.h"
#include "keyMapping.h"
#include <config.h>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <string.h>




template<typename T> RawCommand makeCommand(Command cmdId,T& data){
  RawCommand cmd(cmdId);
  cmd.packet.resize(1+sizeof(T));
  memcpy(&cmd.packet[0]+1,&data,sizeof(T));
  return cmd;
}



NoParamCommand::NoParamCommand(Command _cmd):cmd(_cmd){
}
RawCommand  NoParamCommand::parse(int argc,char** argv){
  if (argc!=0){
    throw std::invalid_argument("no parameters expected");
  }
  return cmd;
}


LocalCommand::LocalCommand(uint8_t _localcmd):localcmd(_localcmd){  
}



RawCommand LocalCommand::parse(int argc,char** argv){
  if (argc!=0){
    throw std::invalid_argument("no parameters expected");
  }
  RawCommand cmd;
  cmd.packet.push_back(ID);
  cmd.packet.push_back(localcmd);
  return cmd;
}

RawCommand  DictionaryCommand::parse(int argc,char** argv){
  typedef std::map<std::string,CommandParser*> Map;
  Map::iterator it; 
  if (argc<1||(it=commands.find(argv[0]))==commands.end()){
    std::stringstream ss;
    if (argc>=1) ss<<argv[0]<<": no such option"<<std::endl;
    ss<<"Expected one of the following options: "<<std::endl;
    for (Map::iterator it=commands.begin();it!=commands.end();++it){
      ss<<it->first<<" ";
    }
    throw std::invalid_argument(ss.str().c_str());
  }
  return it->second->parse(argc-1,argv+1);
}


inline uint8_t readHex(const char* s){
  std::stringstream ss;
  ss<<s;
  int v;
  ss>>std::hex>>v;
  return static_cast<uint8_t>(v);
}

RawCommand WriteCfgBytesCommand::parse(int argc,char** argv){
  RawCommand cmd(CMD_WRITECFG_BYTES);
   if (argc<2){
     throw std::invalid_argument("Expecting <offset(dec)> <value0(hex)>... <valueN(hex)>");
   }
   cmd.packet.push_back(atoi(argv[0]));
  for (int i=1;i<argc;i++){    
    cmd.packet.push_back(readHex(argv[i]));
  }
  return cmd;
}

RawCommand WriteCfgBitsCommand::parse(int argc,char** argv){
  if (argc!=3){
    throw std::invalid_argument("Expecting <offset(dec)> <mask(hex)> <value(hex)>.");
  }
  WriteCfgBits wcb;
  wcb.address=atoi(argv[0]);
  wcb.mask=readHex(argv[1]);
  wcb.value=readHex(argv[2]);
  return makeCommand(CMD_WRITECFG_BITS,wcb);
}




RawCommand SetCfgKeysCommand::parse(int argc,char** argv){
  RawCommand cmd(CMD_SETCFG_KEYS);
  if (argc!=3){
    throw std::invalid_argument("Expecting three filenames: pad1 config, pad2 config, io keys config");
  } 
  cmd.packet.resize(1+NUMBER_OF_KEYS);
  uint8_t* keyConfig=&cmd.packet[0]+1;
  getKeySequence(keyConfig,KEY_PAD1,argv[0]);
  getKeySequence(keyConfig,KEY_PAD2,argv[1]);
  getKeySequence(keyConfig,KEY_EXTRA,argv[2]);  
  return cmd;
}



template<uint8_t IoConfig::*f>
bool fieldEdit(IoConfig& t,const std::string& enabler,const std::string& disabler,const std::string& s,int target){
  if (s==enabler){
    t|=(1<<target);
    return true;
  }
  if (s==disabler){
    t&=~(1<<target);
    return true;
  }
  return false;
}

#define checkField(F,enabler,disabler) if(*t==enabler){F|=1<<target;}else if(*t==disabler){F&=~(1<<target);}

std::vector<std::string> getTokensFromFile(const char* fname){
  std::vector<std::string> tokens;
  std::ifstream f(fname);
  if (!f){
    throw std::runtime_error(std::string("Error opening file ")+fname);
  }
  while (!f.eof()){
    std::string s;
    f>>s;
    if (s.length()>0){
      tokens.push_back(s);
    }
  }
  return tokens;
}

RawCommand SetCfgAutofireCommand::parse(int argc,char** argv){
  if (argc<1) throw std::invalid_argument("Expecting filename\n"
                                          "The file should contain a sequence with the following keywords:\n"
                                          "io io<n> pad1 pad2 up down left right a b c start x y z mode"
                                          "period <value> pushtime <value> \n"
                                          "Rapid fire unit: period 837 pushtime 251 \n"
                                          "Japanese sms: period 260 pushtime 130 \n"


                                          );
  std::vector<std::string> tokens=getTokensFromFile(argv[0]);
  AutofireConfig autofire;
  memset(autofire.firemask,0xFF,sizeof(autofire.firemask));
  KeyType keyType=KEY_PAD1;
  for (std::vector<std::string>::iterator t=tokens.begin();
       t!=tokens.end();
       t++){
    bool known=false;
    if (*t=="pad1"){
      keyType=KEY_PAD1;
      known=true;
    }else if (*t=="pad2"){
      keyType=KEY_PAD1;
      known=true;
    }else if ((*t).substr(0,2)=="io"){
      keyType=KEY_EXTRA;
      if ((*t).length()==2){
        known=true;
      }
    }else if(*t=="period"){
      t++;
      std::stringstream ss;
      ss<<*t;
      ss>>autofire.period;
      known=true;
    }else if(*t=="pushtime"){
      t++;
      std::stringstream ss;
      ss<<*t;
      ss>>autofire.pushTime;
      known=true;
    }
    
    if (!known){
      try{
        int off=getOffset(keyType,t->c_str());
        autofire.firemask[off/8]&=~(1<<(off%8));
      }catch(...){
        throw std::invalid_argument("expected a sequence with the following keywords:\n"
                                  "io io<n> pad1 pad2 up down left right a b c start x y z mode "
                                   "period <value> pushtime <value> \n"
                                    "Rapid fire unit: period 837 pushtime 251 \n"
                                    "Japanese sms: period 260 pushtime 130 \n"
                                  );
      }
    }
    
  }//for tokens
  return makeCommand(CMD_SETCFG_AUTOFIRE,autofire);
  
}




RawCommand SetCfgIoCommand::parse(int argc,char** argv){
 
  if (argc<1) throw std::invalid_argument("Expecting filename\n"
                                          "The file should contain a sequence with the following keywords:\n"
                                          "io<n> key_off/key_on input/output low/high latch_off/latch_on"
                                          );
  std::vector<std::string> tokens=getTokensFromFile(argv[0]);
  IoConfig ioConfig;

  ioConfig.enableKeyReport=0x03;
  ioConfig.ddrVal=0x00;
  ioConfig.portVal=0x0F;
  ioConfig.enableLatch=0;


  
  int target=0;
  for (std::vector<std::string>::iterator t=tokens.begin();
       t!=tokens.end();
       t++
       ){

    if (t->substr(0,2)=="io"){
      target=atoi(t->substr(2).c_str());
    }else checkField(ioConfig.enableKeyReport,"key_on","key_off")
      else checkField(ioConfig.ddrVal,"output","input")
        else checkField(ioConfig.portVal,"high","low")
          else checkField(ioConfig.enableLatch,"latch_on","latch_off")
            else{
      throw std::invalid_argument("expected a sequence with the following keywords:\n"
                                  "io<n> key_off/key_on input/output low/high latch_off/latch_on"
                                  );
      
    }
    
  }
  return makeCommand(CMD_SETCFG_IO,ioConfig); 
}


template<typename T>
RawCommand makeMaskValueCommand(Command cmdId,int argc,char** argv){
  if (argc!=2){
     throw std::invalid_argument("expected <mask(hex)> <value(hex)>");
  }
  T t;
  t.mask=readHex(argv[0]);
  t.value=readHex(argv[1]);
  return makeCommand(cmdId,t);
}

RawCommand SetIoPortCommand::parse(int argc,char** argv){
  return makeMaskValueCommand<PortVal>(CMD_SETIO_PORT,argc,argv);
}


RawCommand SetIoKeyvalCommand::parse(int argc,char** argv){
  return makeMaskValueCommand<KeyVal>(CMD_SETIO_KEYVAL,argc,argv);
}
