/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/yarp/yarpEncoder.h>
#include <helium/signal/wrapInfo.h>


namespace helium{





template<typename Sig>
void YarpEncoder<Sig>::LimbListener::operator()(const std::string& s){
  std::stringstream ss(s);
  std::string msgheader;double d;
  for (size_t i=0;i<updates.size();++i){
    ss>>d;
    updates[i](d*M_PI/180);
  }
}


template<typename Sig>
YarpEncoder<Sig>::LimbListener::LimbListener():
  ConnReference<const std::string&>(this){
  //ConnReference<const std::string&>* c=this;
  //std::cout<<"new ConnReference @"<<c<<std::endl;
}


template<typename Sig>
YarpEncoder<Sig>::LimbListener::~LimbListener(){
  //ConnReference<const std::string&>* c=this;
  //std::cout<<"death of ConnReference @"<<c<<std::endl;
}

template<typename Sig>
YarpEncoder<Sig>::LimbListener::LimbListener(const LimbListener&):
  ConnReference<const std::string&>(this){
  //ConnReference<const std::string&>* c=this;
  //std::cout<<"new copy ConnReference @"<<c<<std::endl;
}



template<typename Sig>
bool YarpEncoder<Sig>::LimbListener::hasListeners(){
  for (size_t i=0;i<updates.size();i++){
    if (updates[i].listenerNum()) return true;
  }
  return false;
}


template<typename Sig>
void YarpEncoder<Sig>::LimbListener::resize(int n){
  updates.resize(n);
}



template<typename Sig>
void YarpEncoder<Sig>::LimbListener::connect(int lj,ConnReference<double>& c){
  updates[lj].connect(c);
}


template<typename Sig>
void YarpEncoder<Sig>::LimbListener::disconnect(int lj,ConnReference<double>& c){
  updates[lj].disconnect(c);
}



template<typename Sig>
YarpEncoder<Sig>::YarpEncoder(YarpClient& pclient,const YarpPaths& ppaths,const std::vector<int>& pathSizes):
  client(pclient),
  paths(ppaths)
{
  
  if (paths.paths.size()!=pathSizes.size()){
    throw exc::InvalidOperation("Creating Yarpencoder: different sizes for path limbs and axes in each limb.");
  }
  
  listeners.resize(pathSizes.size());
  //std::cout<<listeners.size()<<"listeners"<<std::endl;
  for (size_t i=0;i<pathSizes.size();++i){
    listeners[i].resize(pathSizes[i]);
    for (int j=0;j<pathSizes[i];++j){
      jointMap.push_back(std::make_pair(i,j));
    }    
  }
  

}



template<typename Sig>
void YarpEncoder<Sig>:: connect(int j,ConnReference<double>& c){
  LimbListener& listener=listeners[jointMap[j].first];
  

  bool hadListeners=listener.hasListeners();
  listener.connect(jointMap[j].second,c);
  wrapInfo(c,j);
  if (!hadListeners){
    //ConnReference<const std::string&>* la=&listener;
    //std::cout<<"Connectign "<<la<<"which is"<<jointMap[j].first<<"th"<<std::endl;
    //listener("123");
    client.connectReader(paths.paths[jointMap[j].first]+"/state:o",listener);
  }
}



template<typename Sig>
void YarpEncoder<Sig>:: disconnect(ConnReference<double>& c){  
  int j;
  unwrapInfo(c,j);
  LimbListener& listener=listeners[jointMap[j].first];
  listener.disconnect(jointMap[j].second,c);
  if (!listener.hasListeners()){
    client.disconnectReader(listener);
  }

}

    




}
