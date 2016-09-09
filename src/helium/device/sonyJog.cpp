#include <helium/device/sonyJog.h>
#include <cstdio>

#include <helium/util/ioUtil.h>

namespace helium{

  const int SonyJog::KeyValues[KEYS]={
    2,
    17,18,19,20,
    27,22,29,
    25,28,26,
    23,24
  };
  
  SonyJog::SonyJog(const char* devname){
    if (devname){
      fp=fopen(devname, "r");    
    }
    running=true;
    flush();
  }

  SonyJog::~SonyJog(){
    running=false;
    fclose(fp);
    join();
  }
  

  void SonyJog::flush(){
    p=0;
  }


  void SonyJog::parseMouse(){
    //printn<5>(packet)<<std::endl;
    leftClick.assure((packet[LMOUSEBUTTON]&MOUSELEFT)!=0);
    rightClick.assure((packet[LMOUSEBUTTON]&MOUSERIGHT)!=0);
    stickx((char)packet[LSTICKX]);
    sticky((char)packet[LSTICKY]);
  }

  void SonyJog::parseKeyb(){
    //std::cout<<"parsing keyb"<<endl;
    //printn<8>(packet)<<std::endl;;
    int keyf=0;
    shift.set(packet[LSHIFT]==2);
    for (int i=LSHIFT+1;i<KEYBPACKETLENGTH;i++){
      if (4<=packet[i]&&packet[i]<=11){
	jog(packet[i]-4);	
      }else if(12<=packet[i]&&packet[i]<=16){
	jog(11-packet[i]);	
      }else{
	switch(packet[i]){
	case 46:
	  jog(-6);
	  break;
	case 48:
	  jog(-7);
	  break;
	case 49:
	  jog(-8);
	  break;
	case 30:
	  wheel(1);
	  break;
	case 31:
	  wheel(-1);
	  break;
	}

	for (size_t k=0;k<KEYS;k++){
	  if (packet[i]==KeyValues[k]){
	    keyf|=(1<<k);
	  }
	}

      }      
    }//for
    keys.assure(keyf);
  }

  void SonyJog::push(int c){
    //std::cout<<"pushed "<<c<<"@"<<p<<endl;
    packet[p]=c;
    p++;
    if ((packet[LPACKETTYPE]==MOUSETYPE)&&(p==MOUSEPACKETLENGTH)){//mouse like packet
      parseMouse();
      flush();
    }else if ((packet[LPACKETTYPE]==KEYBTYPE)&&(p==KEYBPACKETLENGTH)){
      parseKeyb();
      flush();
    }
  }
  
  void SonyJog::run(){
    Time prevt=getSystemMillis();   
    while(true){
      if (!running) break;
      int c=fgetc (fp);
      Time t=getSystemMillis();
      if (t-prevt>5){
	flush();
      }
      prevt=t;
      push(c);
    }
  }




  
  
  void SonyJog:: StickFilter::operator()(){
    value.assure(0);
  }

    
  void SonyJog:: StickFilter::operator()(int v){
    timer.rearm(10);
    if (v==0&&zeroCounts<2){
      zeroCounts++;
    }else{
      value.assure((double)v/(1+zeroCounts));
      zeroCounts=0;
    }
  }


  SonyJog:: StickFilter::StickFilter():ConnReference<void>(this),ConnReference<int>(this),zeroCounts(0){
      timer.signal.connect(*this);
    }




 

  AbsoluteSonyJog::AbsoluteSonyJog(const char* devname):
    SonyJog(devname)
  {
    stickx.connect(absoluteX);
    sticky.connect(absoluteY);
  }


 

};




