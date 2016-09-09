/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.2
 * Release_flags s
 */


#include <helium/core/exception.h>
#include <helium/debug/debugOptions.h>
#include <helium/debug/trace.h>
#include <helium/util/ioUtil.h>
#include <helium/util/mout.h>

//#define LOUDEXC

namespace helium{

  
  namespace exc{
    
    Exception::Exception(const std::string &s):std::runtime_error("Error:"+s){
#ifdef TRACEDEBUG

#ifndef LOUDEXC 
      if (!ExceptionsAreSilent){
#endif
	merr(msg::ERR)("Exception:"+s);
	printTrace();
#ifndef LOUDEXC
      }
#endif
#endif
    }
    
    Bug:: Bug(const std::string &s):Exception("Bug"+addDetail(s)){
    }

    void Bug::rethrow(){
      throw *this;
    }
    
    Exception* Bug::clone() const {
      return new Bug(*this);
    }
    
    
    FileOpenRead::FileOpenRead(const std::string &filename):Exception("unable to open file:"+filename){
    }
    
    void  FileOpenRead::rethrow(){
	throw *this;
    }

    Exception* FileOpenRead::clone() const{
      return new FileOpenRead(*this);
    }
    
    
    FileOpenWrite::FileOpenWrite(const std::string &filename):Exception("unable to open file for writing:"+filename){
    }
    
    void  FileOpenWrite::rethrow(){
	throw *this;
    }

    Exception* FileOpenWrite::clone() const{
      return new FileOpenWrite(*this);
    }
    



    
    SyntaxError::SyntaxError(const std::string &s):Exception("Syntax error:"+s){
    }

    void SyntaxError::rethrow(){
      throw *this;
    }

    Exception* SyntaxError::clone() const{
      return new SyntaxError(*this);
    }

    BadFileFormat::BadFileFormat(const std::string& s):Exception(s){}
    BadFileFormat::BadFileFormat(const std::string& filetype,const std::string& filename,const std::string& reason):
	Exception("Bad file format for file "+filename+" opened as "+filetype+
		  addDetail(reason)){
      }

    void BadFileFormat::rethrow(){
      throw *this;
    }

    Exception*  BadFileFormat::clone() const{
	return new BadFileFormat(*this);
    }

    XMLError::XMLError(const std::string &s):BadFileFormat("XML error:"+s){
    }

    void XMLError::rethrow(){
      throw *this;
    }

    Exception*  XMLError::clone() const{
      return new XMLError(*this);
    }
    
    InvalidOperation:: InvalidOperation(const std::string &s):Exception("Invalid operation:"+s){
      printTrace();
    }
    

    void InvalidOperation::rethrow(){
	throw *this;
      }

    Exception*  InvalidOperation::clone() const{
	return new InvalidOperation(*this);
    }

    InvalidDataValue::InvalidDataValue(const std::string& what,const std::string& reason):Exception("Invalid value for "+what+addDetail(reason)){	
    }

    InvalidDataValue::InvalidDataValue(const char* what,const std::string& reason):Exception(std::string("Invalid value for ")+what+addDetail(reason)){	
    }

    InvalidDataValue::InvalidDataValue(const InvalidDataValue& v,const std::string& reason):Exception(std::string(v.what())+" for "+reason){
      }

    void InvalidDataValue::rethrow(){
      throw *this;
    }
    
    Exception* InvalidDataValue::clone() const{
      return new InvalidDataValue(*this);
    }
    
    
    void InvalidCast::rethrow(){
	throw *this;
    }

    Exception* InvalidCast::clone() const{
      return new InvalidCast(*this);
    }

    void DuplicateID::rethrow(){
      throw *this;
    }

    Exception* DuplicateID::clone() const{
      return new DuplicateID(*this);
    }


    IDException::IDException(const std::string &s):Exception(s){
    }

    void IDException::rethrow(){
      throw *this;
    }
    
    Exception* IDException::clone() const{
      return new IDException(*this);
    }

    void UnknownID::rethrow(){
      throw *this;
    }
    
    Exception* UnknownID::clone() const{
	return new UnknownID(*this);
    }
    
    MissingData::MissingData():IDException("null data"){
    }

    void MissingData::rethrow(){
      throw *this;
    }

    Exception*  MissingData::clone() const{
	return new MissingData(*this);
      }

    ConstChange::ConstChange(const std::string& s):Exception(s){
      };
    
    void ConstChange::rethrow(){
      throw *this;
    }
    
     Exception* ConstChange::clone() const{
	return new ConstChange(*this);
     }

    
    UnimplementedFeatureException::UnimplementedFeatureException(const std::string& s):Exception("Unimplemented feature"+addDetail(s)){
    }

    void UnimplementedFeatureException::rethrow(){
      throw *this;
    }

    Exception* UnimplementedFeatureException::clone() const{
      return new UnimplementedFeatureException(*this);
    }
    

    NetException::NetException(const std::string& host,int port,const std::string& s):Exception("TCP connection to "+host+":"+toString(port)+" "+addDetail(s)){
    }

    NetException::NetException(const std::string& s):Exception(s){	
    };
    
     
   void NetException::rethrow(){
      throw *this;
    }
    
     Exception* NetException::clone() const{
	return new NetException(*this);
     }
   

    DeviceNotFoundException::DeviceNotFoundException(const std::string&  s):Exception(s){
    }

    DeviceNotFoundException::DeviceNotFoundException(int vendorID,int productID,const std::string&name ):Exception(stringBegin()<<"Device 0x"<<std::hex<<vendorID<<":0x"<<std::hex<<productID<<addDetail(name," (",")")<<" not found"<<stringEnd()){
    }

   
     
    void  DeviceNotFoundException::rethrow(){
      throw *this;
    }
    
     Exception*  DeviceNotFoundException::clone() const{
	return new  DeviceNotFoundException(*this);
     }
   




    ThreadException::ThreadException(const std::string& s):Exception("Thread Exception:"+s){
	
    };
    

    void ThreadException::rethrow(){
	throw *this;
      }

    Exception*  ThreadException::clone() const{
	return new ThreadException(*this);
      }


    MutexException::MutexException(const std::string& name,const std::string& err):Exception("Mutex "+name+" error:"+err){
	
      };

    void MutexException::rethrow(){
	throw *this;
      }

    Exception* MutexException::clone() const{
	return new MutexException(*this);
      }

    SemaphoreException::SemaphoreException(const std::string& err):Exception("Semaphore error:"+err){
	
    };

    void SemaphoreException::rethrow(){
	throw *this;
      }
    
    Exception* SemaphoreException::clone() const{
	return new SemaphoreException(*this);
      }

    SendingOnDisconnectedSocket::SendingOnDisconnectedSocket(const std::string& s):NetException("Sending on disconnected socket "+addDetail(s)){	
    };

    SendingOnDisconnectedSocket::SendingOnDisconnectedSocket(const std::string& host,int port,const std::string& s):NetException(host,port,"Sending on disconnected socket"+addDetail(s)){	
    };
    
    void SendingOnDisconnectedSocket::rethrow(){
	throw *this;
      }
    
    Exception* SendingOnDisconnectedSocket::clone() const{
	return new SendingOnDisconnectedSocket(*this);
      }


    WaitingOnDisconnectedSocket::WaitingOnDisconnectedSocket(const std::string& s):NetException("Waiting on disconnected socket "+addDetail(s)){	
      };


    void WaitingOnDisconnectedSocket::rethrow(){
	throw *this;
      }

    Exception* WaitingOnDisconnectedSocket::clone() const{
	return new WaitingOnDisconnectedSocket(*this);
      }


    InvalidNetOperation::InvalidNetOperation(const std::string& s):NetException("Invalid net operation"+addDetail(s)){
      
    }
    
    InvalidNetOperation::InvalidNetOperation(const std::string& host, int port,const std::string& s):NetException(host,port,"Invalid net operation"+addDetail(s)){
      
    }

    void InvalidNetOperation::rethrow(){
	throw *this;
      }

    Exception* InvalidNetOperation::clone() const{
	return new InvalidNetOperation(*this);
      }
    

    std::string MultipleException::message(const std::vector<Exception*>& excs){
	std::stringstream ss;
	for (size_t i=0;i<excs.size();i++){
	  ss<<" "<<*excs[i];
	  }
	return ss.str();
      }

    void MultipleException::rethrow(){
      throw MultipleException(this->excs);//subexceptions are lost!
    }

    Exception* MultipleException::clone() const{
      return new MultipleException(const_cast<MultipleException*>(this)->excs); //subexceptions are lost!
    }

    size_t MultipleException::size() const{
      return excs.size();
    }

    Exception*  MultipleException::operator[](int i){
      return excs[i];
    }

    void MultipleException::handleUsing(ExceptionHandler* eh){
      for(size_t i=0;i<excs.size();i++){
	eh->handle(*excs[i]);
      }
    }

    MultipleException::MultipleException(std::vector<Exception*>& pexcs):
	Exception("Multiple exceptions:"+message(pexcs)),
	excs(pexcs)
      {	  
	pexcs.clear();	  
      }

    MultipleException::~MultipleException() throw(){
      for (size_t i=0;i<excs.size();i++){
	delete excs[i];
      }
    }

    MultipleExceptionIterator::MultipleExceptionIterator(MultipleException& pe):i(0),e(pe){
    }

    MultipleExceptionIterator&  MultipleExceptionIterator::operator++(){
      i++;
      return *this;
    }
    
    bool MultipleExceptionIterator::operator!=(const size_t& n){
      return i!=n;
    }
      
    bool MultipleExceptionIterator::operator<(const size_t& n){
      return i<n;
    }

    void MultipleExceptionIterator::operator()(){
      e[i]->rethrow();
    }

    void MultipleExceptionHolder::handle(const Exception& e){
      excs.push_back(e.clone());//deleted by ~MultipleException()
    }

    void MultipleExceptionHolder::check(){
	if (excs.size()==0) return;
	if (excs.size()>1){
	  throw MultipleException(excs);
	}
	
	//size==1
	try{
	  excs[0]->rethrow();	    	
	}catch(...){
	  delete excs[0];
	  excs.clear();
	  throw;
	}
      }//check



    MultipleExceptionHolder::~MultipleExceptionHolder(){
      check();
    }


    ExceptionHolder::ExceptionHolder():ep(NULL){
    }

    void ExceptionHolder::handle(Exception &e){
      ep=e.clone();
    }

    void ExceptionHolder::check(){
      if (ep!=NULL){
	try{
	  ep->rethrow();
	}catch(...){
	  delete ep;
	  ep=NULL;
	  throw;
	}
      }
    }//check
     
    ExceptionHolder::~ExceptionHolder(){
	check();
    }
 

    void PrintExceptionHandler::handle(Exception &e){
      merr(msg::ERR)(e.what());
    }
    


  }//ns exc
}//ns helium
