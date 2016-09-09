#ifndef  HE_ROTOTRANSLATION
#define HE_ROTOTRANSLATION


namespace helium{
  typedef double RotoTranslation[16];


  inline std::ostream& operator<<(std::ostream& o,const RotoTranslation& r){
    for (int i=0;i<4;i++){
      for (int j=0;j<4;j++){
	o<<r[i*4+j]<<" ";
       }
      o<<std::endl;
    }
    return o;
  }


}

#endif
