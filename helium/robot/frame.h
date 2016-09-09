/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags ge
 */

#ifndef HE_FRAME
#define HE_FRAME

#include <map>
#include <set>


#include <helium/vector/array.h>
#include <helium/vector/operations.h>
#include <helium/core/time.h>
#include <helium/smartpointer/ndref.h>
#include <helium/robot/motionParameter.h>


namespace helium {

  enum FrameValueType {VALUEFRAME, INTERPOLATEFRAME}; 


  typedef std::pair<FrameValueType,double> FrameValue;



  class Posture:public Array<double> { /// a posture consists of only double
  public:
    std::string name;
    Posture(int i):Array<double>(i),name(){ 
      helium::zero(*this);
    }

    Posture(const Posture& p);
    Posture(const Posture& p,std::string n);
    Posture(int i,std::string n):Array<double>(i),name(n){ 
      //std::cout<<"new posture s @"<<this<<std::endl;
    }
    Posture():name(""){//std::cout<<"new posture @"<<this<<std::endl; 
    }
    ~Posture(){//std::cout<<"posture @"<<this<<" gone "<<std::endl;
    }
    Posture(helium::Array<helium::FrameValue> frameValue);
    void getFrameValue(helium::Array<helium::FrameValue> &frameValue) const;
  };



  //void toDeg(Posture& dst,const Posture& src);

  class Motion{
    Posture NANPosture;
    int n;
  public:
    std::string robot; //robot name
    const Posture& getNanPosture(){return NANPosture;}

    typedef std::map<Time,NDRef<Posture> >  MotionMap;
    MotionMap motion;  


    //enum MotionAction {INSERTFRAME,MODIFYFRAME,DELETEFRAME,LINKFRAME,UNLINKFRAME,CLEARFRAME};
    //Signal<std::pair<MotionAction> > motionChanged; //signals the "Time of interest", e.g. the time to go to with an undo
    //Motion(MotionParameter &mp);

    Motion(size_t n);    
    Motion(const Motion& m);/////////< 20140714 uncommented, needed to copy motion in guideHistory //20131230 commented
    //Motion(const MotionExport &me);

    /** only for initialization (in guideLogic)
     * DO NOT ever call this unless the robot setting is changed!
     * @param n joint size
     */
    void setJointSize(size_t n); 
    int getJointSize() const;

    /** only for initialization (in guideLogic)
     * @param r robot name
     */
    void setRobotName(const std::string &r);
    std::string getRobotName() const;

    const MotionMap& getMotion() const{
      return motion;
    }

    Motion& operator=(const Motion& m);
   
    const Posture& operator[](Time t) const{ 
      return  *(motion.find(t)->second);
    }
    Posture& operator[](Time t){ 
      return *  (motion.find(t)->second); 
    }

    const Posture& getPosture(Time t) const;
    void getPosture(Time t, Posture &p) const;
    double getAngle(int id,Time t) const { return ( motion.find(t))->second.getData().data[id]; }
    bool isLinked(helium::Time t) const; //this means its shares the Posture with someone else
    bool isFrame(Time t) const;
    
    void getLink(std::set<helium::Time> &link,helium::Time t) const;
    int getSize() const { return motion.size(); }

    Time getNextTime(Time t, int jointId=-1) const; 
    Time getPrevTime(Time t, int jointId=-1) const;
    Time getEndTime() const ;

    //Time getStartFrame() const;
    bool isEmpty() const;


    void insertShiftFrame(Time t,Time dt,const Posture &p);
    void insertFrame(Time t);
    void insertFrame(Time t,const NDRef<Posture> &p);
    void insertFrame(Time t,const Posture &p);
    void insertFrame(Time t,const Posture &p,std::string name);
    void modifyFrame(Time t,int i, double d);

    void insertZeroFrame();
    //void modifyFrame(Time t,const Posture &p);
    void deleteFrame(Time t);
    void linkFrame(Time from, Time to);
    void unlinkFrame(Time t);
    void clear();
    void adjustTime(Time t, Time dt);
    void replace(const Motion &m);
    void rename(Time t,std::string s);
  
    
  };

  std::ostream& operator<<(std::ostream& o,const helium::Motion& m);
  
  std::istream& operator>>(std::istream& i,helium::Motion& m);


  //std::istream& operator>>(std::istream& i,helium::Posture& p);  



}



#endif
  //is not a member of posture
    //friend std::ostream & operator<<(std::ostream &o,const Posture &p) {
      /*o << " ";
      for(int i=0;i<p.N;i++) 
      o << p.posture.data[i] << " ";*/
    //o<<*(static_cast<Array<double>(*this));
    //return o;
    //}
    //todo: to be replaced by tree
    //int N;

    //Posture(int n);
    //Posture(Array<double> p);
    /*void reset();
    double& operator [] (int i) const{
      return posture.data[i];
    }
    Posture& operator = (const double &d) {
      for(int i=0;i<N;i++)
	posture.data[i] = d;
      return *this;
    }
    Posture& operator = (const Posture &p) {
      for(int i=0;i<N;i++)
	posture.data[i] = p.posture.data[i];
      return *this;
    }
    bool operator != (Posture& p){
      for(int i=0;i<N;i++) 
	if(posture.data[i] != p.posture.data[i])
	  return true;
      return false;
    }
    */

    //public: //debug
    //Array<double> posture;
