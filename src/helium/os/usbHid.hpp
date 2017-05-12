/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_USBHID_IMPL
#define  HE_USBHID_IMPL
#include <helium/core/void.h>
#include <helium/util/printHex.h>
namespace helium{


  template<int N,bool padRead,bool padWrite>
  UsbMessage<N,padRead,padWrite>::operator const unsigned char*() const{
    return this->data+1;
  }
  

  template<int N,bool padRead,bool padWrite>
  UsbMessage<N,padRead,padWrite>::operator unsigned char*(){
    return this->data+1;
  }
  

  template<int N,bool padRead,bool padWrite>
  const unsigned char& UsbMessage<N,padRead,padWrite>::operator[](int idx) const{
    return this->data[idx+1];
  }
  
  

  template<int N,bool padRead,bool padWrite>
  unsigned char& UsbMessage<N,padRead,padWrite>::operator[](int idx){
    return this->data[idx+1];
  }

  template<int N> struct voidsizeMaker{
    inline static const Void* get(int size){return NULL;}
  };
  template<> struct voidsizeMaker<array::RESIZABLE>{
    inline static const int get(int size){return size+1;}
  };


  template<int N,bool padRead,bool padWrite>
 size_t UsbMessage<N,padRead,padWrite>::payloadSize() const {
   return Super::size()-1;
 }


  template<int N,bool padRead,bool padWrite>
  size_t UsbMessage<N,padRead,padWrite>::completeSize() const{
    return Super::size();
  }

  template<int N,bool padRead,bool padWrite>
  void UsbMessage<N,padRead,padWrite>::resize(size_t n){
    Super::resize(n+1);
  }



  template<int N,bool padRead,bool padWrite>
  UsbMessage<N,padRead,padWrite>::UsbMessage(int size):
    Super(voidsizeMaker<N>::get(size)){
    this->data[0]=0;
  }


  template<int N,bool padRead,bool padWrite>
  UsbMessage<N,padRead,padWrite>& UsbMessage<N,padRead,padWrite>::operator=(const unsigned char* d){
    memcpy(this->data+1,d,payloadSize());
    return *this;
  }

  template<int N,bool padRead,bool padWrite>
  std::ostream& operator<<(std::ostream& o,const  UsbMessage<N,padRead,padWrite>& m){
    o<<(padWrite?"(with report id) ":"");
    printHex(m.data+(padWrite?0:1),padWrite?m.completeSize():m.payloadSize(),o); 
    return o;
  }

  


      template<int N,bool padRead,bool padWrite>
      void UsbHid::sendAll(const UsbMessage<N,padRead,padWrite>& m){
      //start at 1 if there's no id to read
     int toSend=padWrite?m.completeSize():m.payloadSize();     
      if (send(m.data+(padWrite?0:1),toSend)!=toSend){
	throw exc::UsbWriteException(toSend);
      }
    }
    
 
    
    
    template<int N,bool padRead,bool padWrite>
      void UsbHid::recvAll(UsbMessage<N,padRead,padWrite>& m){      
      int toRead=padRead?m.completeSize():m.payloadSize();
      unsigned char* dst=m.data+(padRead?0:1);     
      //std::cout<<"padRead is "<<padRead<<std::endl;
      //std::cout<<"start read of "<<toRead<<std::endl;
      while(toRead>0){
	int r=recv(dst,toRead);
	if (r<0) throw exc::UsbReadException(toRead);
	//std::cout<<"received "<<r<<" bytes"<<std::endl;
	toRead-=r;
	dst+=r;
      }
      //start at 1 if there's no id to read
    }
    
    


    template<int N,bool padRead,bool padWrite>
    int UsbHid::recv(UsbMessage<N,padRead,padWrite>& m){      
      int toRead=padRead?m.completeSize():m.payloadSize();
      unsigned char* dst=m.data+(padRead?0:1);     
      //std::cout<<"padRead is "<<padRead<<std::endl;
      //std::cout<<"start read of "<<toRead<<std::endl;
	int r=recv(dst,toRead);
	if (r<0) throw exc::UsbReadException(toRead);
	//std::cout<<"received "<<r<<" bytes"<<std::endl;
	return r;
      //start at 1 if there's no id to read
    }

}


#endif
