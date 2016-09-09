/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

namespace helium{

template<typename T>
void YarpRpc::get(T& t,const std::string& path,const std::string& what){
  static const int BUFFERSIZE=1024;
  char buffer[BUFFERSIZE];
  std::string fullpath=path+"/rpc:i";
  client.send(fullpath,("d\nget "+what+"\n").c_str());
  client.recvUntil(fullpath,buffer,BUFFERSIZE,"[ok]\r\n");
  std::stringstream ss;
  ss<<buffer;
  std::string s;
  ss>>s;//[is]
  ss>>s;//what
  read(t,ss);
  ss>>s;//[ok]
}



template<typename T>
void YarpRpc::set(const T& t,const std::string& path,const std::string& what){
  //std::cout<<"Sending{"<<(stringBegin()<<"do\nset "<<what<<" "<<t<<"\n"<<stringEnd()).c_str()<<"}"<<std::endl;
  client.send(path+"/rpc:i",(stringBegin()<<"do\nset "<<what<<" "<<t<<"\n"<<stringEnd()).c_str());
}

}
