/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SLNODE
#define HE_SLNODE


namespace helium{

    namespace node{

    template<typename T>
    struct SlAutoDeleteNode{   
      typedef T value_type;
      T data;
      SlAutoDeleteNode* next;
      SlAutoDeleteNode(const T& pdata):data(pdata),next(NULL){
      }           
      ~SlAutoDeleteNode(){
	delete data;
      }
      T& operator*(){
	return data;
      }
    };

    template<typename T>
    struct SlPlainNode{
      typedef T value_type;
      T data;
      SlPlainNode* next;
      SlPlainNode(const T& pdata):data(pdata),next(NULL){
      }      
      
      T& operator*(){
	return data;
      }
    };

      template<typename T>
    struct SlDCNode{
      typedef T value_type;
      T data;
      SlDCNode* next;
      SlDCNode(const T& pdata):data(pdata),next(NULL){
      }      
	
	SlDCNode():next(NULL){
	}      
	
	T& operator*(){
	  return data;
	}
      };

      
       

    }//ns node
  
}//ns helium

#endif
