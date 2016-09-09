#ifndef HE_MOCAP
#define HE_MOCAP
#include <helium/system/cellSignature.h>
namespace helium{

  static const int MOCAPCELLPORT=49001;
  extern const IDPath BODIESLOCATION;
  extern const IDPath UNRECOGNIZEDLOCATION;
  extern const IDPath WHOLELOCATION;


  typedef Array<double,3> Marker;

  class MarkerSet: public std::vector<Marker >{
  };

  template<>
  struct hasSize<MarkerSet>{
    static const bool value=true;
  };


  class WholeMarkerSet{
  public:
    std::vector<MarkerSet> bodies;
    MarkerSet unrecognized;
  };




  template<>
  struct ReadPolicy<MarkerSet>{
    static const int value=READSN;
  };

  template<>
  struct WritePolicy<MarkerSet>{
    static const int value=WRITESN;
  };

  
  inline std::ostream& operator<<(std::ostream& o,const WholeMarkerSet& m){   
    sizeWrite(o,m.bodies);
    o<<" ";
    write(o,m.unrecognized);
    return o;
  }
  
  inline std::istream& operator>>(std::istream& i,WholeMarkerSet& m){   
    sizeRead(m.bodies,i);
    read(m.unrecognized,i);
    return i;
  }


  class SingleBodySignature:public CellSignature{    
  public:
    typedef MarkerSet GT;
    typedef int GP;//body id
    typedef Void ST;
    typedef Void SP;
    typedef MarkerSet UT;
    typedef int UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
    //typedef DefaultAccessPolicy<GT,GP,ST,SP,UT,UP,
    //getRequiresParam,setRequiresParam,connectRequiresParam,true,DISABLEAUTOCONNECT> Access;
 };
  

  typedef DefaultRoSignature<MarkerSet> UnrecBodySignature;

  template<>   
  struct DefaultAccessOptions<UnrecBodySignature>{
    static const int value=REQUIRELISTENERSFORAUTOCONNECT| UPDATEUSESREFERENCE;
  };

  typedef DefaultRoSignature<WholeMarkerSet> WholeMarkerSignature;

  template<>   
  struct DefaultAccessOptions<WholeMarkerSignature>{
    static const int value=REQUIRELISTENERSFORAUTOCONNECT| UPDATEUSESREFERENCE;
  };


  



}//helium
#endif
