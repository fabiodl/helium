/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_EXCEPTION_
#define HE_EXCEPTION_

#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <helium/core/typename.h>


namespace helium{
  
    //fwd decl    
      template<typename T>
      inline std::string toString(const T& t);


  namespace exc{
      
    inline std::string addDetail(const std::string& s){
      if (s=="") return s;
      return ":"+s;
    }

    inline std::string addDetail(const std::string& s,const std::string& opens,const std::string& closes){
      if (s=="") return "";
      return opens+s+closes;
    }

    

    class Exception: public std::runtime_error{
    public:
      Exception(const std::string &s);
      virtual void rethrow()=0;/*{
				 throw *this;
				 }*/
      
      virtual Exception* clone() const=0;/*{
					   return new(*this);
					   }*/
      virtual ~Exception() throw(){
      }
      
    };

    
    inline std::ostream& operator<<(std::ostream& o,const helium::exc::Exception &e){
      return o<<e.what();
    }

    

 
    class Bug:public Exception{
    public:
      Bug(const std::string &s="");      
      virtual void rethrow();
      Exception* clone() const;

    };
    
    class FileOpenRead:public Exception{
    public:
      FileOpenRead(const std::string &filename);      
      virtual void rethrow();      
      Exception* clone() const;
    };
    
    class FileOpenWrite:public Exception{
    public:
      FileOpenWrite(const std::string &filename);      
      virtual void rethrow();      
      Exception* clone() const;
    };


    class SyntaxError:public Exception{
    public:
      SyntaxError(const std::string &s);
      virtual void rethrow();
      Exception* clone() const;
    };

    class BadFileFormat:public Exception{
    protected:
      BadFileFormat(const std::string& s);
    public:
      BadFileFormat(const std::string& filetype,const std::string& filename,const std::string& reason="");
      
      virtual void rethrow();

      Exception* clone() const;
    };


    class XMLError:public BadFileFormat{

    public:
      XMLError(const std::string &s);      
      virtual void rethrow();
      Exception* clone() const;
    };


    

    class InvalidOperation:public Exception{
    public:
      InvalidOperation(const std::string &s);
      virtual void rethrow();
      Exception* clone() const;
    };


    class InvalidDataValue:public Exception{
    public:
      template<typename T> InvalidDataValue(const T& t,const std::string& s=""):Exception("Invalid value for "+Typename<T>::name+addDetail(s)){	
      }
     
      InvalidDataValue(const std::string& what,const std::string& reason="");
      InvalidDataValue(const char* what,const std::string& reason="");
      InvalidDataValue(const InvalidDataValue& v,const std::string& reason);

      virtual void rethrow();
      Exception* clone() const;
    };


    class InvalidCast:public InvalidOperation{

      template <typename F,typename T> std::string getMessage(const F &f){
	std::stringstream ss;
	ss<<"Conversion from "<<Typename<F>::name<<"=["<<f<<"] to "<<Typename<T>::name;
	return ss.str();
      }
    public:

      template<typename T,typename T2>
      InvalidCast(const T &t,const T2& t2):InvalidOperation(getMessage<T2,T>(t2)){
      }
      
      virtual void rethrow();
      Exception* clone() const;

    };
   

  class DuplicateID:public exc::Exception{
    template <typename I> std::string getMessage(const I &id){
      std::stringstream ss;
      ss<<"duplicate id:"<<id;
      return ss.str();
    }
    
    template <typename I,typename J> std::string getMessage(const I &id1,const J &id2){
      std::stringstream ss;
      ss<<"duplicate id:"<<id1<<" - "<<id2;
      return ss.str();
    }
        
    public:
    template <typename I> DuplicateID(const I &id):Exception(getMessage(id)){
    }
    template <typename I,typename J> DuplicateID(const I &id1,const J &id2):Exception(getMessage(id1,id2)){
    }
    virtual void rethrow();      
    Exception* clone() const;
    
    }; //DuplicateID


    class IDException:public exc::Exception{
    protected:
      template <typename I> std::string getMessage(const std::string &s,I &id){
	std::stringstream ss;
	ss<<s;
	ss<<id;
	return ss.str();
      }
      IDException(const std::string &s);

      virtual void rethrow();

      Exception* clone() const;

    };//IDException

    class UnknownID:public IDException{
      
    public:
      template <typename I> UnknownID(const I &id):IDException(getMessage("unknown id:",id)){
      }
      virtual void rethrow();      
      Exception* clone() const;
    };//UnknownID



    class MissingData:public IDException{
      public:
      MissingData();
      
      template <typename I> MissingData(const I &id):IDException("no data available for "+toString(id)){
      }
      virtual void rethrow();

      Exception* clone() const;
    };//MissingData

    class ConstChange:public Exception{
    public:
      ConstChange(const std::string& s);
      
      virtual void rethrow();
      Exception* clone() const;
     
    };//ConstChange


    class UnimplementedFeatureException:public Exception{
    public:
      UnimplementedFeatureException(const std::string& s);
      
      virtual void rethrow();
      Exception* clone() const;
     
    };//UnimpletedFeatureException


    class NetException:public Exception{
    public:

      NetException(const std::string& host,int port,const std::string& s);
      

      NetException(const std::string& s);      
      virtual void rethrow();
      Exception* clone() const;
    };

  
    class DeviceNotFoundException:public Exception{
    public:
      DeviceNotFoundException(const std::string&);
      DeviceNotFoundException(int vendorID,int productID,const std::string& name="");
      virtual void rethrow();
      Exception* clone() const;
    };

    

    class ThreadException:public Exception{
    public:
      ThreadException(const std::string& s);

      virtual void rethrow();
      Exception* clone() const;
    };


    class MutexException:public Exception{
    public:
      MutexException(const std::string& name,const std::string& err);

      virtual void rethrow();
      Exception* clone() const;
    };


    class SemaphoreException:public Exception{
    public:
      SemaphoreException(const std::string& err);      
      virtual void rethrow();
      Exception* clone() const;
    };
    


    class SendingOnDisconnectedSocket:public NetException{
    public:
      SendingOnDisconnectedSocket(const std::string& s="");


      SendingOnDisconnectedSocket(const std::string& host,int port,const std::string& s="");

      virtual void rethrow();

      Exception* clone() const;
    };


    class WaitingOnDisconnectedSocket:public NetException{
    public:
      WaitingOnDisconnectedSocket(const std::string& s="");
      
      virtual void rethrow();

      Exception* clone() const;
    };


    class InvalidNetOperation:public NetException{
    public:
      InvalidNetOperation(const std::string& s="");

      InvalidNetOperation(const std::string& host, int port,const std::string& s="");
      
      virtual void rethrow();
      Exception* clone() const;

    };
    

    class ExceptionHandler{
    public:
      virtual void handle(Exception &e)=0;      
    };


    class MultipleException:public Exception{
      std::vector<Exception*> excs;
      
      
      std::string message(const std::vector<Exception*>& excs);
      
      
      virtual void rethrow();//subexceptions are lost!
      
      Exception* clone() const;//subexceptions are lost!

      
    public:
      
      size_t size() const;
      
      Exception* operator[](int i);

      void handleUsing(ExceptionHandler* eh);

      MultipleException(std::vector<Exception*>& pexcs);//clears the vector      
      ~MultipleException() throw();
      
    };//MultipleException

    class MultipleExceptionIterator{   
      size_t i;
      MultipleException& e;
    public:
      MultipleExceptionIterator(MultipleException& pe);
      
      MultipleExceptionIterator& operator++();
      
      bool operator!=(const size_t& n);
      
      bool operator<(const size_t& n);

      void operator()();//throws the current excpetion
    };
    



    class MultipleExceptionHolder{
      std::vector<Exception*> excs;           
    public:            

      void handle(const Exception& e);//cloned internally
      void check();
      ~MultipleExceptionHolder();

    };//MultipleExceptionHolder
  

    class ExceptionHolder:public ExceptionHandler{
      Exception* ep;
    public:
      ExceptionHolder();      
      void handle(Exception &e);
      void check();      
      void clear();
      virtual ~ExceptionHolder();      
    };

    
    class PrintExceptionHandler:public ExceptionHandler{
      void handle(Exception &e);
    };
    
    


  }//ns exc
  
 
  
}//ns helium

    

#endif
