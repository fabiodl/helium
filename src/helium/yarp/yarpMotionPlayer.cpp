/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/yarp/yarpMotionPlayer.h>

using namespace helium;



YarpMotionPlayer::TempData::TempData(int n):
  posture(n),maxv(n),v(n){
}


void YarpMotionPlayer::play(const helium::Motion& m,const helium::MotionParameter& mp){
  mutex.lock();
  currMotion=m;
  motionParam=mp;
  playing=true;
  zeroTime=(timeSource.getTime()-mp.startTime)/mp.playSpeed;
  mutex.unlock();
  hasWork();
}

void YarpMotionPlayer::stop(){
  mutex.lock();
  playing=false;
  mutex.unlock();
}


YarpMotionPlayer::YarpMotionPlayer(YarpMotor& pym,const double* maxv):
  mutex("localPlayManager"),
  threadRunning(false),
  ym(pym),
  tempData(ym.getAxesNum()),
  currMotion(ym.getAxesNum()),
  policy(currMotion),
  motionParam(-1,INVALIDTIME,1),
  hasWorkConnection(&hasWork),
  playing(false)
{
  std::copy(maxv,maxv+tempData.maxv.size(),std::back_inserter(tempData.maxv));
  start();
}

YarpMotionPlayer::~YarpMotionPlayer(){

}

void YarpMotionPlayer::run(){
  threadRunning=true;
  timer.signal.connect(hasWorkConnection);
  while(threadRunning){
    hasWork.wait();    
    mutex.lock();
    if (playing){            
      Time currentTime=motionParam.playSpeed*(timeSource.getTime()-zeroTime);
      int iteration=floor(currentTime/policy.getEndTime());
      std::cout<<"currenTime="<<currentTime<<"iteration="<<iteration<<" of "<<motionParam.iters<<std::endl;
      if (motionParam.iters>=0&&iteration>=motionParam.iters){
	policy.getPosture(&tempData.posture[0],currMotion.getEndTime());
	ym.setVelocity(&tempData.maxv[0]);
	ym.rotate(&tempData.posture[0]);
	framePlayState(std::make_pair(framePlay::STOPPED,currMotion.getEndTime()) );
	playing=false;
      }else{
	Time now=currentTime-iteration*policy.getEndTime();
	framePlayState(std::make_pair(framePlay::PLAYING,currMotion.getEndTime()) );
	Time nextTime=currMotion.getNextTime(now);
	policy.getPosture(&tempData.v[0],now);//v as temporary
	policy.getPosture(&tempData.posture[0],nextTime);
	for (size_t i=0;i<tempData.v.size();++i){
	  tempData.v[i]=fabs(1000*(tempData.posture[i]-tempData.v[i])/(nextTime-now));
	}
	ym.setVelocity(&tempData.v[0]);
	std::cout<<"Now is "<<now<<" setting"<<tempData.v[0]<<std::endl;
	ym.rotate(&tempData.posture[0]);	
	timer.rearm(nextTime-now);
      }
      
    }//if Playing
    
    mutex.unlock();
    
  }//while threadRunning
  timer.signal.disconnect(hasWorkConnection);

}
