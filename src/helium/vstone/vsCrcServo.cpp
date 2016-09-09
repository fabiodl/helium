#include <helium/vstone/vsCrcServo.h>

namespace helium{

  ///static const int RETRIES=2;
 
  namespace vservo{

    //typedef motor::Raw<VServoSign>::PropertyList::const_iterator IT;

    /*void GetterToken::notify(int i){
      (*to[i])(&props[i]);
      }*/

    /*GetterToken::GetterToken(const motor::Raw<VServoSign>::PropertyList &info,motor::Raw<VServoSign>::PropNormalizer& pn):
     props(info.size()),to(info.size()){
     //std::cout<<"|created "<<this<<std::endl;
     int i=0;
     for (IT it=info.begin();it!=info.end();++it){
       //props[i].first
       props[i].second=it->second;
       props[i].third=&pn;
       to[i]=it->first;
       i++;
     }
     
   }//GetterToken ctor
    */
    /*void GetterToken::splitByJoint(Array <motor::Raw<VServoSign>::Token>& tk,const motor::Raw<VServoSign>::PropertyList &info,motor::Raw<VServoSign>::PropNormalizer& pn){
      std::map<int,motor::Raw<VServoSign>::PropertyList> lists;
      for (IT it=info.begin();it!=info.end();++it){
	lists[it->second->joint].push_back(*it);
      }
      typedef std::map<int,motor::Raw<VServoSign>::PropertyList>::iterator  Mit;
      tk.resize(lists.size());
      int i=0;
      for (Mit mit=lists.begin();mit!=lists.end();++mit){
      	tk[i].reset(new GetterToken(mit->second,pn));
	i++;
      }
      }*/


  }//ns vservo
    

}//ns helium
 

 
