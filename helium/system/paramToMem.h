/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_PARAMTOMEM
#define HE_PARAMTOMEM



#include <algorithm>
#include <helium/core/exception.h>
#include <helium/util/ioUtil.h>
#include <helium/system/basicCell.h>
#include <helium/system/cellProtocol.h>
#include <helium/robot/robotProtocol.h>


namespace helium{

  inline void dataValuePair(IDPath &key,     std::string& value, std::string& s){

    std::vector<std::string> tokens;
    tokenize(tokens,s,"=");
    if (tokens.size()!=2){
      tokens.clear();
      tokenize(tokens,s," ");
    }
    if (tokens.size()!=2){
      throw exc::SyntaxError(s+" command line arguments should have the form -param=value or -param value");
    }
    std::vector<std::string> pt;
    std::stringstream ss;
    ss<<tokens[0];
    std::string spacelessKey;
    ss>>spacelessKey;
    tokenize(pt,spacelessKey,PATHSEPARATORCHAR);      
    key.downPath(IDPath(pt));
    value=tokens[1];
  }




  inline void paramToMem(Memory& mem,int argc,char** argv,
		  const std::map<IDPath,std::string> &defaults=std::map<IDPath,std::string>(),
		  const std::vector<IDPath> &known=std::vector<IDPath>()
		  ){    
    std::stringstream ss;
    argc--; //no need of the exe filename
    Cell* c=new SerializedDataCell(argv[0]);
    mem.assurePath(EXECUTABLELOCATION)->setData(c);
    mem.addForDeletion(c);

      std::map<IDPath,std::string> m=defaults;
      std::vector<IDPath> lknown=known;
      for (std::map<IDPath,std::string>::iterator it=m.begin();it!=m.end();it++){
	lknown.push_back(it->first);
      }     

      if (argc){
	argv++;
	ss<<" ";
	print(" ",argv,argc,ss);
	std::vector<std::string> pairs;
	tokenizeLongDelimiter(pairs,ss.str()," -");      
	for (size_t i=0;i<pairs.size();i++){
	  //std::cout<<"pair {";
	  //write(std::cout,pairs[i]);
	  //std::cout<<"}";
	  IDPath key;
	  std::string value;
	  dataValuePair(key,value,pairs[i]);      
	  m[key]=value;
	}                
      }//if argc
  
      for (std::map<IDPath,std::string>::iterator it=m.begin();it!=m.end();it++){
	Cell* c=new CellHolder(new SerializedDataCell(it->second));
	mem.assurePath(it->first)->setData(c);
	mem.addForDeletion(c);
	if (std::find(lknown.begin(),lknown.end(),it->first)==lknown.end()){
	  std::cout<<"Warning: key "<<it->first<<" was not expected"<<std::endl;
	}
	//std::cout<<"adding "<<it->first<<" = "<<it->second<<std::endl;
      }        


  }

  
  inline std::string getParam(Memory& mem,const std::string& what){
    Cell* c=getRealCell(mem.downPath(what)->getDataP());
   
    if ((c!=NULL)&&(c->getProperties()!=NULL)&&((c->getProperties()->getCellFlags()&SERIALIZEDDATACELL)!=0)){
      return trust_cast<SerializedDataCell*>(c)->getReference();
    }else{
      std::cout<<c<<" "<<c->getProperties()<<" "<<c->getProperties()->getCellFlags()<<std::endl;
      throw exc::MissingData(what);
    }
  }

}//namespace helium

#endif


