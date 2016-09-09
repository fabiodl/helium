/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_REMOTECELLEXPORTER
#define HE_REMOTECELLEXPORTER

#include <helium/system/cell.h>
#include <helium/system/cellProtocol.h>
#include <sstream>

namespace helium{
  
 
  inline void printCellCapabilities(std::ostream& o,const Cell* c){
    o<<c<<" "<<(c!=NULL&&c->getProperties()!=NULL?c->getProperties()->getCapabilities():0);
  }


  class RemoteCellExporter:public Cell{
    Memory &m;
  public:

    std::string getName() const{
      return "remoteCellExporter";
    }
  RemoteCellExporter(Memory &pm):m(pm){
      std::vector<std::string> tokens;
      tokenize(tokens,helium::TREEDESCRIPTIONLOCATION,helium::PATHSEPARATORCHAR);
      IDPath path(tokens);
      m.assurePath(path)->setData(this);
    }

   

    void streamGet(std::ostream& o){
      m.machinePrint(o,printCellCapabilities);
    }

    void formatSignal(std::ostream& o,const DataP data){
      o<<(trust_cast<std::stringstream*>(data)->str());
    }
   
    
  };

}//helium


#endif
