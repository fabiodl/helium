#ifndef HE_SPOOLERCONTROL
#define HE_SPOOLERCONTROL

#include <helium/spooling/spooler.h>
#include <helium/system/basicCell.h>


namespace helium{
  class SpoolerControlSignature:public CellSignature{
  public:
    typedef int GT;
    typedef Void GP;
    typedef Void ST;
    typedef Void SP;
    typedef int UT;
    typedef int UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=true;
  };



  template<typename Res>
  class SpoolerControl:public TypedCell<SpoolerControlSignature>{
    Spooler<Res>& owner;
    Mutex m;
    typedef std::list<std::pair<Cell::CellCallback*,int> > List;

    List prior;



    std::string getName() const{
      return "spooling min priority allowed";
    }

    void getData(int& p){      
      p=owner.getMinPriority();      
    }

    void connect(Cell::CellConnection& ref,const int& t){
      m.lock();
      for ( List::iterator it=prior.begin();
	    it!=prior.end();++it){
	(*it->first)(&t);
      }

      ref.disc=new IterDiscInfo<int,List::iterator>(prior.insert(prior.end(),std::make_pair(ref.cb,t) ));
      owner.setMinPriority(t);
      m.unlock();      
    }
    
    void disconnect(Cell::CellConnection& ref){
      m.lock();
      IterDiscInfo<int,List::iterator>* it=trust_cast<IterDiscInfo<int,List::iterator>*>( ref.disc);
      prior.erase(it->it);
      if (prior.size()>0){
	owner.setMinPriority(prior.back().second);
      }else{
	owner.setMinPriority(0);
      }
      m.unlock();
    }


    /*void setData(const int& p){
      owner.setMinPriority(p);
      }*/

    /*void setData(StdReply& r,const int& p){
      owner.setMinPriority(p);
      }*/





  public:
    SpoolerControl(Spooler<Res>& powner):owner(powner),m("spoolctrl"){
    }

  };


};



#endif
