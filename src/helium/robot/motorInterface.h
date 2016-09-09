/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_MOTORINTERFACE
#define HE_MOTORINTERFACE

#include <iostream>
#include <helium/util/ioUtil.h>
#include <helium/core/triple.h>
#include <helium/signal/callback.h>
#include <helium/signal/signal.h>
#include <helium/core/deletable.h>
#include <helium/vector/array.h>
#include <helium/smartpointer/reference.h>
#include <helium/robot/hwInterface.h>
namespace helium{

  

  namespace motor{


    enum PropertyID{TARGETPOSITION,POTENTIOPOSITION,POWER,TEMP,//VSSV410
		    RAWPROPERTIES,
		    PROPERTIESNUM};  
  }



    template<> struct EnumStrings<motor::PropertyID>{
      static const int NUM=motor::PROPERTIESNUM;
      static const char* strings[NUM];
    };

  namespace motor{    

    struct JointPropertyRequest{
      PropertyID id;
      int idx;
      bool operator<(const JointPropertyRequest& b) const{
	if (id<b.id) return true;
	return idx<b.idx;
      }
      JointPropertyRequest(){}
      JointPropertyRequest(PropertyID pid):id(pid),idx(-1){}
      JointPropertyRequest(PropertyID pid,int pidx):id(pid),idx(pidx){}
    };


    struct PropertyRequest{
      int joint;
      PropertyID id;
      int idx;
      
      PropertyRequest(){}

      PropertyRequest(int pjoint,PropertyID pid,int pidx=-1):joint(pjoint),id(pid),idx(pidx){
      } 

      PropertyRequest(int pjoint,const JointPropertyRequest& r):
	joint(pjoint),id(r.id),idx(r.idx){	
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

    inline std::ostream& operator<<(std::ostream& o,const JointPropertyRequest& r){
      return o<<en::eio<<r.id<<" "<<r.idx;
    }
    
    inline std::istream& operator>>(std::istream& i,JointPropertyRequest& r){
      return i>>en::eio>>r.id>>r.idx; 
    }

    inline std::ostream& operator<<(std::ostream& o,const PropertyRequest& r){
      return o<<r.joint<<" "<<en::eio<<r.id<<" "<<r.idx;
    }
    
    inline std::istream& operator>>(std::istream& i,PropertyRequest& r){
      return i>>r.joint>>en::eio>>r.id>>r.idx; 
    }
    

    typedef double NormPos;
    typedef double NormProp;
    

    /*struct NormProp{
      double v,val;
      NormProp(double n=-1):v(n),val(n){
      }
      };*/

    /*struct NormPos{
      double v,val;
      NormPos(double n=-1):v(n),val(n){
      }
      NormPos operator-(NormPos& n){
	return	NormPos( v-n.v);
      }
      NormPos operator-() const{
	return	NormPos(-v);
      }

      operator double() const{
	return v;
      }


      operator int() const{
	return v;
      }
      template <typename T> bool operator>(const T& s) const{
	return v>s;
      }
      };
    
    inline NormPos operator*(const double& d,const NormPos& n){
      return NormPos(d*n.v);
    }
    inline NormPos operator*(const NormPos& n,const double& d){
      return NormPos(d*n.v);
    }

    

    inline std::ostream& operator<<(std::ostream& o,const NormProp & p){
      return o<<p.v;
    }
    
    inline std::ostream& operator<<(std::ostream& o,const NormPos & p){
      return o<<p.v;
    }
    

    inline std::istream& operator>>(std::istream& o,NormProp & p){
      return o>>p.v;
    }
    
    inline std::istream& operator>>(std::istream& o,NormPos & p){
      return o>>p.v;
      }*/
   
    enum MotorLevel{IFACELEVEL,RAWLEVEL,HWLEVEL};
    static const int MOTORLEVELS=3;
    
    typedef std::string IfaceId; 
    
    

    class Interface{



    public:    

      typedef NormProp Property;
      //typedef double Property;

      typedef NormPos Position;
      //typedef double Position;


      static const Property INVALIDPROP;

      static const Position INVALIDPOS;
      static const Position FREEPOS;
      static const Position MOVINGPOS;
      static const Position COLLISIONPOS;

      virtual int getMotorNum()=0;

      virtual void rotate(int j,Position val)=0;      
      virtual void rotate(const Position* d)=0;
      virtual void rotate(const bool*, const Position* d)=0;

      virtual Position getPosition(int joint)=0;            
      virtual void getPosition(Position* p)=0;
     
      virtual void connect(ConnReference<Position>& conn,int j)=0;
      virtual void disconnect(ConnReference<Position>& conn,int j)=0;
      virtual IfaceId getIfaceId()=0;
    };

  
    template<typename PropType,
	     typename PosType,
	     PropType invalidProp,
	     PosType invalidPos,
	     PosType movingPos,
	     PosType freePos,
	     PosType collisionPos
	     > struct EssentialSign{
      typedef PropType Pros;
      typedef PosType Pos;
      static const PropType INVALIDPROP;
      static const PosType INVALIDPOS;
      static const PosType MOVINGPOS;
      static const PosType FREEPOS;
      static const PosType COLLISIONPOS;
    };

  template<typename PropType,
	   typename PosType,
	   PropType invalidProp,
	   PosType invalidPos,
	   PosType movingPos,
	   PosType freePos,
	   PosType collisionPos
	   > 
  const PropType EssentialSign<PropType,PosType,
			       invalidProp,
			       invalidPos,movingPos,freePos,collisionPos>::INVALIDPROP=invalidProp;
    

    template<typename PropType,
	     typename PosType,
	     PropType invalidProp,
	     PosType invalidPos,
	     PosType movingPos,
	     PosType freePos,
	     PosType collisionPos
	     > 
    const PosType EssentialSign<PropType,PosType,
				invalidProp,
				invalidPos,movingPos,freePos,collisionPos>::INVALIDPOS=invalidPos;


    

    template<typename MotorSign> struct Raw{

      typedef typename MotorSign::Prop Property;
      typedef typename MotorSign::Pos Position;
      /*static const Property INVALIDPROP;
      
      static const Position INVALIDPOS;
      static const Position MOVINGPOS;
      static const Position FREEPOS;*/


      class PropNormalizer{	
      public:
	virtual Interface::Property normProp(typename Raw::Property value,const PropertyRequest& r)=0;
	virtual typename Raw::Property unnormProp(Interface::Property value,const PropertyRequest& r)=0;
      };
      
      class PosNormalizer{
      public:
	virtual Interface::Position normPos(typename Raw::Position value,int joint)=0;
	virtual typename Raw::Position unnormPos(Interface::Position value,int joint)=0;
      };

      

      
      typedef triple<Property,const PropertyRequest*,PropNormalizer*> PropertyNotify;
      typedef std::pair<Callback<PropertyNotify*>*,const PropertyRequest*> PropertyNotifyRequest;
      typedef std::list<PropertyNotifyRequest> PropertyList;
      typedef SingleReference<Deletable> Token; 


      
    
      
      class Set:public Interface{
      public:
	
	virtual void rawRotate(int j,typename Raw::Position val)=0;
	virtual void rawRotate(const typename Raw::Position* d)=0;
	virtual void rawRotate(const bool*,const typename Raw::Position* d)=0;
	virtual typename Raw::Position rawGetPosition(int joint)=0;       	
	virtual void rawGetPosition(typename Raw::Position *p)=0;   
	
	virtual void rawConnect(ConnReference<typename Raw::Position>& conn,int j)=0;
	virtual void rawDisconnect(ConnReference<typename Raw::Position>& conn,int j)=0;
      };
    
      class Hardware:public Set,public HwInterface{
      public:
	virtual PropNormalizer& convProp()=0;
	virtual PosNormalizer& convPos()=0;
	virtual typename Raw::Position getPotentiometer(int joint,int checks=0)=0;       
	virtual void rawGetProperty(typename Raw::Property& o, const PropertyRequest & r)=0;
	virtual void rawSetProperty(const typename Raw::Property& i, const PropertyRequest & r)=0;    	
	virtual void rawGetGetterToken(Token& tk,const PropertyList &info)=0;
	virtual void rawGetOptimizedSplit(Array <Token>& tk,const PropertyList &)=0;
	virtual void rawGetProperty(const Token& tk)=0;	

	
	virtual void setPowerState(int j,bool state=true)=0;
	virtual bool getPowerState(int j)=0;
	virtual void getPowerState(bool* b)=0;

	
	virtual void hwRotate(int j,typename Raw::Position val)=0;
	virtual void hwRotate(const typename Raw::Position* d)=0;
	virtual void hwRotate(const bool*,const typename Raw::Position* d)=0;
      };

      
      class NoHardware:public Hardware{	
	PropNormalizer& convProp();
	PosNormalizer& convPos();
	typename Raw::Position getPotentiometer(int joint,int checks);
	void rawGetProperty(typename Raw::Property& o, const PropertyRequest & r);
	void rawSetProperty(const typename Raw::Property& i, const PropertyRequest & r);
	void rawGetGetterToken(Token& tk,const PropertyList &info);
	void rawGetOptimizedSplit(Array <Token>& tk,const PropertyList &);
	void rawGetProperty(const Token& tk);
	void setPowerState(int j,bool state=true);
	bool getPowerState(int j);
	void getPowerState(bool* );
	void rotate(int, Interface::Position);	
	void rotate(const Interface::Position*);
	void rotate(const bool*,const Interface::Position*);

	Interface::Position getPosition(int j);
	void getPosition(Interface::Position* p);
	void rawGetPosition(typename Raw::Position* p);
	void connect(ConnReference<Interface::Position>&, int);
	void disconnect(ConnReference<Interface::Position>&,int j);
	void rawConnect(ConnReference<typename Raw::Position>& conn,int j);
	void rawDisconnect(ConnReference<typename Raw::Position>& conn,int j);
	void rawRotate(int, typename Raw::Position);
	void rawRotate(const typename Raw::Position*);
	void rawRotate(const bool*,const typename Raw::Position*);
	typename Raw::Position rawGetPosition(int);
	IfaceId getIfaceId();
	void hwRotate(int j,typename Raw::Position val);
	void hwRotate(const typename Raw::Position* d);
	void hwRotate(const bool* ,const typename Raw::Position* d);
	
	
	void getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &) const;
	
	void isWorking(Array<HwState>& s,const HwIdList& id);//assuming  s.size()== id.ids.size()
	HwState isWorking(int id);
	void isWorking(Callback<const HwStateNotification&>*,const HwInterface::WorkingToken&);
	int getMotorNum();
      };

      static NoHardware NOHARDWARE;
      
      

    };//Raw
    
  
    template<typename MotorSign,MotorLevel L> struct  motorTypes{       
    };
    
    template<typename MotorSign> struct motorTypes<MotorSign,IFACELEVEL>{
      typedef Interface Motor;      
    };
    
    template<typename MotorSign> struct  motorTypes<MotorSign,RAWLEVEL>{
      typedef typename Raw<MotorSign>::Set Motor;
    };
    
    template<typename MotorSign> struct  motorTypes<MotorSign,HWLEVEL>{
      typedef typename Raw<MotorSign>::Hardware Motor;
    };
    
    
   
    


  };//motor


  template<typename MotorSign> typename motor::Raw<MotorSign>::NoHardware motor::Raw<MotorSign>::NOHARDWARE;

  template<>  struct EnumStrings<motor::MotorLevel>{
    static const int NUM=3;
    static const char* strings[NUM];
  };


  
}

#include <helium/robot/motorInterface.hpp>

#endif
