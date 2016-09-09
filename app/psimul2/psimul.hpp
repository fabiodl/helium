
template<typename MotorSign> void PSimul::addSimulatedMotSpooling( std::auto_ptr<helium::SimulMotorAdapter<helium::ODERobot,MotorSign> > &miface,
								   std::auto_ptr<helium::MotorSpooler<MotorSign> > &ms,
								   MotorModes<MotorSign>& modes,
								   helium::ODEWorld* w,
								   helium::ODERobot* r,
								   double* motData,
								   int n,
								   typename helium::SimulPropNormalizer<helium::ODERobot,MotorSign>::ParamPtr prop,
								   typename helium::SimulPosNormalizer<helium::ODERobot,MotorSign>::ParamPtr pos
){
  

  miface.reset(new helium::SimulMotorAdapter<helium::ODERobot,MotorSign>(r,motData,n,prop,pos));
  ms.reset(new helium::MotorSpooler<MotorSign>(n,false));
  ms->interfaces.do_registerAll(miface.get());   
  ms->start();
  ms->setInterface(helium::motor::IfaceSelection(helium::motor::IFACELEVEL,miface->getIfaceId()));
  ms->setInterface(helium::motor::IfaceSelection(helium::motor::RAWLEVEL,miface->getIfaceId()));
  ms->setInterface(helium::motor::IfaceSelection(helium::motor::HWLEVEL,miface->getIfaceId()));
  
   if (isMotorModeSet(OFFMOTORMODE)){
    modes.off.reset(new helium::MotorOffMode<MotorSign>());
    ms->registerMode(modes.off.get());    
  }

   if (isMotorModeSet(LINEARMOTORMODE|FRAMEPLAYMOTORMODE)){
     modes.tsrc.reset(new helium::SecondRefTimeSource(w->t));
  }
   

   if (isMotorModeSet(LINEARMOTORMODE)){
     modes.mlm.reset(new helium::MotorLinearMode<MotorSign,helium::motor::IFACELEVEL>(*ms,*modes.tsrc));
    ms->registerMode(modes.mlm.get());    
  }

   if (isMotorModeSet(FRAMEPLAYMOTORMODE)){
     modes.mfp.reset(new helium::MotorFramePlayMode<MotorSign>(*ms,*modes.tsrc));
    ms->registerMode(modes.mfp.get());    
  }


    if (isMotorModeSet(RAWLINEARMOTORMODE)){
      modes.rmlm.reset(new helium::MotorNotifiedLinearMode<MotorSign,helium::motor::RAWLEVEL>(*ms,*modes.tsrc));
      ms->registerMode(modes.rmlm.get());    
  }

   switch(defaultMode){
   case OFFMOTORMODE:
     ms->setMotorMode(modes.off->getMotorModeId());
     break;
   case LINEARMOTORMODE:
     ms->setMotorMode(modes.mlm->getMotorModeId());
     break;
   case FRAMEPLAYMOTORMODE:
     ms->setMotorMode(modes.mfp->getMotorModeId());
     break;
   case RAWLINEARMOTORMODE:
     ms->setMotorMode(modes.rmlm->getMotorModeId());
     break;
   }

    if (isSet(STARTSERVER)){

      dexport(mem,*ms,"robot");//motorSpooler
      if (isMotorModeSet(LINEARMOTORMODE)){
	dexport(mem,*modes.mlm,"robot");
      }
      if (isMotorModeSet(FRAMEPLAYMOTORMODE)){
	dexport(mem,*modes.mfp,"robot");
      }
      if (isMotorModeSet(RAWLINEARMOTORMODE)){
	dexport(mem,*modes.rmlm,"robot");
      }

    }
   

}
