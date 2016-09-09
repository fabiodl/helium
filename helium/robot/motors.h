#ifndef HE_MOTORS
#define HE_MOTORS


#include <helium/core/deletable.h>
#include <helium/vector/array.h>
#include <helium/smartpointer/reference.h>
#include <helium/core/triple.h>
#include <helium/signal/callback.h>
#include <helium/util/ioUtil.h>

namespace oldhelium{

  /*namespace exc{ moved to hw
    class MotorException{
      int type;
      int motId;
      MotorException(int ptype,int pmotId=-1):type(ptype),motId(pmotId){
      }
      
    };
    }*/




  class Motors{   

    
    
  public:
    enum PropertyID{POSITION,POWER,TEMP,PID_I,PID_D,PID_E,VSSV410,PROPERTIES};
    
   
    

    typedef  int Property;
    typedef  double NormProperty;

    typedef int RawPosition;
    typedef double NormPosition;


    static const RawPosition INVALIDRAWPOS;
    static const NormPosition INVALIDNORMPOS;
    static const RawPosition MOVINGRAWPOS;
    static const NormPosition MOVINGNORMPOS;
    static const RawPosition FREERAWPOS;
    static const NormPosition FREENORMPOS;










    struct PropertyRequest{
      int joint;
      PropertyID id;
      int idx;

      PropertyRequest(){}

      PropertyRequest(int pjoint,PropertyID pid,int pidx=-1):joint(pjoint),id(pid),idx(pidx){
      }   
      void set(int pjoint,PropertyID pid,int pidx=-1){
	joint=pjoint;
	id=pid;
	idx=pidx;
      }   
      bool operator==(const PropertyRequest& b){
	return joint==b.joint&&id==b.id&&idx==b.idx;
      }
      bool operator!=(const PropertyRequest& b){
	return joint!=b.joint||id!=b.id||idx!=b.idx;
      }


      bool operator<(const PropertyRequest& b) const{
	if (joint<b.joint) return true;
	if (joint==b.joint&&id<b.id) return true;
	if (joint==b.joint&&id==b.id&&idx<b.idx) return true;
	return false;
      }
    };


    
    typedef triple<Property,const PropertyRequest*,Motors*> PropertyNotify;
    typedef std::pair<Callback<PropertyNotify>*,const PropertyRequest*> PropertyNotifyRequest;
    typedef std::list<PropertyNotifyRequest> PropertyList;
    typedef SingleReference<Deletable> Token; 

    
    
    virtual NormProperty normalize(Property value,const PropertyRequest& r)=0;
    virtual Property unnormalize(NormProperty value,const PropertyRequest& r)=0;

    
    virtual NormPosition normalize(RawPosition p,int j)=0;
    virtual RawPosition unnormalize(NormPosition p,int j)=0;

    
    

    virtual void rotate(int j,RawPosition val)=0;
    virtual void rotate(int j,NormPosition d)=0;
    virtual void rotate(RawPosition* d)=0;
    virtual void rotate(NormPosition* d)=0;

    
    virtual void setPower(int j,bool state=true)=0;

    virtual RawPosition getRawPosition(int j)=0;
    virtual NormPosition getNormPosition(int j)=0;

    virtual void getProperty(Property& o, const PropertyRequest & r)=0;
    virtual void setProperty(const Property& i, const PropertyRequest & r)=0;

    virtual void getGetterToken(Token& tk,const PropertyList &info)=0;
    virtual void getOptimizedSplit(Array <Token>& tk,const PropertyList &)=0;
    virtual void getProperty(const Token& tk)=0;


    

    virtual bool check(int m)=0;



    
  };
    

  inline std::ostream& operator<<(std::ostream& o,const Motors::PropertyRequest& r){
    return o<<r.joint<<" "<<en::eio<<r.id<<" "<<r.idx;
  }
  
  inline std::istream& operator>>(std::istream& i,Motors::PropertyRequest& r){
    return i>>r.joint>>en::eio>>r.id>>r.idx; 
  }
  


}


//virtual void getSetterToken(Token& tk,const PropertyList &info)=0;
    //virtual void setProperty(const Token& tk)=0;

//typedef void (*NormalizeF)(int joint,int pidx);
    

    /*class Normalizer{
      virtual NormalizedProperty normalize(Property value)=0;
      virtual Property unnormalize(NormalizedProperty value)=0;
      };*/

    
    /*class PropertyToken{
      
    private:
      PropertyToken(const PropertyToken& b){};
      PropertyToken& operator=(const PropertyToken& b){return *this;};
    public:
      Deletable* ginfo;
      PropertyToken():ginfo(NULL){}
      ~PropertyToken(){
	if (ginfo!=NULL) delete ginfo;
      }      
      };*/



    //virtual void getNormalizer(Reference<Normalizer>& norm, PropertyRequest& r)=0;

#endif
