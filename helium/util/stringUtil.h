/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_STRINGUTIL
#define HE_STRINGUTIL
#include <string>
#include <iostream>
#include <sstream>
namespace helium{

 

  inline void splitFilename (const std::string& str,std::string& folder,std::string& file) {
    size_t found;
    found=str.find_last_of("/\\");
    folder = str.substr(0,found);
    file = str.substr(found+1);
  }

  inline void getFolder(const std::string& str,std::string& folder) {
    std::string f;
    splitFilename(str,folder,f);
  }

  inline bool isHeadSubstring(const std::string &sub, const std::string &s){    
    for (size_t i=0;i<sub.size();i++){
      if (sub[i]!=s[i]) return false;
    }
    return true;
  }//headMatch


  inline int headMatches(const std::string &a, const std::string &b){   
    int i=0;
    for (;i<(int)(std::min)(a.size(),b.size());i++){
      if (a[i]!=b[i]) break;
    }
    return i;
  }




  inline void space(int spaces,std::ostream& o=std::cout){
    for (int i=0;i<spaces;i++){
      o<<" ";
    }
  }

  inline std::string spaces(int n){
    //std::cout<<"retrun spaces x"<<n<<std::endl;
    std::string s;
    s.resize(n,' ');
    return s;
  }

  inline std::string spaces(int n,char c){
    //std::cout<<"retrun spaces x"<<n<<std::endl;
    std::string s;
    s.resize(n,c);
    return s;
  }
  
  inline std::string padWithSpaces(const std::string& s,size_t n){    
    return s.length()>=n?s:s+spaces(n-s.length());
  }

  inline std::string rightStringAlign(const std::string& s,size_t n){    
    return s.length()>=n?s:spaces(n-s.length())+s;
  }

  
  inline bool isDigit(const std::string& s){
    int x;
    std::stringstream ss;
    ss<<s;
    ss>>x;
    return !ss.fail();
  }

  

  inline bool startsByDigit(const std::string& s){   
    return (s[0]>='0'&&s[0]<='9');
  }

};//helium

#endif
