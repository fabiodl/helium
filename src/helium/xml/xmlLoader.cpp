/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include "xmlLoader.h"
#include <tinyxml/tinyxml.h>
#include <helium/system/basicCell.h>
#include <helium/util/mout.h>
#include <tinyxml/tinyxml.h>
namespace helium{

 
  
  class XmlFile{
    TiXmlDocument doc;
  public:
    
    TiXmlElement* root;
    
    XmlFile(const std::string& filename):doc(filename.c_str()){
      doc.LoadFile();
      if (doc.Error()) throw exc::XMLError("while loading "+filename+":"+doc.ErrorDesc());  
      root = doc.RootElement();    
    }
    
  };


  


  
  /*XmlLoader::XmlLoader(const std::string &filename):loadingName(filename){	     
    
    TiXmlDocument doc(filename.c_str());
    doc.LoadFile();
    if (doc.Error()) throw exc::XMLError("while loading "+filename+":"+doc.ErrorDesc());    
    TiXmlElement* root = doc.RootElement();    
    loadData(&local,&thisModel,root,local);
    
    }*/

  


  /*void XmlLoader::exportRef(Memory& mem,const IDPath &path){
    if (local.hasPath("_")){
      mem.merge(local.downPath("_"));
    }else{
      mem.merge(&local);
    }
    }*/

  /*XmlLoader::XmlLoader(Memory& mem,const IDPath &path){
    import(mem,path);
    }*/
  
  /*void XmlLoader::import(Memory& mem,const IDPath &path){
    local.merge(&mem);
    }*/
  
  void setNullData(Memory& mem,IDNode<Cell>* n,const std::string& s){
    if (n->getDataP()!=NULL){
      IDPath p;
      n->getLocalPath(p);
      std::stringstream ss;
      n->getData().streamGet(ss);
      if (ss.str()!=s){
	mout(msg::WARNING)(stringBegin()<<"ignoring config value for "<<p<<" because already set to "<<stringEnd());
      }
      
    }else{
      CellHolder * c=new CellHolder(new SerializedDataCell(s));   
      //std::cout<<"seri cell"<<*c<<std::endl;
      n->setData(c);          
      mem.addForDeletion(c);
    }
    
  }
  static const XmlPathType Element=ELEMENT;
  static const XmlPathType Attribute=ATTRIBUTE;
  static const XmlPathType Text=TEXT;



  template<bool D,bool M>
  void XmlLoader::gloadData(IDNode<Cell>* n,ModelNode* m,TiXmlElement * e,Memory* mem){

    if (e->Value()!=transparent){
    if (D){
      n=n->assurePath(e->Value());    
    }
    if (M){
      m=m->assurePath(e->Value());
    }
    const char* id=e->Attribute("id");
    if (id!=NULL){
      if (D){
	n->setTransparent(true);
	n=n->assurePath(id);
      }
      if (M){
	m=m->assurePath(id);
      }
      //n->setData(IDCELL());
    }    

    if (D){
    
    const char* name=e->Attribute("name");
    if (name!=NULL){
      if (id!=NULL){
	//std::cout << "a <- " << name << std::endl;
	n->addAlias(name);	
      }else{
	//n=n->assurePath(name);
	//	m=m->assurePath(name);
      }    
    }//name!=null

    }


    const char* t=e->GetText();
    if (t!=NULL){ 
      if (D){
	setNullData(*mem,n,t);
      }
      if (M){
	//std::cout<<"--=got text=--"<<t<<std::endl;
	m->setData(&Text);
      }
    }
    for (TiXmlAttribute * a=e->FirstAttribute();a!=NULL;a=a->Next()){
      if (D){
	setNullData(*mem,n->assurePath(a->Name()),a->Value());
      }
      if (M){
	//std::cout<<"--=got attribute=--"<<a->Value()<<std::endl;
	m->assurePath(a->Name())->setData(&Attribute);
      }
    }
    if (M&&m->getDataP()==NULL){
      m->setData(&Element);
    }
    }//!transparent
    for (TiXmlElement * se=e->FirstChildElement();se!=NULL;se=se->NextSiblingElement()){
      gloadData<D,M>(n,m,se,mem);
    }        
  }
  

 




  /*XmlModel::XmlModel(const std::string& name){
    TiXmlDocument doc(name.c_str());
    doc.LoadFile();
    if (doc.Error()) throw exc::XMLError(doc.ErrorDesc());
    TiXmlElement* root = doc.RootElement();
    loadModel(this,root);
    }*/

  

  /*  XmlLoader::PathType XmlLoader::Model::getType(IDPath& path,Cell* c){  
    size_t oks;
    IDNode<PathType>* t=downMaximumPath(oks,path);
    if (oks==path.size()){
      return t->getData(); 
    }else{
      if (c==NULL) return ELEMENT;
      return ATTRIBUTE;      
    }
    
    };*/



  bool isArrayContainer(const IDNode<Cell>* n){    
    if (!n->childrenNum()) return false;
    for (int i=0;i<(int)n->childrenNum();i++){
      if (!startsByDigit(n->getChild(i)->getMainAlias())) return false;            
    }
    return true;
  }















  void saveData(const IDNode<Cell>* n,const ModelNode* m,TiXmlNode * e,bool isElement){
    std::string name;
    n->getMainAlias(name);
    //std::cout<<"getting cell of"<<name<<"@ "<<n<<std::endl;
    const Cell* c=n->getDataP();
    //std::cout<<"cell should be"<<c<<std::endl;
    std::stringstream data;
    if (c!=NULL){
      //std::stringstream ss;
      //      const_cast<Cell*>(c)->getData(ss);
      //std::cout<<ss.str()<<std::endl;
      const_cast<Cell*>(c)->streamGet(data);
    }

    
    if (startsByDigit(name)){
    
      e=e->LinkEndChild(new TiXmlElement(n->getParent()->getMainAlias().c_str())); 
      e->ToElement()->SetAttribute("id",name.c_str());
      if (c!=NULL){
	e->ToElement()->SetAttribute("value",data.str().c_str());
      }
    }else{	
      XmlPathType type;      
      if (m!=NULL&&m->getDataP()!=NULL){
	type=m->getData();
      }else{
	//std::cout<<"no model data,resort to defaults"<<std::endl;
	type=ELEMENT;
	if (isArrayContainer(n)){
	  type=NONE;
	}else if (isElement&&e->ToElement()->Attribute(name.c_str())==NULL&&c!=NULL){
	  type=ATTRIBUTE;
	}
	if (type==ELEMENT&&c!=NULL){
	  type=TEXT;
	}
      }

      //std::cout<<name<<" is "<<type<<std::endl;
      switch(type){
      case ELEMENT:
	e=e->LinkEndChild(new TiXmlElement(name.c_str()));
	break;
      case TEXT:{
	  TiXmlElement* ne=new TiXmlElement(name.c_str());
	  e=e->LinkEndChild(ne);
	  ne->LinkEndChild(new TiXmlText(data.str().c_str()));
      }
	  break;
      case ATTRIBUTE:
	//std::cout<<"attribute vals is"<<data<<std::endl;
	e->ToElement()->SetAttribute(name.c_str(),data.str().c_str());
	break;
      case NONE:
	//std::cout<<name<<" is ignored"<<std::endl;
	break;
      }
      
    }//!startsByDigit

    
    for (int i=0;i<(int)n->childrenNum();i++){
      size_t son;
      saveData(n->getChild(i),m==NULL?NULL:m->downHop(son,n->getChild(i)->getMainAlias()),e,true);            
    }
    
  }//saveData



  //loadData(IDNode<Cell>* n,ModelNode* m,TiXmlElement * e,Memory* mem)
  void XmlLoader::loadDataAndModel(Memory& mem,const std::string &filename){
    XmlFile in(filename);
    gloadData<true,true>(&mem,&thisModel,in.root,&mem);
  }

  void XmlLoader::loadData(Memory& mem,const std::string &filename){
    XmlFile in(filename);
    gloadData<true,false>(&mem,NULL,in.root,&mem);
  }

  void XmlLoader::loadModel(ModelNode& model,const std::string &filename){
    XmlFile in(filename);
    gloadData<false,true>(NULL,&model,in.root,NULL);
  }

  void XmlLoader::loadModel(const std::string &filename){
    loadModel(thisModel,filename);
  }


  void XmlLoader::save(const char* filename,Memory& mem,const ModelNode* m){
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );        
    if (doc.Error()) throw exc::XMLError(doc.ErrorDesc());       
    TiXmlNode * head=&doc;

    if (mem.childrenNum()!=1){
      mout(msg::WARNING)("saving an xml with multiple root elements");
      head=head->LinkEndChild(new TiXmlElement("_"));
      //throw exc::InvalidOperation("saving an xml with multiple root elements");
    }

    //std::cout<<thisModel<<std::endl;
    size_t son;
    if (m==NULL){
      m=&thisModel;
    }    
    saveData(mem.getChild(0),m->downHop(son,mem.getChild(0)->getMainAlias()),head,false);  
    doc.SaveFile(filename);
  }//save


  /*void XmlLoader::exportData(Memory& mem,const IDPath &path){    
    TiXmlDocument doc(name.c_str());
    doc.LoadFile();
    if (doc.Error()) throw exc::XMLError(doc.ErrorDesc());    
    TiXmlElement* root = doc.RootElement();
    IDNode<Cell>* n=mem.downPath(path);
    loadData(n,root);
  
    }*/

  
  XmlLoader::XmlLoader(const std::string& ptransparent):transparent(ptransparent){
    
  }



}//helium
