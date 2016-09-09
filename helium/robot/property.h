#ifndef HE_PROPERTY
#define HE_PROPERTY



namespace helium{

  template<typename T>
  struct Property{
    typedef T type;
  };
  

  template<typename T>
  class NotifiedProperty:public Property<T>{
  public:
    Callback<void> notify*;
    NotifiedProperty():notify(NULL){}    
  };


  class PropertyToken{

    private:
      PropertyToken(const PropertyToken& b){};
      PropertyToken& operator=(const PropertyToken& b){return *this;};
    public:
      Deletable* ginfo;
      PropertyToken():ginfo(NULL){}
      ~PropertyToken(){
	if (ginfo!=NULL) delete ginfo;
      }


      
  };

 

};

#endif
