#ifndef HE_ROBOVIEMAKERFILE
#define HE_ROBOVIEMAKERFILE

#include <helium/util/ioUtil.h>
#include <fstream>
#include <memory>

namespace helium{
  
  class UnicodeFile{
  protected:
    std::fstream f;
  public:
    UnicodeFile(const char* filename,bool out);
    UnicodeFile& operator<<(const std::string& s);

    template<typename T> UnicodeFile& operator<<(const T& t){
      //std::cout<<"calling"<<std::endl;
      *this<<((const std::string&)toString(t));
      return *this;
    }
    
    bool getline(std::string& s);
    
  };

  

  class RMFile:public UnicodeFile{
    std::string filename;
  public:
    class Line{
    public:
      std::string type;
      std::vector<std::string> components;
      Line(){}
      Line(const std::string& data);
      const std::string& operator[] (size_t f) const;
    };
    std::vector<Line> content;
    RMFile(const char* filename,bool out);
    void write();
    
  };

  
  class RMPoseFile{
  public:
    virtual void getPoses(std::vector<std::vector<int> >& pos,std::vector<int>& transit)=0;
    virtual ~RMPoseFile(){}
  };

  class RM003File:public RMPoseFile{
    const RMFile& f;
    class PoseLine{
    public:
      int time;
      std::vector<int> pos;
      int nextPose;
      int breakPose;
      std::string wavfile;
      PoseLine(const RMFile::Line& s);
    };


  public:
    RM003File(const RMFile& f);
    void getPoses(std::vector<std::vector<int> >& pos,std::vector<int>& transit);
  };
  


  class RM2File:public RMPoseFile{
    RMFile& f;
  public:
    struct PoseLine{
      int time;
      std::vector<int> pos;
      std::string wavfile;
      Array<bool,30> disable;
      PoseLine();
      PoseLine(const RMFile::Line& s);
      PoseLine(int time,
	       const std::vector<int>& pos,
	       const std::string& wavfile,
	       const Array<bool,30>& enable);

      operator RMFile::Line();
    };

    struct MotionLine{
      std::string name;
      int x,y,nextArrowLength,breakArrowLength;
      int nextPose,breakPose;    
      MotionLine(const RMFile::Line& s);
      MotionLine(std::string name,
		 int x,int y,int nextArrowLength, int breakArrowLength,
		 int nextPose,int breakPose);    
      operator RMFile::Line();
    };

    class SimpleLine:public RMFile::Line{
    public:
      SimpleLine(const std::string& type);
      SimpleLine(const std::string& type,const std::string& par);
    };


    RM2File(RMFile& f);   
    void getPoses(std::vector<std::vector<int> >& pos,std::vector<int>& transit) ;
    void setPoses(std::vector<std::vector<int> >& pos,std::vector<int>& transit,bool asLoop) ;

  };


  class RMCalibFile{
  public:
    virtual void getCalibration(std::vector<int> & calib)=0;
    virtual ~RMCalibFile(){}
  };

  class RMRomFile:public RMCalibFile{
    const RMFile& f;
  public:
    RMRomFile(const RMFile& f);
    void getCalibration(std::vector<int> & calib) ;
  };

  class RMDefposFile:public RMCalibFile{
    const RMFile& f;
  public:
    RMDefposFile(const RMFile& f);
    void getCalibration(std::vector<int>& calib); 
  };


  class RMAutoPoseFile:public RMPoseFile{
    RMFile f;
    std::auto_ptr<RMPoseFile> pf;
  public:
    RMAutoPoseFile(const char* filename);
    void getPoses(std::vector<std::vector<int> >& pos,std::vector<int>& transit) ;
  };


  class RMAutoCalibFile:public RMCalibFile{
    RMFile f;
    std::auto_ptr<RMCalibFile> cf;
  public:
    RMAutoCalibFile(const char* filename);
    void getCalibration(std::vector<int>& calib); 
  };

  class RMOutputFile:public RMFile,public RM2File{
  public:
    RMOutputFile(const char* filename);
    using RM2File::setPoses;
  };





}//helium




#endif
