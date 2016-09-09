/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef _HE_YARPENCODER
#define _HE_YARPENCODER

#include <helium/yarp/yarpClient.h>
#include <helium/yarp/yarpPaths.h>


namespace helium{


  template<typename Sig>
  class YarpEncoder{    
    YarpClient& client;
    YarpPaths paths;

    class LimbListener:
      public Callback<const std::string&>,
      public ConnReference<const std::string&>
    {
      std::vector<Sig> updates;
    public:
      void resize(int n);
      void operator()(const std::string& s);

      LimbListener(const LimbListener&);
      LimbListener();
      ~LimbListener();
      void connect(int lj,ConnReference<double>&);
      void disconnect(int lj,ConnReference<double>&);      
      bool hasListeners();
    };

    std::vector<LimbListener> listeners;
    std::vector<std::pair<int,int> > jointMap;

public:  





    YarpEncoder(YarpClient& client,const YarpPaths& paths,const std::vector<int>& pathSizes);

    void connect(int j,ConnReference<double>& );
    void disconnect(ConnReference<double>& );

};


}

#include <helium/yarp/yarpEncoder.hpp>


#endif
