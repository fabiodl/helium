/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef NOTIFIEDMOTORLINEARMODE
#define NOTIFIEDMOTORLINEARMODE

#include <helium/robot/motorLinearMode.h>

namespace helium{

template <typename MotorSign,motor::MotorLevel L>
struct NotifiedLinearModeSignature:public CellSignature{
  typedef typename MotorLinearMode<MotorSign,L>::P GT;
  typedef Void GP;
  typedef Void ST;
  typedef Void SP;
  typedef GT UT;  
  typedef Void UP;

  static const bool getRequiresParam=false;
  static const bool setRequiresParam=false;
  static const bool connectRequiresParam=false;
    
};


template <typename MotorSign,motor::MotorLevel L>
class MotorNotifiedLinearMode:public MotorLinearMode<MotorSign,L>{

  typedef MotorLinearMode<MotorSign,L> Base;
  typedef typename Base::Spool Spool;
  
  class MNLMPositionCell:public TypedCell<NotifiedLinearModeSignature<MotorSign,L> >,
		     public Cell::CellCallback,
		     public ConnReference<Cell::CellSignalData>
  {
    MotorNotifiedLinearMode& owner;    
    friend class MotorNotifiedLinearMode;
    CellSignal<typename NotifiedLinearModeSignature<MotorSign,L>::GT,true,false> sig;
    
    void do_getData(typename Base::P& pos);
    void getData(typename Base::P& pos);

    std::string getName() const{
      return "LinearMode position";
    }


    inline int id(){
      //std::cout<<"id is "<<(this-owner.posc.data)<<std::endl;
      return this-owner.posc.data;
    }

    inline bool isSpecial(typename MotorSign::Pos& r,motor::Interface::Position n){
      if (n==motor::Interface::INVALIDPOS){
	r=MotorSign::INVALIDPOS;
	return true;
      }
      if (n==motor::Interface::MOVINGPOS){
	r=MotorSign::MOVINGPOS;
	return true;
      }
      if (n==motor::Interface::FREEPOS){
	r=MotorSign::FREEPOS;
	return true;
      } 
      return false;
    }

    void operator()(Cell::CellSignalData dp){   
      double d=*static_cast<const motor::PositionCellSignature::UT*>(dp);
      //std::cout<<"The position is "<<d<<" and signal ["<<id()<<"]"<<owner.info.currPos[id()]<<"(="<<std::hex<< owner.info.currPos[id()]<<std::dec<<")"<<std::endl;

      typename MotorSign::Pos r;
      if (isSpecial(r,d)){
	sig(r);
      }else{
	sig(owner.info.currPos[id()]);
      }
    }
    void connect(Cell::CellConnection& ref){
      sig.connect(ref);
    }

    void disconnect(Cell::CellConnection& ref){
      sig.disconnect(ref);
    }
  public:
    MNLMPositionCell( MotorNotifiedLinearMode& powner,int id):
      //id because the pointer in the owner is not ready yet
      ConnReference<Cell::CellSignalData>(this),
      owner(powner){
      owner.info.hlPos[id].connect(this);
    }

    ~MNLMPositionCell(){
      owner.info.hlPos[id()].disconnect(this);
    }
  };
  
  friend class PositionCell;

 


  void addMotor(typename Base::Motors& m,int j,motor::ModeId prevMode){
    Base::addMotor(m,j,prevMode);
    posc[j].sig(this->info.currPos[j]);   
  }

public:

    Array<MNLMPositionCell,
	array::RESIZABLE,
	array::InternalAllocator<ParamIdxAllocator<MNLMPositionCell,MotorNotifiedLinearMode&,
						MotorNotifiedLinearMode&> >
	> posc;

  
  MotorNotifiedLinearMode(Spool& spool,TimeSource& timeSource):   
    MotorLinearMode<MotorSign,L>(spool,timeSource),
    posc(*this,spool.getMotorNum())
  {
  }

};//MotorNotifiedLinearMode

  template <typename MotorSign,motor::MotorLevel L>
  struct DefDescr<MotorNotifiedLinearMode<MotorSign,L> >:public  DefDescr<MotorLinearMode<MotorSign,L> > { 
    DefDescr(MotorNotifiedLinearMode<MotorSign,L>& mlm):
      DefDescr<MotorLinearMode<MotorSign,L> >(mlm){      
    }

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      DefDescr<MotorLinearMode<MotorSign,L> >::getMapMembers(hopname,members);
      IDPath path("target.linear",".");
      path.downPath(mpAdapter<MotorSign,L>::modeId);
      path.downPath("current");
      members.push_back(vectMapping(static_cast<MotorNotifiedLinearMode<MotorSign,L>&>(this->obj).posc,"motor",path) );
    }
    
  };


}//helium

#include <helium/robot/motorNotifiedLinearMode.hpp>

#endif
