/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.2
 * Release_flags s
 */

#include <helium/robot/motorSpooler.h>

namespace helium{
  


  namespace motor{

    


  }


  namespace spt{  
    const char* NoMotId::NOMOTMODEID="nomode";
    
  };

   template<>
  void deserializeGet<motor::ExactPositionCellSignature>(motor::ExactPositionCellSignature::GT& u,std::istream& i){
     /*std::string s;
     i>>s;
     std::cout<<"DESERIALIZEGET"<<s<<std::endl;*/
     rawHexRead(u,i);
     //std::cout<<"DESERIALIZEGET "<<u<<std::endl;
  }
  

  template<>
  void deserializeUpdate<motor::ExactPositionCellSignature>(motor::ExactPositionCellSignature::UT& u,std::istream& i){
    /*std::string s;
    i>>s;
    std::cout<<"DESERIALIZEUPDATE "<<s<<std::endl;*/
    rawHexRead(u,i);
    //std::cout<<"DESERIALIZEUPDATE "<<u<<std::endl;
  }
  

   




  //const int MotorMode::NOMODE=-1;

}
