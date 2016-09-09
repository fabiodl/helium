/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#include <helium/system/remoteCell.h>


namespace helium{

  

  
  namespace exc{
    class UnknowTypeRemoteCell:public UnsupportedCellOperation{
    public:
      UnknowTypeRemoteCell(CellOperation t,std::string& name):	
	UnsupportedCellOperation(t,"typeless remote cell "+name){
      }
    };//UnknowTypeRemoteCell
  }//exc
  
      
  void RemoteCell<UnspecifiedRemoteCellSignature>::streamGet(std::ostream& o){
    CellClient::WholeContentRequest r(remotename,client,o);
    client->get(&r);
  }

  void RemoteCell<UnspecifiedRemoteCellSignature>::streamGet(std::ostream& o,std::istream& i){
    std::string s;
    std::getline(i,s,char(0));
    CellClient::WholeContentRequest r(remotename,client,o);    
    client->get<std::string,std::string>(&r,&s);
  }

  void RemoteCell<UnspecifiedRemoteCellSignature>::streamSet(std::istream& i){
    std::string s;
    std::getline(i,s,char(0));
    client->set(remotename,&s);
  }


  void RemoteCell<UnspecifiedRemoteCellSignature>::streamSet(std::ostream& o,std::istream& i){
    std::string s;
    std::getline(i,s,char(0));
    CellClient::WholeContentRequest r(remotename,client,o);    
    client->set<std::string>(&r,&s);
  }

}//helium
