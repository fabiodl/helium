/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef APP_SLAYER_
#define APP_SLAYER_

#include <helium/robot/application.h>
#include <helium/core/exception.h>

#include "hwComponent.h"



class Slayer:public helium::Application{
  helium::HwCmpList components;
  typedef std::map<helium::HwFactoryName,helium::HwFactoryInterface* > FactoryMap;
  FactoryMap factories;
public:

  template<typename Device> void addComponentType(const helium::HwFactoryName& type);
  enum State{UNKNOWN,SKIP,OK,FAILED};
  ~Slayer();
private:
  void printSummary();
  void run();
  FactoryMap::iterator getFactory(const helium::HwFactoryName& n);    
  std::vector<State> hwState;
};

namespace helium{
template<>  struct EnumStrings<Slayer::State>{
    static const int NUM=4;
    static const char* strings[4];       
  };

}

#include "slayer.hpp"

#endif
