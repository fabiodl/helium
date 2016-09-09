/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#ifndef HE_HISTORYSTACK
#define HE_HISTORYSTACK

#include <iostream>
#include <vector>

namespace helium{

  /*  template<typename T> void swap(T& a,T& b){
    T t(a);
    a=b;
    b=t;
    }*/
  
  template<typename T>
  class ExternalPresentHistoryStack{ ///< present state not included in the stack
    std::vector<T> hist;
    int futureCnt;
  public:
    
    ExternalPresentHistoryStack():futureCnt(0){
  }

    void clear() {
      hist.clear();
      futureCnt = 0;
    }
    
    void pushPast(const T& past){
      if (futureCnt){ //prevent "double future" when rewind and then start doing new things
	hist.erase(hist.begin()+ (hist.size()-futureCnt),hist.end());
	futureCnt=0;
      }
      hist.push_back(past);
    }

    bool canForward(){
      return futureCnt>0;
    }

    bool canRewind(){
      return hist.size()-futureCnt>0;
    }

    void rewind(T& present){
      futureCnt++;
      std::swap(present,hist[hist.size()-futureCnt]);
    }

    void forward(T& present){
      std::swap(present,hist[hist.size()-futureCnt]);
      futureCnt--;
    }

    void printt() {
    for(size_t i=0;i<hist.size();i++)  {
      std::cout << "he" << i << " ";
      print(hist[i]);
      std::cout  << std::endl;
    }
    }

   size_t getSize() {
      return hist.size();
    }

  }; //ExternalPresentHistoryStack
  
  
  
  template<typename T>
  class InternalPresentHistoryStack{ ///< present state is in the stack
    std::vector<T> hist;
    int futureCnt;

    void destroyFuture(){
      if (futureCnt){
	hist.erase(hist.begin()+ (hist.size()-futureCnt),hist.end());	
	futureCnt=0;
      }
    }

  public:    
    void clear(const T& present) {
      hist.clear();
      futureCnt = 0;
      hist.push_back(present);
    }

    InternalPresentHistoryStack(const T& present):futureCnt(0){
      hist.push_back(present);
    }

    void pushPast(){
      destroyFuture();
      hist.push_back(hist.back());    
    }

    void pushPresent(const T& t){ //equal to pushPast(); present()=t;
      destroyFuture();
      hist.push_back(t);
    }

    T& previous(){
      return hist[hist.size()-2-futureCnt];
    }

    T& present(){
      return hist[hist.size()-1-futureCnt];
    }

    T& future() {
      return hist[hist.size()-futureCnt];
    }


    const T& previous() const{
      return hist[hist.size()-2-futureCnt];
    }

    const T& present() const{
      return hist[hist.size()-1-futureCnt];
    }

    const T& future() const{
      return hist[hist.size()-futureCnt];
    }


    bool canRewind() const{
      return hist.size()-futureCnt>1;
    }

    bool canForward() const{
      return futureCnt>0;
    }
    
    void rewind(){
      futureCnt++;
    }

    void forward(){
      futureCnt--;
    }

    size_t getSize() const{
      return hist.size();
    }

    void print() const{
      for(size_t i=0;i<hist.size();i++)  {
	std::cout << i << " " << hist[i] << std::endl;
      }
    }

  };


}




#endif
