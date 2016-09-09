/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_IOUTIL
#define HE_IOUTIL
#include <sstream>
#include <cstdio>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <helium/core/exception.h>
#include <helium/core/enum.h>
#include <helium/core/triple.h>
#include <helium/vector/array.h>
#include <cctype>
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <helium/core/nan.h>
#include <stdint.h>
namespace helium{


  enum DataIoType{SCALARIO,VECTIO,ENUMIO,NENUMIO}; //NENUM=Numerical Enum


  //N numerical , 
  //SN numerical preceeded by size
  //IT with iterator
  //E use insert
  //R amortized resize
  
  enum ReadPolicyFlags{READSCALAR=0,READN=1,READSN=2,READIT=4,READE=8,READR=16};
  enum WritePolicyFlags{WRITESCALAR=0,WRITEN=1,WRITESN=2,WRITEIT=4};
  


  template<typename T> struct ReadPolicy{
    static const int value=READSCALAR;
  };


  template<typename T> struct WritePolicy{
    static const int value=WRITESCALAR;
  };


  template<typename T> inline void read(T& t,std::istream& i);
  template<typename T> inline void write(std::ostream& o,const T& t);

  //--customizations

  template<typename F,typename S>
  inline void read(std::pair<F,S>& p, std::istream& i);  
  template<typename F,typename S>
  inline void write(std::ostream& o,const std::pair<F,S>& p);

  inline void read(double& d,std::istream& i);

  inline void read(bool& b,std::istream& i);
  inline void write(std::ostream& o,bool& b);

  //----ReadPolicy Customization
  
   
  template<typename T,typename A> struct ReadPolicy<std::vector<T,A> >{
    static const int value=READIT|READE;
  };

  template<typename T,typename A> struct ReadPolicy<std::list<T,A> >{
    static const int value=READIT|READE;
  };

  template<typename T,typename A> struct ReadPolicy<Array<T,array::RESIZABLE,A> >{
    static const int value=READN|READR;
  };

  template<typename T,int N,typename A> struct ReadPolicy<Array<T,N,A> >{
    static const int value=READN;
  };

  //----WritePolicy Customization

  
  
  template<typename T,typename A> struct WritePolicy<std::vector<T,A> >{
    static const int value=WRITEIT;
  };

  template<typename T,typename A> struct WritePolicy<std::list<T,A> >{
    static const int value=WRITEIT;
  };

  template<typename T,typename A> struct WritePolicy<std::set<T,A> >{
    static const int value=WRITEIT;
  };
  

  template<typename T,int N,typename A> struct WritePolicy<Array<T,N,A> >{
    static const int value=WRITEN;
  };



  
  //---- derived functions

  
  template <typename T> inline void readEn(T &t,std::istream &i);

  //--readWhole

  template <typename T> inline void readWhole(T &t,std::istream &i){
    read(t,i);
  }

 inline void readWhole(std::string &t,std::stringstream &i){
    t=i.str();    
  }

  inline void readWhole(std::string& s,std::istream& i){
    std::stringstream ss;
    ss<<i.rdbuf();
    s=ss.str();
  }


  /*template <typename F> inline void readWhole(std::pair<F,std::string> &p,std::istream &i){
    read(p.first,i);
    readWhole(p.second,i);
    }*/

  /*template <typename S> inline void readWhole(std::pair<std::string,S> &p,std::istream &i){
    readWhole(p.first,i);
    read(p.second,i);
    }*/

  template <typename F,typename S> inline void readWhole(std::pair<F,S> &p,std::istream &i){
    read(p.first,i);
    readWhole(p.second,i);
  }


  template<typename T>
  inline void readWholeShws(T &t,std::istream &i){
    read(t,i);
  }

  //skip head white spaces
   inline void readWholeShws(std::string &s,std::istream &i){
    readWhole(s,i>>std::ws);
  }

  template <typename F,typename S> inline void readWholeShws(std::pair<F,S> &p,std::istream &i){
    read(p.first,i);
    readWholeShws(p.second,i);
  }
    
  


  //--convert
  template <typename T,typename F> inline void convert(T &t,const F& f){
    std::stringstream ss;
    write(ss,f);    
    readWhole(t,ss);
  }
  
  template <typename T,typename F> inline void convert(T &t,const F& f,const std::string& msg){
    std::stringstream ss;
    try{
      write(ss,f);    
      readWhole(t,ss);
    }catch(exc::InvalidDataValue& v){
      throw exc::InvalidDataValue(v,msg);
    }
  }

  template<typename T,typename F> inline T convert(const F& f){
    T t;
    std::stringstream ss;
    write(ss,f);    
    readWhole(t,ss);
    return t;
  }


  template<typename T>
  inline void readAutoHex(T& r,std::istream& in){
    int x;
    std::string s;
    in>>s;
    bool isHex=(s[1]=='x');
    if(sscanf(s.c_str(),isHex?"%x":"%d",&x)<1){
      throw exc::InvalidDataValue(r,s+" interpred as "+(isHex?"hex":"dec"));
    }
    r=x;
  }

  
  class AutoHex{
    std::istream& i;
  public:
    AutoHex(std::istream& ip):i(ip){}
    template<typename T>
    std::istream& operator>>(T& r){
      readAutoHex(r,i);
      return i;
    }    
  };

  enum AutoHexType{

    autohex
  };


  inline AutoHex operator>>(std::istream& i,AutoHexType){
    return AutoHex(i);
  }


  template<typename T,bool singleByte=sizeof(T)==1>
  class HexPassThroughInt;

  template<typename T>
  class HexPassThroughInt<T,true>
  {
  public:
    static void read(T& x,std::istream& i){
      int p;
      i>>std::hex>>p>>std::dec;
      x=p;
    }
    static void write(std::ostream& o, const T& x){
      o<<std::hex<<(int)x<<std::dec;
    }
    
  };

    template<typename T>
    class HexPassThroughInt<T,false>
    {
    public:
      static void read(T& x,std::istream& i){
	i>>std::hex>>x>>std::dec;
      }

      static void write(std::ostream& o, const T& x){
	o<<std::hex<<x<<std::dec;
      }

    };


  

  template<typename T>
  inline static void readHex(T& x,std::istream& i){
    HexPassThroughInt<T>::read(x,i);
    if (i.fail()){
      std::stringstream ss;
      ss<<i.rdbuf();
      throw exc::InvalidDataValue(x,ss.str());
    }    
  }

  template<typename T>
  inline static void writeHex(std::ostream& o, T& x){
    HexPassThroughInt<T>::write(o,x);
  }



  template<typename T,typename F>
  inline static void convertHex(T& x,const F& f){
    std::stringstream ss;
    write(ss,f);
    readHex(x,ss);
  }
  

  template<typename T,typename F>
  inline static T convertHex(const F& f){
    T x;
    std::stringstream ss;
    write(ss,f);
    readHex(x,ss);
    return x;
  }

  //--toString


  template<typename T> void fromString(T& t,const std::string&s){
    std::stringstream ss;
    ss<<s;
    read(t,ss);
  }


  template<typename T>
  inline std::string toString(const T& t){
    std::stringstream ss;
    write(ss,t);
    return ss.str();
  }




  

 
  class stringBegin{
    std::stringstream ss;     
    public:   
    std::ostream& get_ostream(){
      return ss;
    }

    template<typename T>
    stringBegin& operator<<(const T& t){
      write(ss,t);
      return *this;
    }


    

    
    std::string operator<<(const stringBegin* (*f)(const stringBegin*)){
      return ss.str();
    }

     std::string operator<<(const stringBegin*){
      return ss.str();
    }
    
  };

  inline const stringBegin* stringEnd(const stringBegin* s=NULL){return NULL;}

  




  /*  struct Stringify{};
  void asString(Stringify&);


  std::string operator<<(std::stringstream& ss,const void (*f)(Stringify&)){
    return ss.str();
    NO, because stringstream returns an ostream


  */

  template<typename T>
  inline const std::string toHexString(const T& t){
    std::stringstream ss;
    ss<<std::hex<<t;
    return ss.str();
  }


  template<>
  inline const std::string toHexString(const uint8_t& t){
    std::stringstream ss;
    ss<<std::hex<<(uint16_t)t;
    return ss.str();
  }


  template<typename T>
  inline const std::string toHexString(const T& t,int padding){
    std::stringstream ss;
    ss<<std::setfill('0')<<std::setw(padding)<<std::hex<<t;
    return ss.str();
  }

  
  inline const std::string& toString(const std::string& s){
    return s;
  }

  


  //--size[Write/Read]


  template<typename T> inline void read(T& t,std::istream& in,int n){
    for (int i=0;i<n;i++){
      read(t[i],in);
    }
  }

  template<typename T> inline void write(std::ostream& o,T& t,int n){
    for (int i=0;i<n-1;i++){
      write(o,t[i])<<" ";
    }
    if (n>0){
      write(o,t[n-1]);
    }
  }


  template <typename T> inline void sizeWrite(std::ostream& o,const T& t){
    o<<t.size()<<" ";
    write(o,t);
  }

  template <typename T> inline void sizeWriten(std::ostream& o,const T& t,size_t n){
    o<<n;
    for (size_t i=0;i<n;i++){     
      o<<" ";
      write(o,t[i]);
    }
  }

  template <typename T> inline void sizeRead(T& t,std::istream& i){
    size_t n;
    i>>n;
    t.resize(n);
    read(t,i,n);
  }
  
 

  


  template<typename K,typename T,typename C,typename A>
  std::string keysToString(std::map<K,T,C,A>& m){
    std::stringstream ss;
    for (typename std::map<K,T,C,A>::iterator it=m.begin();
	 it!=m.end();it++){
      ss<<" "<<it->first;
    }
    return ss.str();
  }



inline void tokenize(std::vector<std::string>& tokens,
		     const std::string& str,	      
		     const std::string& delimiters=" \n\r")
{

  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos); 	
  while (std::string::npos != pos || std::string::npos != lastPos)
    {      
      tokens.push_back(str.substr(lastPos, pos - lastPos));      
      lastPos = str.find_first_not_of(delimiters, pos);
      pos = str.find_first_of(delimiters, lastPos);
    }
}//tokenize


  inline void tokenizeLongDelimiter(std::vector<std::string>& tokens,
		     const std::string& str,	      
		     const std::string& delimiters)
{
  //std::cout<<"to tokenize: {"<<str<<"}"<<std::endl;
  std::string::size_type lastPos;
  for (lastPos=0;
       str.substr(lastPos,delimiters.length())==delimiters;
       lastPos=lastPos+delimiters.length()){
    //std::cout<<"lastPos1="<<lastPos<<std::endl;
  }  
  //std::cout<<"lastPos2="<<lastPos<<std::endl;
  std::string::size_type pos     = str.find(delimiters, lastPos); 	
  while ( lastPos<str.length())
    {      
      tokens.push_back(str.substr(lastPos, pos - lastPos));      
      //std::cout<<"pushed token"<<tokens.back()<<std::endl;
      for (lastPos=pos;
	   pos!=std::string::npos&& lastPos<str.length()&&
       str.substr(lastPos,delimiters.length())==delimiters;
	   lastPos=lastPos+delimiters.length()){
	//std::cout<<"lastPos4="<<lastPos<<std::endl;
      }  
      pos = str.find(delimiters, lastPos);
      //std::cout<<"lastPos3="<<lastPos<<std::endl;
    }

}//tokenize



  template<typename T>
  inline void tokenizeInPlace(T& tokens,
			      const std::string& str,	      
			      const std::string& delimiters=" \n\r")
  {
    
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos); 	
    int c=0;
    while (std::string::npos != pos || std::string::npos != lastPos)
    {      
      convert(tokens[c++],str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delimiters, pos);
      pos = str.find_first_of(delimiters, lastPos);
    }
  }//tokenize




   template <typename T> inline std::string ptoString(const T* t){
   if (t==NULL)
     return "NULL";   
  std::stringstream ss;
  ss<<*t;
  return ss.str();
 }//toString



  //--stream associations


  template<typename T,int N,typename A> 
  inline std::ostream& operator<<(std::ostream& o,const Array<T,N,A>& a){
    write(o,a);
    return o;
  }

  template<typename T,int N,typename A> 
  std::istream& operator>>(std::istream& i,Array<T,N,A>& a){
    read(a,i);
    return i;    
  }
  
  namespace en{
    
    enum IoAssume{ASSUMEUNKNOWN,ASSUMEENUM,ASSUMEPTR};
    
    template<IoAssume ia> struct Enio{};


    inline Enio<ASSUMEUNKNOWN>* io(){return NULL;}
    inline Enio<ASSUMEENUM>* eio(){return NULL;}
    inline Enio<ASSUMEPTR>* ptrio(){return NULL;}
  }
  //--print

  
  template<typename T> std::ostream& print(const T& t,std::ostream& o=std::cout){
    write(o,t);
    return o;
  }
  
  
  template <typename T> std::ostream& print(const T* d,int m,int n,std::ostream& out=std::cout){
    for (int i=0;i<m;i++){
      for (int j=0;j<n;j++){
	write(out,d[i*n+j]);
	out<<" ";
      }
      out<<std::endl;
    }
    return out;
  }
  
  template <typename T> std::ostream& print(const T* d,int n,std::ostream& out=std::cout){
    for (int i=0;i<n;i++){
      out<<" ";
      write(out,d[i]);    
    }
    return out;
  }


template <size_t n,typename T> std::ostream& printn(const T* d,std::ostream& out=std::cout){
  for (size_t i=0;i<n;i++){
      out<<" ";
      write(out,d[i]);    
    }
    return out;
  }

  
  inline std::ostream& print(const double* m,std::ostream& out=std::cout){
    for (int i=0;i<4;i++){
      for (int j=0;j<4;j++)
	out<<m[i*4+j]<<" ";    
      out<<std::endl;
    }
    return out;
  }




  template <typename A> std::ostream & print(const std::string pre,const A &a,const std::string post,int n,std::ostream &o=std::cout){    
    for (int i=0;i<n;i++){      
      o<<pre<<a[i]<<post;
    }
    return o;
  }

  template <typename A> std::ostream & print(const std::string pre,const A &a,const std::string post,std::ostream & o=std::cout){
    return print(pre,a,post,(int)a.size(),o);
  }

  template <typename A> std::ostream & print(const std::string middle,const A &a,int n,std::ostream &o=std::cout){    
    for (int i=0;i<n-1;i++){      
      write(o,a[i]);
      o<<middle;
    }
    if (n>0) write(o,a[n-1]);
    return o;
  }

  template <typename A> std::ostream & print(const std::string middle,const A &a,int f,int t,std::ostream &o=std::cout){    
    for (int i=f;i<t-1;i++){      
      write(o,a[i]);
      o<<middle;
    }
    if (t-f>0) write(o,a[t-1]);
    return o;
  }
    

  template <typename A> std::ostream & print(const std::string middle,const A &a,std::ostream & o=std::cout){
    return print(middle,a,(int)a.size(),o);
  }



}//helium

#include <helium/util/ioUtil.hpp>


#endif
