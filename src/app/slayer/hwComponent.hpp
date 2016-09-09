/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

namespace helium{
template<typename Component>
void HwFactory<Component>::create(Memory& mem,const HwComponentName& name,const IDPath& confPath,const IDPath& servicePath){  
  Component* c=new Component(mem,name,confPath,servicePath);
  (c->isTemporary()?tempComps:comps).push_back(c);
}


  template<typename Component>
  HwFactory<Component>::HwFactory(const HwFactoryName& name):HwFactoryInterface(name){
  }
  

  template<typename Component>
  bool HwFactory<Component>::canBeDestroyed(){
    for (size_t i=0;i<tempComps.size();i++){
      delete tempComps[i];
    }
    tempComps.clear();
    return comps.size()==0;
  }



template<typename Component>
HwFactory<Component>::~HwFactory(){
  for (size_t i=0;i<comps.size();i++){
    delete comps[i];
  }
}


}//ns helium
