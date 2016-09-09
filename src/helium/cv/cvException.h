#ifndef HE_CVEXCEPTION
#define HE_CVEXCEPTION

namespace helium{

  namespace exc{
    class CvException:public Exception{
    public:
      CvException(const std::string& s):Exception("OpenCV"+addDetail(s)){
      }
      
      virtual void rethrow(){
	throw *this;
      }

      Exception* clone() const{
	return new CvException(*this);
      }
    };


  }//exc


  
}


#endif
