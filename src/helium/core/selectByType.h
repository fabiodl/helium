/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_SELECTBYTYPE
#define HE_SELECTBYTYPE

namespace helium{

    template<typename F,typename S,typename U> struct selectByType{            
    };

    template<typename F,typename S> struct selectByType<F,S,F>{
      template<typename T,typename U> inline static T& get(T& f,U& s){
	return f;
      } 
      template<typename T,typename U> inline static const T& get(const T& f,const U& s){
	return f;
      } 

      template<typename D1,typename D2> struct Definition{
	typedef D1 value;
      };
      
    };


    template<typename F,typename S> struct selectByType<F,S,S>{
      template<typename T,typename U> static U& get(T& f,U& s){
	return s;
      }      
      template<typename T,typename U> static const U& get(const T& f,const U& s){
	return s;
      } 
      template<typename D1,typename D2> struct Definition{
	typedef D2 value;
      };
    };

  
  //value is F if T is Fon, S if T is Son
  template<typename F,typename S,typename Fon,typename Son,typename T> struct correspondingType;

  template<typename F,typename S,typename Fon,typename Son> 
  struct correspondingType<F,S,Fon,Son,Fon>{
    typedef F value;
  };

  template<typename F,typename S,typename Fon,typename Son> 
  struct correspondingType<F,S,Fon,Son,Son>{
    typedef S value;
  };


}

#endif
