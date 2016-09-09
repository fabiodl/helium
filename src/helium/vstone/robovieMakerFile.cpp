#include <helium/vector/operations.h>
#include <helium/vstone/robovieMakerFile.h>
#include <helium/util/mathUtil.h>

namespace helium{



  UnicodeFile::UnicodeFile(const char* filename,bool out):f(filename,
							    (out?std::fstream::out:std::fstream::in) | std::ios::binary){
    if (!f.is_open()){
      throw exc::FileOpenRead(filename);
    }
    /*if (out){
      f<<(char)(255)<<(char)(254);
    }
    else{
      unsigned char bom[2];
      f>>bom[0];
      f>>bom[1];
      if (bom[0]!=0xFF||bom[1]!=0xFE){
	throw exc::BadFileFormat("Vstone file",filename,"bad byte order mark (BOM)"+toString(int(bom[0]))+" "+toString(int(bom[1])) );
	}   
	}*/
  }
  

  UnicodeFile&   UnicodeFile::operator<<(const std::string& s){
    //std::cout<<"length "<<s.length()<<std::endl;
    for (size_t i=0;i<s.length();i++){     
      f<<s[i]<<(char)(0);
    }
    return *this;
  }


  bool UnicodeFile::getline(std::string& s){
    s="";
    //char c[2];
    unsigned char c;
    if(!f) return false;
    while (f){
      f.read((char*)&c,1);      
      if (!f) return false;
      if (c==13||c==0xFF||c==0xFE||c==0) continue;
      if (c==10) break;

      s+=c;
    }
    return true;
  }


  RMFile::RMFile(const char* pfilename,bool out):UnicodeFile(pfilename,out),filename(pfilename){
    if (!out){
      std::string s;
      while(getline(s)){      
	//if (s.length()==0) continue;
	//std::cout<<"got line {"<<s<<"}"<<std::endl;
	content.push_back(Line(s));
      }    
    }
  }



  std::ostream& operator<<(std::ostream& o,const RMFile::Line& l){
    if (l.type.length()>0){
    o<<l.type<<":";
    if (l.components.size()!=0){
      o<<"[";
      print("]-[",l.components,o);
      o<<"]";    
    }
  }
  return o<<char(0xd)<<char(0xa);
}


void RMFile::write(){
  f<<(char)(255)<<(char)(254);
  for (size_t i=0;i<content.size();i++){
    *this<<content[i];
  }
  f.flush();
}




  RMFile::Line::Line(const std::string& s){
    if (s.length()==0){
      return;
    }
    size_t pos=s.find(':');
    if (pos==std::string::npos) throw exc::BadFileFormat("Vstone file","","no semicolon in line "+s);
    
    type=s.substr(0,pos);

    int level=0;
    std::string currcont;
    for (pos=pos+1;pos!=s.length();pos++){
      switch(s[pos]){
      case '[':
	if (level==0){
	  currcont="";	 
	}else{
	  currcont+=s[pos];
	}
	level++;
	break;
      case ']':
	level--;
	if (level==0){
	  components.push_back(currcont);
	}else{
	  currcont+=s[pos];
	}
	break;
      default:
	currcont+=s[pos];
      }//switch
    }//for
  }

  const std::string& RMFile::Line::operator[](size_t f) const{
    if (components.size()<=f){
      throw exc::BadFileFormat("Vstone file","","expected field "+toString(f));    
    }
    return components[f];
  }
  
  
  
  
  
template<typename T> //for long
  void rmReadInt(T& r,const std::string& s){
    convert(r,s);
  }
template<typename T>
std::string rmWriteInt(const T& t){
  return toString(t);
}


  template<typename T> //for long
  void rmReadHex(T& r,const std::string& s){
    convertHex(r,s);
    r=expandBits(r,16);
  }

template<typename T> //for long
std::string rmWriteHex(const T& i){
  char buffer[256];
  sprintf(buffer,"0x%04x",i);
  buffer[strlen(buffer)-6]='0'; //delete the FFFFF at the beginning for negative numbers
  buffer[strlen(buffer)-5]='x';  
  return std::string(&buffer[strlen(buffer)-6]);
}


  void rmReadString(std::string& r,const std::string& s){
    if (s.length()<2){
      throw exc::BadFileFormat("Vstone file","","bad string delimiters");    
    }
    r=s.substr(1,s.length()-2);
  }

std::string rmWriteString(std::string& s){
  return "["+s+"]";
}



  void rmReadUnescapedString(std::string& r,const std::string& s){
    std::stringstream ss;
    ss<<s;
    ss>>r;
  }


  template<typename T>
  void rmReadHexList(T& x,const std::string& s){
    x.clear();
    std::vector<std::string> tokens;
    tokenize(tokens,s,",");
    for (size_t i=0;i<tokens.size();i++){
      int v;
      convertHex(v,tokens[i]);
      x.push_back(expandBits(v,16));
    }
  }

template<typename T>
std::string rmWriteHexList(T& x){
  std::string s;
   for (size_t i=0;i<x.size()-1;i++){
     s+=rmWriteHex(x[i])+",";
   }
   s+=rmWriteHex(x.back());
   return s;
}


  template<typename T>
  void rmReadIntList(T& x,const std::string& s){
    x.clear();
    std::vector<std::string> tokens;
    tokenize(tokens,s,",");
    for (size_t i=0;i<tokens.size();i++){
      int v;
      convert(v,tokens[i]);
      x.push_back(v);
    }
  }

template<typename T>
std::string rmWriteIntList(T& x){
  std::stringstream ss;
  print(",",x,ss);
  return ss.str();
}

template<typename T>
void rmReadBoolList(T& x,const std::string& s){
  for (size_t i=0;i<x.size();i++){
    x[i]=(s[i]=='1');
  }
}


template<typename T>
std::string rmWriteBoolList(const T& x){
  std::string s;
  for (size_t i=0;i<x.size();i++){
    s+=x[i]?"1":"0";
  }
  return s;
}


  RM003File::RM003File(const RMFile& pf):f(pf){
    
  }

  RM003File::PoseLine:: PoseLine(const RMFile::Line& l){

    rmReadInt(time,l[0]);
    rmReadHexList(pos,l[1]);
    rmReadInt(nextPose,l[2]);
    rmReadInt(breakPose,l[3]);
    rmReadString(wavfile,l[4]);
  }





  int getAxisNum(const RMFile& f){
    int r;
    for (size_t i=0;i<f.content.size();i++){
      if (f.content[i].type=="_ENABLE_AXIS"){
	rmReadInt(r,f.content[i][0]);
	return r;
      }
    }
    throw exc::BadFileFormat("Vstone file","","no _ENABLE_AXIS");
  }



  void RM003File::getPoses(std::vector< std::vector<int> >& pos,std::vector<int>& time){
    pos.clear();
    time.clear();

    std::vector<PoseLine> poses;
    int an=getAxisNum(f);

    for (size_t i=0;i<f.content.size();i++){
      if (f.content[i].type=="_POSE"){
	poses.push_back(PoseLine(f.content[i]));
	if ((int)poses.back().pos.size()!=an){
	  throw exc::BadFileFormat("Vstone file","","pose does not have length compatible with _ENABLE_AXIS");
	}
      }
    }

    int nextPose=0;
    Array<bool> toBreak(poses.size());
    setAll(toBreak,false);
    while (nextPose!=-1){     
      //std::cout<<"nextPose="<<nextPose<<std::endl;
      if (nextPose>=(int)poses.size()){
	throw exc::BadFileFormat("Vstone file","","bad nextpose");  
      }
      pos.push_back(poses[nextPose].pos);
      time.push_back(poses[nextPose].time);  
      if (poses[nextPose].breakPose!=-1){
	int n=toBreak[nextPose]?poses[nextPose].breakPose:poses[nextPose].nextPose;
	toBreak[nextPose]=(!toBreak[nextPose]);
	nextPose=n;
      }else{
	nextPose=poses[nextPose].nextPose;
      }
    }

    //std::cout<<"file has "<<pos.size()<<"Poses"<<std::endl;
  }

  
  RM2File::RM2File(RMFile& pf):f(pf){
    
  }


RM2File::PoseLine::PoseLine(const RMFile::Line& l){
    rmReadInt(time,l[0]);
    rmReadHexList(pos,l[1]);
    rmReadString(wavfile,l[2]);
    rmReadBoolList(disable,l[3]);
  }

RM2File::PoseLine::PoseLine(int ptime,
			    const std::vector<int>& ppos,
			    const std::string& pwavfile,
			    const Array<bool,30>& pdisable):
  time(ptime),
  pos(ppos),wavfile(pwavfile),disable(pdisable){}

RM2File::PoseLine::operator RMFile::Line(){
  RMFile::Line l;
  l.type="_POSE";
  l.components.push_back(rmWriteInt(time));
  l.components.push_back(rmWriteHexList(pos));
  l.components.push_back(rmWriteString(wavfile));
  l.components.push_back(rmWriteBoolList(disable));
  return l;

}


  RM2File::PoseLine::PoseLine():time(-1){}


  RM2File::MotionLine::MotionLine(const RMFile::Line& l){
    rmReadString(name,l[0]);
    rmReadInt(x,l[1]);    
    rmReadInt(y,l[2]);
    rmReadInt(nextArrowLength,l[3]);
    rmReadInt(breakArrowLength,l[4]);
    rmReadInt(nextPose,l[5]);
    rmReadInt(breakPose,l[6]);
  }


RM2File::MotionLine::MotionLine(std::string pname,
				int px,int py,int pnextArrowLength, int pbreakArrowLength,
				int pnextPose,int pbreakPose):
  name(pname),x(px),y(py),nextArrowLength(pnextArrowLength),breakArrowLength(pbreakArrowLength),
  nextPose(pnextPose),breakPose(pbreakPose){
}


RM2File::MotionLine::operator RMFile::Line(){
  RMFile::Line l;
  l.type="_MOTION_INFO";
  l.components.push_back(rmWriteString(name));
  l.components.push_back(rmWriteInt(x));
  l.components.push_back(rmWriteInt(y));
  l.components.push_back(rmWriteInt(nextArrowLength));
  l.components.push_back(rmWriteInt(breakArrowLength));
  l.components.push_back(rmWriteInt(nextPose));
  l.components.push_back(rmWriteInt(breakPose));
  return l;
}

  void RM2File::getPoses(std::vector< std::vector<int> >& pos,std::vector<int>& time){
    std::vector<std::string> type;
    std::vector<RM2File::PoseLine> posl;
    std::vector<RM2File::MotionLine> motl;
    int next=-1;

    for (size_t i=0;i<f.content.size();i++){
      
      if (f.content[i].type=="_MOTION_INFO"){
	motl.push_back(RM2File::MotionLine(f.content[i]) );
	type.push_back(f.content[i+1].type);
	if (f.content[i+1].type=="_POSE"){
	  posl.push_back(RM2File::PoseLine(f.content[i+1]));
	}else{
	  //std::cout<<"follows a "<<f.content[i+1].type<<std::endl;
	  if (f.content[i+1].type=="_STARTMOT"){
	    next=posl.size();
	  }
	  posl.push_back(RM2File::PoseLine());
	}
      }     

    }
    if (next==-1){
      throw exc::BadFileFormat("Vstone file","","no STARTMOT");
    }
    
    Array<bool> toBreak(motl.size());
    setAll(toBreak,false);
    while (next!=-1){
      //std::cout<<"next="<<next<<std::endl;
      if (type[next]=="_POSE"){
	pos.push_back(posl[next].pos);
	time.push_back(posl[next].time);
      }

      if (motl[next].breakPose!=-1){
	int n=
	  toBreak[next]?
	  motl[next].breakPose:motl[next].nextPose;
	toBreak[next]=(!toBreak[next]);		
	next=n;
      }else{
	next=motl[next].nextPose;
      }

    }
  } //getPoses


RM2File::SimpleLine::SimpleLine(const std::string& ptype){
  type=ptype;
}

RM2File::SimpleLine::SimpleLine(const std::string& ptype,const std::string& par){
  type=ptype;
  components.push_back(par);
}


void getPoseXY(int&x ,int &y,int i){
  y=46+54*((i%36)+(i/36==0?0:1));
  x=50+100*(i/36);
}

void  RM2File::setPoses(std::vector<std::vector<int> >& pos,std::vector<int>& transit,bool loopy){
  f.content.clear();
  f.content.push_back(SimpleLine("_MOTION_FORMAT"," POSEDEF_V1R4 "));
  f.content.push_back(SimpleLine("_ENABLE_AXIS","62"));
  f.content.push_back(RMFile::Line());
  f.content.push_back(MotionLine("",50,10,6,0,1,-1));
  f.content.push_back(SimpleLine("_STARTMOT"));
  f.content.push_back(RMFile::Line());
  int x,y;
  Array<bool,30> enAll;
  setAll(enAll,false);
  for (size_t i=0;i<pos.size();i++){
    getPoseXY(x,y,i);
    f.content.push_back(MotionLine("POSE"+toString(i),x,y,6,6,i+2,-1));
      f.content.push_back(PoseLine(transit[i],pos[i],"",enAll));
      f.content.push_back(RMFile::Line());
  }
  int n=pos.size();
  if (loopy){
    getPoseXY(x,y,n);
    f.content.push_back(MotionLine("BREAK POINT",x,y,16,46,1,n+2));
    f.content.push_back(SimpleLine("_OLDLOOP"));
    n++;
  }
  getPoseXY(x,y,n);
  f.content.push_back(MotionLine("   END   ",x,y,16,46,-1,-1));
  f.content.push_back(SimpleLine("_ENDMOT"));
  f.write();
};





  RMRomFile::RMRomFile(const RMFile& pf):f(pf){
  }
  
  void RMRomFile::getCalibration(std::vector<int> & calib){
    calib.clear();
    for(size_t i=0;i<f.content.size();i++){
      if (f.content[i].type=="MOTOR_BASE"){
	int x;
	rmReadInt(x,f.content[i][0]);
	calib.push_back(x*2048/15000-1024);
      }
    }
  }

  
  RMDefposFile::RMDefposFile(const RMFile& pf):f(pf){
  }

  void RMDefposFile::getCalibration(std::vector<int>& calib){
    calib.clear();
    int curr=-1;
    for(size_t i=0;i<f.content.size();i++){
      if (f.content[i].type=="MAINDEFPOS_NUM"){
	rmReadInt(curr,f.content[i][0]);
      }
    }
    if (curr==-1) throw exc::BadFileFormat("Vstone file","","no DEFPOS_VALUE");

    int c=-1;
    for(size_t i=0;i<f.content.size();i++){
      if (f.content[i].type=="DEFPOS"){
	c++;
	if (c==curr){
	  if (f.content[i+1].type!="AXISENABLE"||
	      f.content[i+2].type!="SERVOENABLE"||
	      f.content[i+3].type!="DEFPOS_VALUE"
	      ){
	    throw exc::BadFileFormat("Vstone file","","unexpected  DEFPOS content order");
	    
	    

	  }//if content[i+1]
	  
	  std::vector<int> idList,valList;
	  zero(valList);
	  rmReadIntList(idList,f.content[i+1][0]);
	  rmReadHexList(valList,f.content[i+3][0]);
	  int m=getMax(idList);
	  calib.resize(m+1);
	  for (size_t i=0;i<valList.size();i++){
	    calib[idList[i]]=valList[i];//-(valList[i]>>4)+2048;
	  }
	  return ;
	}//if c==curr
      }//if content
    }//for i
    throw exc::BadFileFormat("Vstone file","","cannot find specified DEFPOS content");
  }


  RMAutoPoseFile::RMAutoPoseFile(const char* filename):f(filename,false){

    for (size_t i=0;i<f.content.size();i++){
      if (f.content[i].type=="_MOTION_FORMAT"){	
	std::string format;
	rmReadUnescapedString(format,f.content[i][0]);
	if (format=="POSEDEF_V1R4"){
	  pf.reset(new RM2File(f));
	  //std::cout<<"RobovieMaker 2 Format"<<std::endl;
	}else if (format=="POSEDEF_WORD_N"){
	  pf.reset(new RM003File(f));	 
	  //std::cout<<"RobovieMaker for VSRC003 Format"<<std::endl;
	}else{
	  throw exc::BadFileFormat("Vstone file",filename,"does not contain poses in a known format");
	}
	return;
      }
    }
    throw exc::BadFileFormat("Vstone file",filename,"no format specified");
  }
  

  void RMAutoPoseFile::getPoses(std::vector<std::vector<int> >& pos,std::vector<int>& transit){
    pf.get()->getPoses(pos,transit);
  }


  RMAutoCalibFile::RMAutoCalibFile(const char* filename):f(filename,false){    
     for (size_t i=0;i<f.content.size();i++){
       if (f.content[i].type=="MOTOR_BASE"){
	 cf.reset(new  RMRomFile(f));
	 break;
       }else if (f.content[i].type=="DEFPOS_VALUE"){
	 cf.reset(new RMDefposFile(f));
	 break;
       }
       
     }
  }

  void RMAutoCalibFile::getCalibration(std::vector<int>& calib){
    cf.get()->getCalibration(calib);
  }


  


}//helium
