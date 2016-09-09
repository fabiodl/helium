/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_HELPCELL
#define HE_HELPCELL
#include <helium/system/cellProtocol.h>
#include <helium/system/basicCell.h>
#include <helium/system/cellDescription.h>

#include <helium/os/time.h>

namespace helium{

  class TreeHelp{

    friend class DefDescr<TreeHelp>;
    
    class HelpCell:public TypedCell<std::string,Void,
				    Void,Void,
				    Void,Void> {
      std::string getName() const;
      void getData(std::string& s);
    };//HelpCell


   
    
    class TreeCell:public TypedCell<std::string,std::string, //get a subtree
				     Void,Void,
				     Void,Void>{
      IDNode<Cell>& tree;
      
      std::string getName() const;
      
      void print(std::ostream& o,const IDNode<Cell>* n,int spaces=0);
      
      void print(std::ostream& o);

      void getData(std::string& s);

      void getData(std::string& s,const std::string& p);


      class DumpSignature:public CellSignature{
      public:
	typedef Void GT;
	typedef Void GP;
	typedef std::string ST;
	typedef Void SP;
	typedef Void UT;
	typedef Void UP;
	
	static const bool getRequiresParam=false;
	static const bool setRequiresParam=false;
	static const bool connectRequiresParam=false;
	
      };


     
     
      
      
    public:

      class Dump:public TypedCell<DumpSignature>{
	TreeCell& owner;	
	std::string getName() const;
	void setData(const std::string& s);	
      public:
	Dump(TreeCell& powner);
      };
      
      TreeCell(IDNode<Cell>& ptree);

    };


    
    HelpCell help;
    TreeCell treeCell;
    TreeCell::Dump dump;
  public:
    TreeHelp(IDNode<Cell>& ptree);
  };//TreeHelp

  template<>
  class DefDescr<TreeHelp>:public Description<TreeHelp>{
  public:
    DefDescr<TreeHelp>(TreeHelp& th):Description<TreeHelp>(th){      
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(obj.help,"help"));
      members.push_back(scalarMapping(obj.treeCell,IDPath("tree",".")));
      members.push_back(scalarMapping(obj.dump,IDPath("tree.dump",".")));
    }

  };

}



#endif

