/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/device/pedal.h>
#include <helium/device/deviceException.h>
#include <fstream>

using std::string;


namespace helium{
  
  
  LogitecGTForce:: LogitecGTForce(const char* dev,bool autostart,int pchecksum1,int pchecksum2,
				  int prdownThreshold,int prupThreshold,int pldownThreshold,int plupThreshold):
    checksum1(pchecksum1),checksum2(pchecksum2),
    rdownThreshold(prdownThreshold),  rupThreshold(prupThreshold),ldownThreshold(pldownThreshold),lupThreshold(plupThreshold),
    rpedalDown(false),lpedalDown(false),running(true)
    
  {
    handle=fopen(dev,"r");
    if (handle==NULL){
      throw exc::DeviceException("LogitecGTForce","unable to open "+string(dev));
    }
    if (autostart) start();

  }
    


  LogitecGTForce:: ~LogitecGTForce(){
    std::cout << "Press pedal to end..." << std::endl;
    running=false;
    fclose(handle);
    join();
  }

  void LogitecGTForce::run(){
    unsigned char data[DATANUM];
    while(running){
      for (int i=0;i<DATANUM;i++){
	data[i]=fgetc(handle);

#ifdef DEBUGPEDAL
	std::cout << "data["<<i<<"] " << (int)data[i] << std::endl;
#endif

      }
      if (!running) break;

#ifndef DEBUGPEDAL
      if ((data[CHECKSUM1]!=checksum1)||data[CHECKSUM2]!=checksum2){
	//if ((data[CHECKSUM1]!=239&&data[CHECKSUM1]!=255)||data[CHECKSUM2]!=2){
	throw exc::DeviceException("LogitecGTForce",
				   stringBegin()<<"Error pedals checksum, values are "<<(int)data[CHECKSUM1]<<" and "<<(int)data[CHECKSUM2]<<stringEnd());
	
      }
#endif


      dataReceived(data);
      wheelRotation((int)data[WHEEL]|((int)data[BUTTONS]&0x3)<<8);  //DEBUG data[1] may be 0
      

      int button = data[BUTTONS]&(LBUTTON|RBUTTON|XBUTTON|YBUTTON|ABUTTON|BBUTTON);

      if (button){
	switch(button) {
	case LBUTTON: keyPressed('L'); break;
	case RBUTTON: keyPressed('R'); break;
	case XBUTTON: keyPressed('X'); break;
	case YBUTTON: keyPressed('Y'); break;
	case ABUTTON: keyPressed('A'); break;
	case BBUTTON: keyPressed('B'); break;
	}
	
      }
      
      
      if (data[RIGHTPEDAL]<rdownThreshold&&!rpedalDown){
	rightPedalDown();
	rpedalDown=true;
      }
      
      if (data[LEFTPEDAL]<ldownThreshold&&!lpedalDown){
	leftPedalDown();      
	lpedalDown=true;
      }    
      
      if (data[RIGHTPEDAL]>rupThreshold&&rpedalDown){
	rightPedalUp();
	rpedalDown=false;
      }
      
      if (data[LEFTPEDAL]>lupThreshold&&lpedalDown){
	leftPedalUp();      
	lpedalDown=false;
      }
    }

  }

  LogitecGTEventPrinter::LogitecGTEventPrinter(LogitecGTForce& pgt):
    gt(pgt),
    rpdown("right pedal down\n"),
    rpup("right pedal up\n"),
    lpdown("left pedal down\n"),
    lpup("left pedal up\n"),
    wheelRotation("position modified to "),
    keyPressed("key press ")
    
  {
    gt.rightPedalDown.connect(rpdown);
    gt.rightPedalUp.connect(rpup);
    gt.leftPedalDown.connect(lpdown);
    gt.leftPedalUp.connect(lpup);
    gt.wheelRotation.connect(wheelRotation);
    gt.keyPressed.connect(keyPressed);
  }
  

  LogitecGTEventPrinter::~LogitecGTEventPrinter(){
    gt.rightPedalDown.disconnect(rpdown);
    gt.rightPedalUp.disconnect(rpup);
    gt.leftPedalDown.disconnect(lpdown);
    gt.leftPedalUp.disconnect(lpup);
    gt.wheelRotation.disconnect(wheelRotation);
    gt.keyPressed.disconnect(keyPressed);
  }

}//ns hel
