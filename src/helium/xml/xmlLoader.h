/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_XMLLOADER_
#define HE_XMLLOADER_
#include<string>
#include <helium/system/idTree.h>
#include <helium/system/cell.h>

class TiXmlElement;
namespace helium{
  
  
  enum XmlPathType{NONE,ELEMENT,ATTRIBUTE,TEXT};
  typedef IDNode<const XmlPathType> ModelNode;

  


  class XmlLoader{
 
    ModelNode thisModel;
    std::string transparent;
    template<bool D,bool M>
    void gloadData(IDNode<Cell>* n,ModelNode* m,TiXmlElement * e,Memory* mem);
  public:
    //XmlLoader(Memory& mem,const IDPath &path=IDPath());  
    //XmlLoader();
    
    XmlLoader(const std::string& transparent="_");


    void loadDataAndModel(Memory& mem,const std::string &filename);
    void loadData(Memory& mem,const std::string &filename);
    
    void loadModel(ModelNode& model,const std::string &filename);
    void loadModel(const std::string &filename);//loads to thisModel

    void save(const char* filename,Memory& mem,const ModelNode* m=NULL);
    //const std::string& getLoadingName(){return loadingName;}
    //void import(Memory& mem,const IDPath &path=IDPath());  
    //void exportRef(Memory& mem,const IDPath &path=IDPath());

    //void addCells(Memory& n);
  };
    
  }//helium

#endif
