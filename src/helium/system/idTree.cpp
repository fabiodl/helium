/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.08.02
 * \version 0.1
 * Release_flags s
 */

#include <helium/system/idTree.h>

namespace helium{


  bool IDPath::operator <(const IDPath& b) const{
    for (size_t i=0;i<(std::min)(path.size(),b.path.size());i++){
      if (path[i]<b.path[i]) return true;
      if (path[i]>b.path[i]) return false;
    }
    if (path.size()<b.path.size()) return true;
    return false;
  }
  
  bool IDPath::operator ==(const IDPath& b) const{
    return equals(path,b.path);
  }

  bool IDPath::operator !=(const IDPath& b) const{
    return !equals(path,b.path);
  }

  
  IDPath::IDPath(const std::string& s,const std::string &separator){
    tokenize(path,s,separator);
  }
  
  IDPath::IDPath(const std::string& s){
    downPath(s);
  }

  IDPath::IDPath(const char* s){
    downPath(s);
  }
  
  IDPath& IDPath::downPath(const IDPath &p){
    path.insert(path.end(),p.path.begin(),p.path.end());
    return *this;
  }


  IDPath&  IDPath::downPath(const std::string &s){
     if (s.length()==0) return *this;
     path.push_back(s);
     return *this;     
   }
   IDPath& IDPath::downPath(const char* s){
     downPath(std::string(s));
     return *this;
   }   

  IDPath& IDPath::downPath(const GenericIDPath &p){   
   if (p.ups==GenericIDPath::ROOT){
     path.clear();
   }else{
     upPath(p.ups);
   }
   path.insert(path.end(),p.path.begin(),p.path.end());
   return *this;
  }


   IDPath& IDPath::upPath(){
     path.pop_back();
     return *this;
   }

   IDPath& IDPath::root(){
     path.clear();
     return *this;
   }
   
   IDPath& IDPath::upPath(int n){
     for (int i=0;i<n;i++) upPath();
     return *this;
   }

  size_t IDPath::size() const{
    return path.size();
  }   
  
  bool IDPath::isEmpty() const{
    return path.size()==0;
  }
  
  const std::string & IDPath::operator[](int i) const{
    return path[i];
  }

 
std::ostream & operator<<(std::ostream& o,const IDPath &p){
  return print(".",p.path,o);
}


std::istream & operator>>(std::istream& i,IDPath &p){
  std::string s;
  i>>s;
  tokenize(p.path,s,".");
  return i;
}


GenericIDPath::GenericIDPath(const IDPath &p,int pups):ups(pups){    
  path.insert(path.end(),p.path.begin(),p.path.end());
}

GenericIDPath::GenericIDPath(const GenericIDPath& b){
  path.clear();
  path=b.path;
  ups=b.ups;
}

GenericIDPath::GenericIDPath():ups(NORMAL){    
}  


bool GenericIDPath::isEmpty(){
  return (ups==0&&path.size()==0);
}


  IDPath IDPath::operator+(const GenericIDPath &p) const{
    IDPath r(*this);
    return r.downPath(p);
  }



}



