/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_CELLSIGNAL
#define HE_CELLSIGNAL

#include <helium/signal/signalType.h>
#include <helium/core/selectByValue.h>
#include <helium/system/cell.h>



namespace helium{


  template<typename UT,bool TS,bool EH=false>
  class CellSignal:public SignalType<Cell::CellSignalData,TS,EH>::value{
    typedef typename SignalType<Cell::CellSignalData,TS,EH>::value Super;
  public:
    inline void operator()(const UT& u){
      Super::operator()(&u);
    }
    
    inline void operator()(const UT& u,const Callback<Cell::CellSignalData>* noExec){
      Super::operator()(&u,noExec);
    }
    
    inline void operator()(const UT& u,
			   selectFirstOnTrue<
			     EhConnReference<Cell::CellSignalData>&,
			     ConnReference<Cell::CellSignalData>&,
			     EH> noExec){
      Super::operator()(&u,noExec.cb);
    }
  
  inline void signalAndClear(const UT& u){
    Super::signalAndClear(u);
  }
  
  };


  template<typename UT,bool TS,bool EH=false>
  class ConnectableCellSignal:public CellSignal<UT,TS,EH>,
			      public Callback<UT>,
			      public ConnReference<UT>
  {

  public:
    ConnectableCellSignal():ConnReference<UT>(this){}

    using CellSignal<UT,TS,EH>::operator();
    void operator()(UT u){
      CellSignal<UT,TS,EH>::operator()(u);
    }
    
  };


}



#endif
