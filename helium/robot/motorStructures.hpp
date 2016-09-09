/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/system/hexMapping.h>
#include <helium/system/cellDescription.h>
namespace helium{
  namespace motor{
  
  template<typename MotorSign,bool hasZero>
  MotorCalib<MotorSign,hasZero>::MotorCalib():zero(MotorSign::DEFAULTZERO),
			   minv(MotorSign::DEFAULTMIN),
			   maxv(MotorSign::DEFAULTMAX){
  }


  template<typename MotorSign>  
  MotorCalib<MotorSign,false>::MotorCalib():minv(MotorSign::DEFAULTMIN),
		   maxv(MotorSign::DEFAULTMAX){
      }

  }//ns motor

  template<typename MotorSign>
  motor::Interface::Position LinearPosNormalizer<MotorSign>::normPos(typename motor::Raw<MotorSign>::Position value,int joint){           
    if (value==MotorSign::INVALIDPOS) return motor::Interface::INVALIDPOS;
      return (value-calib.data[joint].zero)*MotorSign::TONORM;
    }

   template<typename MotorSign>
   typename motor::Raw<MotorSign>::Position  LinearPosNormalizer<MotorSign>::unnormPos(motor::Interface::Position value,int joint){
     if (value==motor::Interface::INVALIDPOS) return MotorSign::INVALIDPOS;
      return (typename motor::Raw<MotorSign>::Position)(round(value*MotorSign::TORAW+calib.data[joint].zero));
    }


  template<typename MotorSign>
  motor::Interface::Property PositionOnlyPropNormalizer<MotorSign>::normProp(typename motor::Raw<MotorSign>::Property value,const motor::PropertyRequest& r){
    switch (r.id){
    case motor::TARGETPOSITION:
    case motor::POTENTIOPOSITION:
      return posNorm.normPos(value,r.joint);
    default:
      throw exc::InvalidOperation("Conversion for property "+toString(r.id)+" not available");
    }       
  }

  template<typename MotorSign>  
  typename motor::Raw<MotorSign>::Property PositionOnlyPropNormalizer<MotorSign>::unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r){
    switch (r.id){
    case motor::TARGETPOSITION:
    case motor::POTENTIOPOSITION:
      return posNorm.unnormPos(value,r.joint);
    default:
      throw exc::InvalidOperation("Conversion to raw for property "+toString(r.id)+" not available");
    }       
    
  }



  template<typename MotorSign>
  PosLimiter< MotorSign>::PosLimiter(motor::Calib<MotorSign>& pcalib):calib(pcalib){
    }
  
  template<typename MotorSign>
  typename motor::Raw<MotorSign>::Position PosLimiter< MotorSign>::getMin(int joint){
    return calib.data[joint].minv;
  }
  
  template<typename MotorSign> 
  typename motor::Raw<MotorSign>::Position PosLimiter< MotorSign>::getMax(int joint){
      return calib.data[joint].maxv;
    }

  template<typename MotorSign> 
  typename motor::Raw<MotorSign>::Position PosLimiter< MotorSign>::limit(typename motor::Raw<MotorSign>::Position val,int joint){
      if (val<calib.data[joint].minv){
	return calib.data[joint].minv;
      }
      if (val>calib.data[joint].maxv){
	return calib.data[joint].maxv;
      }
      return val;
    }

  template<typename MotorSign> 
  void DefDescr<motor::MotorCalib<MotorSign,true> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(hexMapping(this->obj.minv,"min"));
      members.push_back(hexMapping(this->obj.maxv,"max"));
      members.push_back(hexMapping(this->obj.zero,"zero"));     
    }    
  
  template<typename MotorSign> 
  void DefDescr<motor::MotorCalib<MotorSign,false> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(hexMapping(this->obj.minv,"min"));
      members.push_back(hexMapping(this->obj.maxv,"max"));  
    }    

  template<typename MotorSign> 
  void DefDescr<motor::Calib<MotorSign> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      //std::cout<<"mapping calib"<<std::endl;
      members.push_back(vectMapping(this->vd(this->obj.data),
				    IDPath("motors.motor","."),"calib",IDPath("motors.num",".")));
    }    
  
  /*template<>
  void DefDescr<IdMapper>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(vectMapping(vd(obj.rawId),IDPath("robot.motors.motor","."),"rawId",IDPath("robot.motors.num",".") ));
      }*/

  template<typename MotorSign>
  void MotorGetterToken<MotorSign>::notify(int i){
      (*to[i])(&props[i]);
  }



 

  template<typename MotorSign>
   MotorGetterToken<MotorSign>::MotorGetterToken(const typename motor::Raw<MotorSign>::PropertyList &info,typename motor::Raw<MotorSign>::PropNormalizer& pn):
     props(info.size()),to(info.size()){
     //std::cout<<"|created "<<this<<std::endl;
     int i=0;
     typedef typename motor::Raw<MotorSign>::PropertyList::const_iterator IT;
     for (IT it=info.begin();it!=info.end();++it){
       //props[i].first
       props[i].second=it->second;
       props[i].third=&pn;
       to[i]=it->first;
       i++;
     }
  }//GetterToken ctor


  template<typename MotorSign>
  void MotorGetterToken<MotorSign>::splitByJoint(Array <typename motor::Raw<MotorSign>::Token>& tk,const typename motor::Raw<MotorSign>::PropertyList &info,typename motor::Raw<MotorSign>::PropNormalizer& pn){
      std::map<int,typename motor::Raw<MotorSign>::PropertyList> lists;
      typedef typename motor::Raw<MotorSign>::PropertyList::const_iterator IT;
      for (IT it=info.begin();it!=info.end();++it){
	lists[it->second->joint].push_back(*it);
      }
      typedef typename std::map<int,typename motor::Raw<MotorSign>::PropertyList>::iterator  Mit;
      tk.resize(lists.size());
      int i=0;
      for (Mit mit=lists.begin();mit!=lists.end();++mit){
      	tk[i].reset(new MotorGetterToken(mit->second,pn));
	i++;
      }
  }//splitByJoin


 



}//ns helium
