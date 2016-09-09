/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.08.02
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_IDTREE
#define HE_IDTREE

#include <vector>
#include <sstream>
#include <helium/util/stringUtil.h>
#include <helium/util/ioUtil.h>
#include <helium/core/exception.h>
//#include <helium/vector/io.h>
#include <helium/vector/operations.h>
#include <helium/system/cellProtocol.h>


namespace helium{
  
  class GenericIDPath;
  class IDPath{
  protected:
    friend class GenericIDPath;
    std::vector<std::string> path;
    friend std::ostream & operator<<(std::ostream& o,const IDPath &p);
    friend std::istream & operator>>(std::istream& i,IDPath &p);
  public:
    const std::vector<std::string>& getPath()const{
      return path;
    }
    
    IDPath(){}
    IDPath(const std::vector<std::string> &ppath):path(ppath){}
    IDPath(const std::string& s,const std::string &separator);
    IDPath(const std::string& s);
    IDPath(const char* s);
    
    IDPath& downPath(const IDPath &p); 
    IDPath& downPath(const GenericIDPath &p);
    IDPath& downPath(const std::string &s);
    IDPath& downPath(const char* s);
    IDPath& upPath();
    IDPath& root();   
    IDPath& upPath(int n);
    
    size_t size() const;  
    bool isEmpty() const;
    
    bool operator <(const IDPath& b) const;
    bool operator ==(const IDPath& b) const;
    bool operator !=(const IDPath& b) const;    

    IDPath operator+(const GenericIDPath &p) const;

    
    const std::string & operator[](int i) const;
  };
  
  std::ostream & operator<<(std::ostream& o,const IDPath &p);  
  std::istream & operator>>(std::istream& i,IDPath &p);
  
  
  class GenericIDPath:public IDPath{
  private:
    friend class IDPath;
    int ups;
    
 public:
    enum uptype{NORMAL=0,ROOT=-1};

    GenericIDPath();
    GenericIDPath(const IDPath &p,int pups=NORMAL);
    template <typename T> GenericIDPath(const T &t);    
    GenericIDPath(const GenericIDPath& b);     
    bool isEmpty();//a "go-up" path is not empty,cannot use the base class isEmpty  
 };//GenericIdPath
  
 

template<typename T > class IDNode;
template<typename T> std::ostream& operator<< (std::ostream & os, const IDNode<T> &t);

class Match{
 public:
  int minmatch;
  std::string s;
 Match():minmatch(-1){
  }
 Match(const std::string &ps):minmatch(-1),s(ps){
  }
};


 inline std::ostream& operator<<(std::ostream & o, const Match &t){
   return o<<t.s;//<<"["<<t.minmatch<<"]";
 }

  template <typename T> void printAddress(std::ostream& o,const T* t){
    o<<t;
  }

template <typename T> class IDNode{
  
  std::vector<Match> aliases;
  bool transparent;
  T* data;
  IDNode<T>* parent;
  std::vector<IDNode<T>*> sons;

 public:
  IDNode(IDNode<T>* pparent=NULL);
  
  IDNode* getParent() const;
  const std::vector<IDNode<T>*> & getChildren() const;
  IDNode<T>* getChild(int i) const;
  size_t childrenNum() const;
  const  std::vector<Match>& getAliasesMatches() const;
  void getMainAlias(std::string &s) const;  
  const std::string& getMainAlias() const;
  const std::string& getAlias(int i) const;
  virtual ~IDNode(); 
  IDNode* clone(IDNode* pparent=NULL);
  void getLocalPath(IDPath &p) const;
  void merge(IDNode* b);
  void destroyLinkedData(const std::vector<const T*> &toKeep=std::vector<T*>());
  bool hasData();
  T* getDataP();
  const T* getDataP() const;
  IDNode* setData(T* t);
  void addPartialMatches(std::vector<std::string> &matches,const std::string &s);
  void getAliases(std::vector<std::string> &alias);
  IDNode* canEnter(size_t &a,const std::string &s);


  IDNode* downHop(size_t& a,const std::string &s) const;  
  IDNode* downSurePath(const IDPath &p);
  IDNode* downMaximumPath(size_t& matched,const IDPath &p);
  IDNode* downPath(const IDPath &p) const;

  bool hasPath(const IDPath &p);
  IDNode* downMaximumPathNull(size_t& matched,const IDPath &p) const;
  void machinePrint(std::ostream& o,void (*printer)(std::ostream& o,const T* t)=printAddress<T>);
  IDNode* addChild(const std::string &s);
  void fixSonsMatches(int i);
  void unlinkChild(IDNode* n);
  bool hasExactAlias(const std::string& s);  
  IDNode* assurePath(const IDPath &p);
  bool isTransparent() const;
  void setTransparent(bool state=true);
  void addAlias(const std::string &a);
   T& getData() const;  
  IDNode* tracedownPath(std::vector<std::string> &matches, const IDPath &p);

  void print(std::ostream &o,int spaces=0) const;

  void getMatches(std::vector<std::string> &matches, const IDPath &p,const IDNode* m) const;

 private:
  void addPossibilities(std::vector<std::string> &tot, const std::vector<std::string> &what,bool transparent) const;
  void examineProblem(const IDNode* m,const IDPath &p,std::vector<std::string> &matches) const;
  

   
};


template <typename T> inline  std::ostream& operator<< (std::ostream & o, const helium::IDNode<T> &t){
  t.print(o);
   return o;
 }




}//helium

#include "idTree.hpp"

#endif
