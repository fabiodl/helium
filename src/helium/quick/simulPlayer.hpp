namespace helium{


  template<typename MotHwSign>
  SimulPlayerMotors<MotHwSign>::Options::Options():
    prop(NULL),
    initMode(motorModeSet::LINEARMOTORMODE)
  {
    
  };


  template<typename MotHwSign>
  SimulPlayer<MotHwSign>::Options::Options():    
    touchEveryN(10),
    rototraEveryN(10)
  {
    
  }

    class SingleGyroDescription:public helium::Description<helium::sensor::SingleCellAdapter<int,double> >{
  public:
    SingleGyroDescription(helium::sensor::SingleCellAdapter<int,double>& s):helium::Description<helium::sensor::SingleCellAdapter<int,double> >(s){
    }    

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(vectMapping(this->obj.cells,"sensor","value","num"));
    }
    
    
  };


  template<typename MotHwSign>
  SimulPlayerMotors<MotHwSign>::
  SimulPlayerMotors(ODERobot* r,	      
	      typename motor::Raw<MotHwSign>::Hardware& motors,
	      const Options& opts):
    motorIface(motors),
    ms(motors.getMotorNum(),true),
    timesrc(r->getWorld()->t),
    modes(ms,timesrc),
    meh(ms,motorIface),
    motPropMan(ms),
    motNormalIdle(ms,MotSpoolPriority::NORMAL,200),
    motHighIdle(ms,MotSpoolPriority::NORMAL,200){
    ms.interfaces.registerAll(&motorIface);   
    motor::IfaceId mifid=motorIface.getIfaceId();
    ms.setInterface(motor::IfaceSelection(motor::IFACELEVEL,mifid));
    ms.setInterface(motor::IfaceSelection(motor::RAWLEVEL,mifid));
    ms.setInterface(motor::IfaceSelection(motor::HWLEVEL,mifid));
    ms.setMotorMode(modes.getMotorMode(opts.initMode).getMotorModeId());
  }
    
    



  template<typename MotHwSign>
  SimulPlayer<MotHwSign>::
  SimulPlayer(ODERobot* r,	      
	      typename motor::Raw<MotHwSign>::Hardware& motors,
	      //motor::Calib<MotHwSign>& motCalib,
	      SimulWin& win,
	      const Array<double,array::RESIZABLE,array::LINK>& mouseSensor,	      
	      const Array<double,array::RESIZABLE,array::LINK>& forceScalar,	
	      sensor::Calib<int>& touchCalib,
	      SensorListMaxComputation& touchMapper,
	      GyroCalib& gyroCalib,
	      const Options& opts):    
    SimulPlayerMotors<MotHwSign>(r,motors,opts.motorOpts),    
    mouseSimulSensor(mouseSensor.data,mouseSensor.size(),r->getSrcRobot()),
    touchIface(forceScalar.data,touchMapper.getSimulSize(),opts.touchEveryN),
    touchSs(touchIface),
    touchSp(touchSs),
    combfc(mouseSimulSensor),
    denormfc(touchCalib),
    touchCombineFilter(combfc,touchMapper.getSimulSize(),touchSp,touchMapper.getRealSize()),
    touchSortFilter(touchMapper,touchMapper.getRealSize(),touchCombineFilter,touchMapper.getSimulSize()),
    touchDenormFilter(denormfc,touchMapper.getRealSize(),touchSortFilter,touchMapper.getRealSize()),
    touchFilteredHw(touchIface,touchMapper),
    touchErrorHandler(touchSs,touchFilteredHw,touchMapper.getRealSize()),
    touchSingleRaws(touchDenormFilter),
    touchNormalIdle(touchSs,SensSpoolPriority::NORMAL,200),
    touchHighIdle(touchSs,SensSpoolPriority::HIGH,300),

    rototraIface(r,opts.rototraEveryN),
    rototraSs(rototraIface),
    rototraSp(rototraSs),
    gyrodenormfc(gyroCalib),
    gyroDenormFilter(gyrodenormfc,5,rototraSp,16),
    singleGyros(gyroDenormFilter)
  {

    win.addKeyboardInteraction(&mouseSimulSensor);
    win.addMouseSensorListener(&mouseSimulSensor);
    r->getWorld()->addCallback(&touchIface);
    r->getWorld()->addCallback(&rototraIface);
   }



  template<typename HwSign> 
  void DefDescr<SimulPlayerMotors<HwSign> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(dMapping(this->obj.modes));
    members.push_back(dMapping(this->obj.meh));
    members.push_back(dMapping(this->obj.motPropMan));
    members.push_back(dMapping(this->obj.ms));    
  }


    
 

  template<typename HwSign> 
  void DefDescr<SimulPlayer<HwSign> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(static_cast<SimulPlayerMotors<HwSign>&>(this->obj) ) ));
    members.push_back(scalarMapping(d(this->obj.touchErrorHandler),"sensors"));
    members.push_back(vectMapping(vd(this->obj.touchSingleRaws.cells),IDPath("sensors.sensor","."),"value",IDPath("sensors.num",".")));


    members.push_back(scalarMapping(d(this->obj.touchDenormFilter),IDPath("sensors.group.value",".")));
    members.push_back(scalarMapping(this->obj.gyroDenormFilter,IDPath("gyro.group.value",".")));
    members.push_back(scalarMapping(this->template cd<SingleGyroDescription>(this->obj.singleGyros),"gyro"));
    
  }



}//ns he 
