/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SIGNALLEDARRAY
#define HE_SIGNALLEDARRAY

#include <helium/system/cellSignal.h>
namespace helium{


  template<typename T>
  class SignalledArray{
    //Mutex access;
    int n;
    
    std::pair<int,T>* data;    

    typedef  CellSignal<std::pair<int,T>,false,false > PairSig;
    typedef  CellSignal<T,false,false> AbbrvSig;

    PairSig* conns;
    AbbrvSig* abbrvConns;


  public:      
    
    SignalledArray(int pn)://access("sar"),
			   n(pn),data(new std::pair<int,T>[n]),
			   conns(new PairSig[n]),
			   abbrvConns(new AbbrvSig[n]){
      for (int i=0;i<n;i++){
	data[i].first=i;
      }
    }

    ~SignalledArray(){
      delete[] data;
      delete[] conns;
      delete[] abbrvConns;
    }

    void setAll(T t){
      for (int i=0;i<n;i++){
	data[i].second=t;
      }
    }
    
    const T& operator[](int idx){
      return data[idx].second;
    }

    size_t size() const{
      return n;
    }

    bool connect(DisconnectInfo* &disc,Cell::CellCallback* c,int id){
      bool up=conns[id].listenerNum()==0;
      conns[id].connect(disc,c);
      return up;
    }
    
    bool disconnect(DisconnectInfo* &disc,int id){
      conns[id].disconnect(disc);
      return conns[id].listenerNum()==0;
    }

    bool abbrvConnect(Cell::CellConnection& c,int id){
      bool up=abbrvConns[id].listenerNum()==0;
      abbrvConns[id].connect(c);      
      return up;
    }
  
    bool abbrvDisconnect(Cell::CellConnection& c,int id){
      abbrvConns[id].disconnect(c.disc);
      return abbrvConns[id].listenerNum()==0;
    }

    inline void set(int id,T value){
      data[id].second=value;
      conns[id](data[id]);
      abbrvConns[id](value);
    }
    
    inline void set(const std::pair<int,T>& val){
      set(val.first,val.second);
    }


  };



}


#endif
