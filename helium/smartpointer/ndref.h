/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_NDREF
#define HE_NDREF

#include <helium/thread/mutex.h>
namespace helium{

  template<typename T> class NDRef;


  template<typename T>    
    class SP{
    friend class NDRef<T>;
    Mutex m;
    int n;
    //public:
    T* data;    
  private:
  explicit SP():m("sp"),n(0){
    }

    //added by fb 20110530
    inline const int num() {
      return n;
    }

    inline void inc(){     
      //cout<<"inc lock"<<endl;
      m.lock();
      n++;
      //cout<<"I am "<<this<< " and my n is "<<n<<endl;
      //cout<<"inc unlock"<<endl;
      m.unlock();
    }

    inline int dec(){
      int r;
      //cout<<"dec lock"<<endl;
      m.lock();
      n--;
      r=n;
      //cout<<"I am "<<this<< " and my n is "<<n<<endl;
      //cout<<"dec unlock"<<endl;
      m.unlock();
      return r;
    }

   

  };

  template<typename T>    
    class NDRef{
    SP<T>* r;
  public:
    //SP<T>* r;    
    /*NDRef():r(NULL){
      }*/
    
    /*NDRef(){
      r=new SP<T>();
      r->data=new T();   
      r->inc();
      //std::cout<<" Constucted reference "<<this<<"with sp"<<r<<"and data"<<r->data<<endl;
      }*/

    template <typename P>
    NDRef(const P& p){
      r=new SP<T>();
      r->data=new T(p);   
      r->inc();
      //std::cout<<" Constucted reference "<<this<<"with sp"<<r<<"and data"<<r->data<<std::endl;
    }
    

    NDRef(T* t){ //the linked obj is destroyed
      r=new SP<T>();
      r->data=t;
      r->inc();
    }

    NDRef(const NDRef& b){
      r=b.r;
      r->inc();
      //std::cout<<" copy Constucted reference "<<this<<"with sp"<<r<<"and data"<<r->data<<endl;
    }

    NDRef& operator=(const NDRef& b){
      if (r->dec()==0){
	//std::cout<<" = opera,no need of "<<r->data<<std::endl;
	delete r->data;
	delete r;
      }
      //std::cout << "operator=" << std::endl;
      r=b.r;
      r->inc();
      return *this;
    }
    
    ~NDRef(){
   
      if (r->dec()==0){
	//std::cout<<"Ref "<<this<<" decons, no need of "<<r->data << "("<<*r->data<<")"<<std::endl;
	delete r->data;
	delete r;
      }
    }
    
    T& operator*(){
      return *r->data;
    }
    
    const T& operator*() const{
      return *r->data;
    }

    T* operator&(){
      return r->data;
    }

    const T* operator&() const{
      return r->data;
    }

    bool operator==(const NDRef& b){
      return r->data==b.r->data;
    }
    bool operator==(const T* b){
      return r->data==b;
    }
    const int num() {
      return r->num();
    }

    T& getData() const {
      return *r->data;
    }


    T* getPointer() const {
      return r->data;
    }
    
    inline friend std::ostream& operator <<(std::ostream& o,const helium::NDRef<T>& n)  {
      return o<<" "<< n->r->data;
    }

  };



};

#endif
