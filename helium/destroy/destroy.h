#ifndef _DESTROY_
#define _DESTROY_
#include <set>
#include <helium/callback/tsSignal.h>
namespace helium{
  
  template<typename T,bool scalar=true> class Pointer{
    T* t;
  public:
  Pointer():t(NULL){
    }
  Pointer(T* pt):t(pt){
    }
  Pointer(T& pt):t(&pt){
    }

    void destroy(){
      if (t!=NULL){
	if (scalar)
	  delete t;
	else
	  delete[] t;
      }
    }
    
    virtual ~Pointer(){
      destroy();
    }
    
    inline T* operator->(){
      return t;
    }

    inline T& operator*(){
      return *t;
    }

    inline Pointer& operator=(T* pt){
      destroy();
      t=pt;
      return *this;
    }

    inline bool operator==(T* b){
      return t==b;
    }

  };//Pointer;



  template <typename T>
  class DeleteList{
  public:

    std::set<T*> toDelete;

    void addForDeletion(T* t){
      //std::cout<<"adding "<<t<<"from list "<<this<<std::endl;
      toDelete.insert(t);
    }

    void removeFromDeletion(T* t){
      //std::cout<<"removing "<<t<<"from list "<<this<<std::endl;
      toDelete.erase(t);
    }

    virtual ~DeleteList(){
      //std::cout<<"Deleting deletelist "<<this<<std::endl;
      for (typename std::set<T*>::iterator i=toDelete.begin();i!=toDelete.end();i++){
	//std::cout<<"deleting "<<*i<<std::endl;
	delete *i;	
      }
    }//~DeleteList

    T* operator=(T* t){
      addForDeletion(t);
      return t;
    }

  };

  template<typename T>
  std::ostream& operator<<(std::ostream& o,const DeleteList<T>& t){
    for (typename std::set<T*>::const_iterator i=t.toDelete.begin();i!=t.toDelete.end();++i){
      o<<" "<<*i;
    }
    return o;
  }

  
  template<bool signal=true,bool ts=true> class NotifyDelete{
  protected:
    SignalType<void,ts> onDelete;

    virtual ~NotifyDelete(){
      onDelete();
    }
  };
  


};//helium

#endif
