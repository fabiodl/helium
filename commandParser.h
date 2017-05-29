#ifndef _COMMANDPARSER_H_
#define _COMMANDPARSER_H_
#include <command.h>
#include <vector>
#include <map>

class RawCommand{
public:
  std::vector<uint8_t> packet;
  RawCommand(){}
  RawCommand(uint8_t firstByte){
    packet.push_back(firstByte);
  }
};

class CommandParser{
public:
  virtual RawCommand parse(int argc,char** argv)=0;  
};

class NoParamCommand:public CommandParser{

public:
  NoParamCommand(Command _cmd);
  RawCommand parse(int argc,char** argv);

private:
  RawCommand cmd;
};

class LocalCommand:public CommandParser{
public:
  enum Id{
    ID=0xFF
  };
  LocalCommand(uint8_t localcmd);
  RawCommand parse(int argc,char** argv);

private:
  uint8_t localcmd;
};



class DictionaryCommand:public CommandParser{
public:
  std::map<std::string,CommandParser*> commands;
  RawCommand parse(int argc,char** argv);

};

class WriteCfgBytesCommand:public CommandParser{
public:
  RawCommand parse(int argc,char** argv);
};

class WriteCfgBitsCommand:public CommandParser{
public:
  RawCommand parse(int argc,char** argv);
};



class SetCfgKeysCommand:public CommandParser{
  RawCommand parse(int argc,char** argv);
};

class SetCfgAutofireCommand:public CommandParser{
  RawCommand parse(int argc,char** argv);
};

class SetCfgIoCommand:public CommandParser{
  RawCommand parse(int argc,char** argv);
};

class SetIoPortCommand:public CommandParser{
  RawCommand parse(int argc,char** argv);
};

class SetIoKeyvalCommand:public CommandParser{
  RawCommand parse(int argc,char** argv);
};


#endif
