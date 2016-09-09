/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.2
 * Release_flags s
 */

  //---- Vect Reading implementation
namespace helium{
  template<DataIoType daio>
  struct Writer;
  
  template<DataIoType rt>
  struct Reader;






  template <typename T> inline void vnRead(T& v,std::istream& in){ //replace elements,numeric
    for (size_t i=0;i<v.size();i++){
      read(v[i],in);
    }
  }

  template <typename T> inline void vitRead(T& v,std::istream& in){ //replace elements, iterator
    
    for (typename T::iterator it=v.begin();it!=v.end();++it){
      read(*it,in);
    }    
  }

  template<typename T> inline void veRead(T& v,std::istream& in){ //add elements 
    typename T::value_type x;
    while(!in.eof()){
      read(x,in);
      if (in){
	v.insert(v.end(),x);

      }
    }
  }


  template<typename T> inline void vrRead(T& v,std::istream& in){ //add elements, resizing
    int c=0;
    int capacity=1;
    v.resize(capacity);   
    while(in){
      if (c>=capacity){
	capacity*=2;
	v.resize(capacity);
      }
      //std::cout<<"read element"<<c<<" "<<v[c]<<std::endl;
      read(v[c],in);
      c++;	
      if (in.eof()){
	break;
      }
    }
    v.resize(c);
    //std::cout<<"had "<<c<<"elements "<<std::endl;
  }



  template<int N> struct VectReader;

  template<> struct VectReader<READN>
  {
    template<typename T>
    inline static void read(T& v,std::istream& in){
      vnRead(v,in);
    }
  };

  template<> struct VectReader<READSN>
  {
    template<typename T>
    inline static void read(T& v,std::istream& in){
      size_t s;
      in>>s;
      v.resize(s);
      vnRead(v,in);
    }
  };

  template<> struct VectReader<READIT>
  {
    template<typename T>
    inline static void read(T& v,std::istream& in){
      vitRead(v,in);
    }
  };

  template<> struct VectReader<READE>
  {
    template<typename T>
    inline static void read(T& v,std::istream& in){
      veRead(v,in);
    }
  };


  template<> struct VectReader<READR>
  {
    template<typename T>
    inline static void read(T& v,std::istream& in){
      vrRead(v,in);
    }
  };


  template<> struct VectReader<READN|READE>
  {
    template<typename T>
    inline static void read(T& v,std::istream& in){
      if (v.size()){
	vnRead(v,in);
      }else{
	veRead(v,in);
      }
    }
  };
  

  template<> struct VectReader<READIT|READE>
  {
    template<typename T>
    inline static void read(T& v,std::istream& in){
      if (v.size()){
	vitRead(v,in);
      }else{
	veRead(v,in);
      }
    }
  };

  template<> struct VectReader<READN|READR>
  {
    template<typename T>
    inline static void read(T& v,std::istream& in){
      if (v.size()){
	vnRead(v,in);
      }else{
	vrRead(v,in);
      }
    }
  };



  template<> struct Reader<VECTIO>{
    template<typename T>
    inline static void read(T& t,std::istream& in){
      VectReader<ReadPolicy<T>::value>::read(t,in);
    }
  };
  
  //--Vect Writing implemenation


  template<int N> struct VectWriter;

  template<> struct VectWriter<WRITEN>{
    template<typename T>
    inline static void write(std::ostream& o,T& v){
      for (size_t i=0;i<v.size();i++){
	o<<" ";
	helium::write(o,v[i]);
      } //for   
    }//write
  };

  template<> struct VectWriter<WRITESN>{
    template<typename T>
    inline static void write(std::ostream& o,T& v){
      o<<v.size();
      for (size_t i=0;i<v.size();i++){
	o<<" ";
	helium::write(o,v[i]);
      } //for   
    }//write
  };
    
  template<> struct VectWriter<WRITEIT>{
    template<typename T>
    inline static void write(std::ostream& o,T& v){
      for (typename T::const_iterator i=v.begin();i!=v.end();++i){
	o<<" ";
	helium::write(o,*i);
      }
    }    
  };

  template<> struct Writer<VECTIO>{
    template<typename T>
    inline static void write(std::ostream& o,const T& t){
      VectWriter<WritePolicy<T>::value>::write(o,t);
    }
  };
  

    


  //---ENUM

  template<> struct Reader<ENUMIO>{
    template<typename T>
    inline static void read(T& t,std::istream& is){
      std::string s;
      is>>s;
      for (int i=0;i<EnumStrings<T>::NUM;i++){
	if (EnumStrings<T>::strings[i]==s){
	  t=(T)i;
	  return;
	}
      }

      std::stringstream ss;
      ss<<is.rdbuf();
      ss<<" options";
      for (int i=0;i<EnumStrings<T>::NUM;i++){
	ss<<" "<<EnumStrings<T>::strings[i];
      }
      throw exc::InvalidDataValue(t,ss.str());
    }
  };


  template<> struct Writer<ENUMIO>{
    template<typename T>
    inline static void write(std::ostream& o,const T& t){
      o<<EnumStrings<T>::strings[t];
    }
  };
  

  //--NENUM

  template<> struct Reader<NENUMIO>{
     template<typename T>
    inline static void read(T& t,std::istream& i){
      int x;
      i>>x;
      t=(T)x;
      if (i.fail()){
	std::stringstream ss;
	ss<<i.rdbuf();
	throw exc::InvalidDataValue(t,ss.str());
      }
    }
  };

  template<> struct Writer<NENUMIO>{
    template<typename T>
    inline static void write(std::ostream& o,const T& t){
      o<<t;
    }
  };


  //---SCALAR

  
  template<> struct Reader<SCALARIO>{
    template<typename T>
    inline static void read(T& t,std::istream& i){
      i>>t;
      if (i.fail()){
	std::stringstream ss;
	ss<<i.rdbuf();
	throw exc::InvalidDataValue(t,ss.str());
      }
    }
  };
  
  
  template<> struct Writer<SCALARIO>{
    template<typename T>
    inline static void write(std::ostream& o,const T& t){
      o<<t;
    }
  };
  
  

  //---main function

  template<typename T>
  inline void read(T& t,std::istream& i){
    Reader< 
      EnumStrings<T>::NUM==-1?NENUMIO:
      (EnumStrings<T>::NUM>0 ? ENUMIO:(ReadPolicy<T>::value==READSCALAR?SCALARIO:VECTIO))
      >::read(t,i);
  }




  template<typename T>
  inline void write(std::ostream& o,const T& t){
    Writer<
      EnumStrings<T>::NUM==-1?NENUMIO:
      (EnumStrings<T>::NUM>0?ENUMIO:(WritePolicy<T>::value==WRITESCALAR?SCALARIO:VECTIO))
	  >::write(o,t);
  }

  

  //---function customization



  //-pair

  template<typename F,typename S>
  inline void read(std::pair<F,S>& p, std::istream& i){
    read(p.first,i);
    read(p.second,i);
  }


  template<typename F,typename S>
  inline void write(std::ostream& o,const std::pair<F,S>& p){
    write(o,p.first);
    o<<" ";
    write(o,p.second);
  }

  //triple

  template<typename F,typename S,typename T>
  inline void read(triple<F,S,T>& p, std::istream& i){
    read(p.first,i);
    read(p.second,i);
    read(p.third,i);
  }


  template<typename F,typename S,typename T>
  inline void write(std::ostream& o,const triple<F,S,T>& p){
    write(o,p.first);
    o<<" ";
    write(o,p.second);
    o<<" ";
    write(o,p.third);
  }


  
  //-double

  inline void read(double& d,std::istream& i){
    i>>d;
    if (!i){      
      std::string s;
      i.clear();
      i>>s;
      if ((toupper(s[0])=='N'&&toupper(s[1])=='A'&&toupper(s[2])=='N') ||
	  (toupper(s[0])=='-'&&toupper(s[1])=='N'&&toupper(s[2])=='A'&&toupper(s[3])=='N')
	  ){

	d=NAN;

      }else{
	throw exc::InvalidDataValue(d,s);
      }
    }
  }
  
  inline void read(bool& b,std::istream& i){
    i>>b;
    if (!i){
      i.clear();
      i>>std::boolalpha>>b>>std::noboolalpha;
      if (!i){
	std::stringstream ss;
	ss<<i.rdbuf();
	throw exc::InvalidDataValue(b,ss.str());
      }
       
    }
  }

  inline void write(std::ostream& o,bool& b){
    o<<std::boolalpha<<b<<std::noboolalpha;
  }


  template <typename T> inline void readEn(T &t,std::istream &i){
    Reader<(EnumStrings<T>::NUM>0?ENUMIO:NENUMIO)>::read(t,i);
  }


  namespace en{
	template<IoAssume ia>
	struct Eno{
	  std::ostream& o;
	  Eno(std::ostream& po):o(po){}
	  template<typename T>
	  std::ostream& operator<<(const T& t);
	  template<typename T>
	  std::ostream& operator<<(const T* t);//for ASSUMPTR
	};


	template<IoAssume ia>
	struct EnoStringBegin:public Eno<ia>{
	  stringBegin& sb;

	  template<typename T>
	  stringBegin& operator<<(const T& t){
	Eno<ia>::operator<<(t);
	return sb;
	  }

	  EnoStringBegin(stringBegin& psb):Eno<ia>(psb.get_ostream()),sb(psb){
	  }
	};

	template<IoAssume ia>
	struct Eni{
	  std::istream& i;
	  Eni(std::istream& pi):i(pi){}
	  template<typename T>
	  std::istream& operator>>(T& t);
	};

	template<IoAssume ia>
	Eno<ia> operator<<(std::ostream& o,Enio<ia>* (*f)()){
	  return Eno<ia>(o);
	}


	template<IoAssume ia>
	EnoStringBegin<ia> operator<<(stringBegin& sb,Enio<ia>* (*f)()){
	  return EnoStringBegin<ia>(sb);
	}




	template<IoAssume ia>
	Eni<ia> operator>>(std::istream& i,Enio<ia>* (*f)()){
	  return Eni<ia>(i);
	}

	template<>
	template<typename T>
	std::ostream& Eno<ASSUMEUNKNOWN>::operator<<(const T& t){

	  write(o,t);
	  return o;
	}

	template<>
	template<typename T>
	std::istream& Eni<ASSUMEUNKNOWN>::operator>>(T& t){
	  read(t,i);
	  return i;
	}

	template<>
	template<typename T>
	std::ostream& Eno<ASSUMEENUM>::operator<<(const T& t){
	  //std::cout<<"this operator has size of strings "<<EnumStrings<T>::NUM<<std::endl;
	  Writer<(EnumStrings<T>::NUM>0?ENUMIO:NENUMIO)>::write(o,t);
	  return o;
	}

	template<>
	template<typename T>
	std::istream& Eni<ASSUMEENUM>::operator>>(T& t){
	  Reader<(EnumStrings<T>::NUM>0?ENUMIO:NENUMIO)>::read(t,i);
	  return i;
	}

	template<>
	template<typename T>
	std::ostream& Eno<ASSUMEPTR>::operator<<(const T* t){
	  if (t==NULL){
	o<<"NULL";
	  }else{
	write(o,*t);
	  }
	  return o;
	}




  }


}
