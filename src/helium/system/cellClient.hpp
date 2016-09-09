/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CELLCLIENTIMPL
#define HE_CELLCLIENTIMPL
#include <helium/system/remoteCell.h>
#include <helium/system/cellProtocol.h>
#include <helium/system/cellSignature.h>

namespace helium{

  template<typename T> 
  CellClient::TypedRequest<T>::TypedRequest(const std::string& premotename,CellClient* c,T* pt):GenericRequest(premotename,c),t(*pt){
      }  


  template<typename T> 
  void CellClient::TypedRequest<T>::readResult(std::istream& i){
    readWholeShws(t,i);
  }
  
  
  template<typename Signature> 
  CellClient::GetRequest<Signature>::GetRequest(const std::string& premotename,CellClient* c,typename Signature::GT* pt):GenericRequest(premotename,c),t(*pt){
  }  


  template<typename Signature> 
  void CellClient::GetRequest<Signature>::readResult(std::istream& i){
    deserializeGet<Signature>(t,i);
  }

  template<typename Signature> 
  CellClient::SetParamRequest<Signature>::SetParamRequest(const std::string& premotename,CellClient* c,typename Signature::SP* pt):GenericRequest(premotename,c),t(*pt){
  }  

  template<typename Signature> 
  void CellClient::SetParamRequest<Signature>::readResult(std::istream& i){
    deserializeSetParam<Signature>(t,i);
  }

  
  template <typename T,typename P> void CellClient::get(GenericRequest *r,const P* p){
    std::stringstream ss;
    void* id=static_cast<void*>(r);
    ss<<helium::ALIASCHAR<<id<<" "<<helium::IDCHAR<<r->remotename<<" "<<requestChar[helium::GETVALUEPARAMMSG]<<" ";
    write(ss,*p);
    ss<<helium::COMMANDTERMINATORCHAR;
    send(ss.str());
    r->wait();
  }
  
  
  template <typename T> void CellClient::set(std::string& remotename,const T* t){      
    std::stringstream ss;
    //std::cout<<"the data is "<<*t<<std::endl;
    void* id=static_cast<void*>(&defaultFail);
    ss<<helium::ALIASCHAR<<id<<" "<<helium::IDCHAR<<remotename<<" "<<requestChar[helium::SETVALUEMSG]<<" ";
    write(ss,*t);
    ss<<helium::COMMANDTERMINATORCHAR;
    send(ss.str());
  }
  
  
  template <typename T> void CellClient::set(GenericRequest *r,const T* t){
    std::stringstream ss;
    void* id=static_cast<void*>(r);
    ss<<helium::ALIASCHAR<<id<<" "<<helium::IDCHAR<<r->remotename<<" "<<requestChar[helium::SETVALUEPARAMMSG]<<" ";
    write(ss,*t);
    ss<<helium::COMMANDTERMINATORCHAR;
    send(ss.str());
    r->wait();
  }
  


}//helium

#endif
