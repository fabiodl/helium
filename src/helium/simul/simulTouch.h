#ifndef HE_SIMULTOUCH
#define HE_SIMULTOUCH

#include <helium/robot/sensorInterface.h>

namespace helium{

  void markUnused(Robot* robot,const std::set<int>& usedSensors){
    for (size_t s=0;s<robot->skeletonNodes.size();s++){
      if (robot->skeletonNodes[s]->part!=NULL){
	for (size_t g=0;g<robot->skeletonNodes[s]->part->elements.size();g++){
	  Element* currElement=(Element*)robot->skeletonNodes[s]->part->elements[g];	  
	  if ((currElement->customizationType&(1<<MOUSESENSORBOXCUSTOMIZATION))){
	      for (int i=0;i<6;i++){
		int id=((MouseSensorBoxCustomization*)(currElement->elementCustomizations[MOUSESENSORBOXCUSTOMIZATION]))->id[i];
		if (usedSensors.count(id)==0){
		  ((MouseSensorBoxCustomization*)(currElement->elementCustomizations[MOUSESENSORBOXCUSTOMIZATION]))->active[i]=false;	      
		}//if
	      }//for i
	    }//if mousesensorcustomization
	
	}//for g
      }//if part!=NULL
    }//for s
  }//markUnused


  template<typename T>
  class FuseSensors:public sensor::Set<T>{
    std::vector<sensor::Set<T>* > srcs;
    int n;

    public FuseToken:public Deletable{
      Array<Deletable*> tokens;
      
    public:
      std::set<int> idx;

      FuseToken(const std::set<int>& pidx,int srcn):tokens(srcn),idx(pidx){
      }      
      virtual ~FuseToken(){
	destroyS(tokens);
      }           
    };

  public:

    FuseSensors(int pn):n(pn){
    }

    void addSet(sensor::Set<T>* s){
      srcs.push_back(s);
    }
    
    T rawRead(int idx){
      T m=srcs[0]->rawRead(idx);
      for (size_t i=1;i<srcs.size();i++){
	m=max(m,srcs[i]->rawRead(idx));
      }
      return m;
    }

    void rawRead(T* t){
      Array<T> data(n);
      srcs[0]->rawRead(t);
      for (size_t i=1;i<srcs.size();i++){
	srcs[i]->rawRead(data.data);
	accumulateMax(t,data);
      }
    }
    
    void rawRead(T* t, const std::vector<int>& idx){
      Array<T> data(n);      
      srcs[0]->rawRead(t,idx);
      for (size_t i=1;i<srcs.size();i++){
	srcs[i]->rawRead(data.data,idx);
	accumulateMax(t,data,idx.size());
      }      
    }

    

    void getToken(Token& t, const std::set<int>& toRead){
      FuseToken* ft=new FuseToken(toRead,n);
      Token st;
      for (size_t i=0;i<srcs.size();i++){
	srcs[i]->getToken(st,toRead);
	ft->tokens[i]=st.takePointer();
      }
      t=ft;
    }

    void rawRead(T* t,Token& tk){
      Array<T> data(n);      
      srcs[0]->rawRead(t,idx);
      std::vector<int>& idx=trust_cast<FuseToken*>(&tk)->idx;
      for (size_t i=1;i<srcs.size();i++){
	srcs[i]->rawRead(data.data,idx);
	for (size_t j=0;j<idx.size();j++){
	  t[j]=(std::max)(data[j],t[j]);
	}
      }  
      
      size_t size(){
	return srcs[0]->size();
      }

      bool isWorking(int n){
	for (size_t i=0;i<srcs.size();i++){
	  if (!srcs[i]->isWorking(n)) return false;
	}
	return true;
      }

      bool isWorking(){
	for (size_t i=0;i<srcs.size();i++){
	  if (!srcs[i]->isWorking()) return false;
	}
	return true;
      }      
    }
    

  };//FuseSensors


}


#endif


