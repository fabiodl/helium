/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include <helium/thread/sharedAccess.h>
#include <helium/util/ioUtil.h>

namespace helium{

SharedAccess::SharedAccess():state("shAc"),users(0){
}
void SharedAccess::enterShared(){
      state.lock();      
      while(users<0){
	state.unlock();
	changeHappened.wait();
	state.lock();
      }
      users++;      
      state.unlock();
      changeHappened.signal(); //wake friends
    }



void SharedAccess::leaveShared(){
      state.lock();
      users--;
      if (users==0){
	changeHappened.signal(); //wake exclusive
      }
      if (users<0){
	throw exc::SharedAccessException("too many shared leave");
      }
      state.unlock();

    }

    void SharedAccess::enterExclusive(){
      state.lock();      
      while(users!=0){
	state.unlock();
	changeHappened.wait();
	state.lock();
      }
      users=-1;
      state.unlock();      
    }

    void SharedAccess::leaveExclusive(){
      state.lock();      
      if (users!=-1){
	throw exc::SharedAccessException("leaving exclusive with "+toString(users)+" users");
      }
      users=0;
      state.unlock();      
      changeHappened.signal(); //wake others
    }

    void SharedAccess::upToExclusive(){
      state.lock();      
      users--;
      while(users!=0){
	state.unlock();
	changeHappened.wait();
	state.lock();
      }
      users=-1;
      state.unlock();      
    }
    
    void SharedAccess::downToShared(){
      state.lock();      
      if (users!=-1){
	throw exc::SharedAccessException("down from exclusive with "+toString(users)+" users");
      }
      users=1;
      state.unlock();
      changeHappened.signal(); //wake other shares
    }

}
