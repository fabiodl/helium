/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_MOTORSTRUCTURES
#define HE_MOTORSTRUCTURES
#include <iostream>
#include <helium/robot/motorInterface.h>
#include <helium/vector/array.h>
#include <helium/system/cellDescription.h>


namespace helium{


  namespace motor{



    template<typename MotorSign,bool hasZero=MotorSign::NEEDSZERO>
    class MotorCalib{
    public:
      
      typename MotorSign::Pos zero;
      typename MotorSign::Pos minv;
      typename MotorSign::Pos maxv;      

      MotorCalib();                
    };


    template<typename MotorSign>
    class MotorCalib<MotorSign,false>{
    public:
      
      typename MotorSign::Pos minv;
      typename MotorSign::Pos maxv;
      
      MotorCalib();
     
      
    };


    template<typename MotorSign> std::ostream& operator<<(std::ostream& o,const MotorCalib<MotorSign,true>& m){
      return o<<toHexString(m.minv)<<" "<<toHexString(m.maxv)<<" "<<toHexString(m.zero);
    }

    template<typename MotorSign> std::istream& operator>>(std::istream& i,MotorCalib<MotorSign,true>& m){
      return i>>m.minv>>m.maxv>>m.zero;
    }

    
    template<typename MotorSign> std::ostream& operator<<(std::ostream& o,const MotorCalib<MotorSign,false>& m){
      return o<<toHexString(m.minv)<<" "<<toHexString(m.maxv);
    }
    
    template<typename MotorSign> std::istream& operator>>(std::istream& i,MotorCalib<MotorSign,false>& m){
      return i>>m.minv>>m.maxv;
    }



    template<typename MotorSign>
    class Calib{      
    public:     
      Array<MotorCalib<MotorSign> > data;      
    };    
    
    
    

  }





  


  template<typename MotorSign>
  class LinearPosNormalizer:public motor::Raw<MotorSign>::PosNormalizer{
    
    motor::Calib<MotorSign>& calib;
  public:    
    LinearPosNormalizer( motor::Calib<MotorSign>& pcalib):calib(pcalib){
    }
    
    motor::Interface::Position normPos(typename motor::Raw<MotorSign>::Position value,int joint);


    typename motor::Raw<MotorSign>::Position unnormPos(motor::Interface::Position value,int joint);

  };


  template<typename MotorSign>
  class PositionOnlyPropNormalizer:public motor::Raw<MotorSign>::PropNormalizer{
    typename motor::Raw<MotorSign>::PosNormalizer& posNorm;
    


  public:    
    PositionOnlyPropNormalizer(typename motor::Raw<MotorSign>::PosNormalizer& _posNorm ):posNorm(_posNorm){
    }
    
    motor::Interface::Property normProp(typename motor::Raw<MotorSign>::Property value,const motor::PropertyRequest& r);
    typename motor::Raw<MotorSign>::Property unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r);

  };



  template<typename MotorSign>
  class PosLimiter{
    motor::Calib<MotorSign>& calib;
    
  public:    
    PosLimiter(motor::Calib<MotorSign>& pcalib);

    typename motor::Raw<MotorSign>::Position getMin(int joint);
    typename motor::Raw<MotorSign>::Position getMax(int joint);

    typename motor::Raw<MotorSign>::Position limit(typename motor::Raw<MotorSign>::Position val,int joint);
  };
  


  class IdMapper{
  public:
    Array<int> rawId;   
  };


  class IdentityIdMapper:public IdMapper{
  public:
    IdentityIdMapper(int n){
      rawId.resize(n);
      for (int i=0;i<n;i++) rawId[i]=i;
    }
  };

  template<typename MotorSign> 
  class DefDescr<motor::MotorCalib<MotorSign,true> >:public Description<motor::MotorCalib<MotorSign,true> >{
  public:
    DefDescr(motor::MotorCalib<MotorSign,true>& c):Description<motor::MotorCalib<MotorSign,true> >(c){
    }

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };

  template<typename MotorSign> 
  class DefDescr<motor::MotorCalib<MotorSign,false> >:public Description<motor::MotorCalib<MotorSign,false> >{
  public:
    DefDescr(motor::MotorCalib<MotorSign,false>& c):Description<motor::MotorCalib<MotorSign,false> >(c){
    }

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };


  template<typename MotorSign> 
  class DefDescr<motor::Calib<MotorSign> >:public Description<motor::Calib<MotorSign> >{
  public:
    DefDescr(motor::Calib<MotorSign>& c):Description<motor::Calib<MotorSign> >(c){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };


 
  template<>
   class DefDescr<IdMapper>:public Description<IdMapper>{
  public:
    DefDescr(IdMapper& im):Description<IdMapper>(im){      
    }
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(vectMapping(vd(obj.rawId),IDPath("motors.motor","."),"rawId",IDPath("motors.num",".") ));
 

    }

  };

  template<typename MotorSign>
  class MotorGetterToken:public Deletable{      
      typedef typename motor::Raw<MotorSign>::PropertyNotify PropertyNotify;
    public:
      Array<PropertyNotify> props;
      Array<Callback<PropertyNotify*>*> to;
      
      void notify(int i);
      
      MotorGetterToken(const typename motor::Raw<MotorSign>::PropertyList &info,typename motor::Raw<MotorSign>::PropNormalizer& pn);

      static void splitByJoint(Array <typename  motor::Raw<MotorSign>::Token>& tk,const typename motor::Raw<MotorSign>::PropertyList &info,typename motor::Raw<MotorSign>::PropNormalizer& pn);

    
  };//MotorGetterToken    




}//ns helium

#include <helium/robot/motorStructures.hpp>


#endif
