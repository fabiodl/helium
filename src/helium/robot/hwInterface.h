/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_HWINTERFACE
#define HE_HWINTERFACE

#include <vector>
#include <helium/vector/array.h>
#include <helium/util/ioUtil.h>
#include <helium/smartpointer/reference.h>
#include <helium/core/deletable.h>
#include <helium/signal/signal.h>

namespace helium{
  

  struct HwIdList{
    std::vector<int> ids;
    
    HwIdList(){}
    
    HwIdList(size_t n):ids(n){}
    void fillConsecutive(){
      for (std::size_t i=0;i<ids.size();i++){
	ids[i]=i;
      }
    }

    bool operator<(const HwIdList& b) const{
      if (ids.size()<b.ids.size()) return true;
      if (ids.size()>b.ids.size()) return false;
      //size is same
      for (std::size_t i=0;i<ids.size();i++){
	if (ids[i]<b.ids[i]) return true;
      }
      return false;
    }
  };

  struct SingleHwIdList:public HwIdList{
    SingleHwIdList(int id){
      ids.push_back(id);
    }
  };
  
  inline std::ostream& operator<<(std::ostream& o,const HwIdList& l){
    sizeWrite(o,l.ids);
    return o;
  }
  
  inline std::istream& operator>>(std::istream& i,HwIdList& l){
      sizeRead(l.ids,i);
      return i;
  }


  enum DevType{UNNAMEDDEVICE,NAMEDDEVICE,MULTIPLEXINGDEVICE,NAMEDMULTIPLEXINGDEVICE};
  
  enum HwState{UNKNOWNHWSTATE,WORKINGHWSTATE,
	       GENERICFAULTYHWSTATE,FAULTYVALUEHWSTATE,UNREACHABLEHWSTATE};


    
  template<>  struct EnumStrings<HwState>{
    static const int NUM=5;
    static const char* strings[NUM];
  };


  inline bool isFaulty(HwState s){
    return s==FAULTYVALUEHWSTATE||s==UNREACHABLEHWSTATE||s==GENERICFAULTYHWSTATE;
  }

  typedef std::pair<int,HwState> HwStateNotification;


  class HwInterface{
  public:
    typedef SingleReference<Deletable> WorkingToken;
    
    virtual HwState isWorking(int id)=0;
    virtual void isWorking(Array<HwState>& ,const HwIdList&)=0;
    
    virtual void getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList &) const=0;        
    virtual void isWorking(Callback<const HwStateNotification&>*,const WorkingToken&)=0;

  };



 


}


#endif
