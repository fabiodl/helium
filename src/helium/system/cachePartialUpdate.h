/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_CACHEPARTIALUPDATE
#define HE_CACHEPARTIALUPDATE


namespace helium{
  

  //used for keeping an internal cache of type GT=Array<T>,refreshed with updates of type UT=std::pair<int,T>

  template<typename VP>
  class CachePartialUpdate:public VP,
			   public Callback<typename VP::V::RUT>,
			   public ConnReference<typename VP::V::RUT>{

  public:
    CachePartialUpdate():
      VP(helium::Void()),
      ConnReference<typename VP::V::RUT>(this){
      this->connect(this);
    }

    void init(size_t n, const typename VP::V::GT::value_type& initVal){
       this->wCache.resize(n);	
       setAll(this->wCache,initVal);
    }
    
    void operator()(typename VP::V::RUT p){
      this->wCache[p.first]=p.second;
    }
    
    void onRemoteAvailable(){
      this->get(this->wCache);
      for (size_t i=0;i<this->wCache.size();i++){	    
	typename VP::V::UT ut(i,this->wCache[i]);
	this->signalData(ut);
      }      
    }

  };//CachePartialUpdate



}//ns 




#endif
