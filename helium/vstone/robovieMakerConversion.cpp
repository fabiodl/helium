#include <helium/vstone/robovieMakerConversion.h>
#include <helium/robot/framePolicy.h>
namespace helium{
  
  void importMotion(Motion& mot,RMPoseFile& src,RMCalibFile& cal,const IdMapper& idMap,LinearPosNormalizer<VServoSign>& normalizer){
    std::vector<std::vector<int> > absPos;
    std::vector<int> basePos;
    std::vector<int> dt;    
    src.getPoses(absPos,dt);
    cal.getCalibration(basePos);
    std::cout<<absPos.size()<<" poses in the file"<<std::endl;
    double t=0;
    Posture pos(mot.getJointSize());
    for (size_t i=0;i<absPos.size();i++){
      t+=1000/60*dt[i];
      for (size_t j=0;j<pos.size();j++){
	int ridx=idMap.rawId[j];
	pos[j]=normalizer.normPos((absPos[i][ridx]>>4)+basePos[ridx],j);	
      }
      mot.insertFrame(t,pos);
    }
    if (dt[0]){
      mot.linkFrame(1000/60*dt[0],0);
    }

  }

  void exportMotion(RM2File& f,Motion& mot,RMCalibFile& cal,const IdMapper& idMap,LinearPosNormalizer<VServoSign>& normalizer,bool loopy){

    std::vector<int> basePos;
    std::vector<int> dt;   
    std::vector<std::vector<int> > rawPoses;

    int prev=0;
    
    Array<double> v(mot.getJointSize());
    std::vector<int> rawPos;    
    rawPos.resize(30);

    cal.getCalibration(basePos);

    FramePolicy p(mot,false);
    for (Motion::MotionMap::iterator it=mot.motion.begin();it!=mot.motion.end();++it){
      int t=it->first;
      dt.push_back((t-prev)*60/1000);
      prev=t;
      p.getPosture(v.data,t);
      for (size_t j=0;j<v.size();j++){
	int ridx=idMap.rawId[j];
	rawPos[ridx]=(normalizer.unnormPos(v[j],j)-basePos[ridx])<<4;	
      }
      rawPoses.push_back(rawPos);
    }
    f.setPoses(rawPoses,dt,loopy);

  }

  
  void importMotion(Motion& mot,RMPoseFile& src,RMCalibFile& cal,Memory& mem){
     motor::Calib<VServoSign> calib;
     IdMapper idMap;
     dimport(calib,mem);
     dimport(idMap,mem);
     LinearPosNormalizer<VServoSign> psNorm(calib);
     importMotion(mot,src,cal,idMap,psNorm);
  }

  void exportMotion(RM2File& f,Motion& mot,RMCalibFile& cal,Memory& mem,bool loopy){
    motor::Calib<VServoSign> calib;
    IdMapper idMap;
    dimport(calib,mem);
    dimport(idMap,mem);
    LinearPosNormalizer<VServoSign> psNorm(calib);
    exportMotion(f,mot,cal,idMap,psNorm);
  }

  
  RMOutputFile:: RMOutputFile(const char* filename):
    RMFile(filename,true),RM2File((RMFile&)*this){
  }


}
