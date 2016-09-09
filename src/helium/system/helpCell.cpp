/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/system/helpCell.h>
#include <fstream>
namespace helium{

  

  std::string TreeHelp::HelpCell::getName() const{
    return "help";	
  }

  void TreeHelp::HelpCell::getData(std::string& s){
	std::stringstream ss;
	static const char* meaning[MESSAGETYPES]={"get","get w/ param",
						  "set","get w/ param",
						  "connect", "connect w/ param",
						  "disconnect","disconnect w/ param",
						  "update","error","welcome"};
	
	ss<<"operators:"<<std::endl;
	ss<<" request  reply meaning"<<std::endl;
	for(int i=0;i<MESSAGETYPES;i++){
	  ss<<"    "<<requestChar[i]<<"       "<<replyChar[i]<<"   "<<meaning[i]<<std::endl;
	}	 
	s=ss.str();
  }      
  
  
   std::string  TreeHelp::TreeCell::getName() const{
     return "human readable description";	
   }

  void TreeHelp::TreeCell::print(std::ostream& o,const IDNode<Cell>* n,int spaces){
	/*std::cout<<getSystemMillis();	
	  space(spaces,std::cout);
	  if (spaces>0) std::cout<<n->getMainAlias()<<std::endl;
	*/

	space(spaces,o);
	o<<(n->isTransparent()?"(":"[");
	helium::print("|",n->getAliasesMatches(),o);    
	o<<(n->isTransparent()?")":"]");
	o<<"=";
	const Cell* c=n->getDataP();
	if (c==NULL){
	  o<<"[No cell]";
	}else if (c!=this){ //do not recursevely print!
	  try{
	    if (c->getProperties()==NULL|| //try if unknown
		c->getProperties()->getCapabilities()&CELLGETS){
	      const_cast<Cell*>(c)->print(o);
	    }else{
	      o<<"[Unprintable]";
	    }
	  }catch(exc::Exception&){
	    //std::cout<<"catched"<<std::endl;
	    o<<"[!]";
	  }
	}   
	o<<std::endl;
	for (size_t i=0;i<n->childrenNum();i++){
	  print(o,n->getChild(i),spaces+1);
	}
      }


  void TreeHelp::TreeCell::print(std::ostream& o){
    print(o,&tree);
  }
  
  void TreeHelp::TreeCell::getData(std::string& s){
	std::stringstream ss;
	//std::cout<<"Requested tree"<<std::endl;
	print(ss);
	s=ss.str();
      }


  void TreeHelp::TreeCell::getData(std::string& s,const std::string& p){
	IDPath path(p,".");
	std::stringstream ss;
	print(ss,tree.downPath(path));
	s=ss.str();
      }

  

  std::string  TreeHelp::TreeCell::Dump::getName() const{
    return "tree dumper to file";	
  }
  
  void TreeHelp::TreeCell::Dump::setData(const std::string& s){
    std::fstream f(s.c_str(),std::ostream::out);
    owner.print(f);
  }

  TreeHelp::TreeCell::Dump::Dump(TreeCell& powner):owner(powner){
	}
  
  TreeHelp::TreeCell::TreeCell(IDNode<Cell>& ptree):tree(ptree)
  {
  }

  TreeHelp::TreeHelp(IDNode<Cell>& ptree):treeCell(ptree),
					  dump(treeCell){}
  
}
