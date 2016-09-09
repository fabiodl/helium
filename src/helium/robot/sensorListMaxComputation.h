#ifndef HE_SENSORLISTMAXCOMPUTATION
#define HE_SENSORLISTMAXCOMPUTATION

#include <helium/robot/genericFilter.h>

namespace helium{

  class SensorListMaxComputation:public sensor::GenericFilterComputation<double,double>{
  int ns;//n of simulated t.s.

    Array<std::vector<int> > s2r;//simulated 2 real
    Array<std::vector<int> > r2s; //real to simulated
  

  void compute(double* r,const double *s,const HwIdList& what){    
    for (size_t i=0;i<what.ids.size();i++){
      int idx=what.ids[i];
      r[idx]=0;
      for (size_t j=0;j<r2s[i].size();j++){
	r[idx]=(std::max)(r[idx],s[r2s[i][j]]);
      }	   
    }
  }
  
  void getRequired(HwIdList& req,const HwIdList& what){
    req.ids.clear();
    for (size_t i=0;i<s2r.size();i++){
      for (size_t j=0;j<s2r[i].size();j++){
	if (contains(what.ids,s2r[i][j])){
	  req.ids.push_back(i);
	  break;
	}
      }//for j
    }//for i
  }//getRequired

    void getGenerated(HwIdList& gen,const HwIdList& id){
      gen.ids.size();
      for (size_t i=0;i<id.ids.size();++i){
	int idx=id.ids[i];
	for (size_t j=0;j<s2r[idx].size();++j){
	  if (!contains(gen.ids,s2r[idx][j])){
	    gen.ids.push_back(s2r[idx][j]);
	  }//if
	}//for j
      }//for i
    }


  public:
    void add(int r,int s){
      s2r[s].push_back(r);
      r2s[r].push_back(s);
    }

    void resize(int nreal,int nsimul){
      r2s.resize(nreal);
      s2r.resize(nsimul);
    }

    int getSimulSize(){return s2r.size();}
    int getRealSize(){return r2s.size();}

    SensorListMaxComputation(int nreal,int nsimul):s2r(nsimul), 
						   r2s(nreal){
    }

};

  template<>
  class DefDescr<SensorListMaxComputation >: public Description<SensorListMaxComputation >{
    Array<std::pair<int,int> > sensMapping;
  public:
    DefDescr(SensorListMaxComputation & obj):Description<SensorListMaxComputation >(obj){}

    void  getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(sensMapping,""));
    }

    void onInitializationFinished(helium::Memory& mem){
      int rmax=-1,smax=-1;
      
      for(size_t i=0;i<sensMapping.size();i++){
	rmax=std::max(rmax,sensMapping[i].first);
	smax=std::max(smax,sensMapping[i].second);
      }
      obj.resize(rmax+1,smax+1);

      for(size_t i=0;i<sensMapping.size();i++){
	obj.add(sensMapping[i].first,sensMapping[i].second);
	//std::cout<<"mapping "<<sensMapping[i].first<<" to "<<sensMapping[i].second<<std::endl;
      }
    }
  };

  

}



#endif

