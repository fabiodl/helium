#Release_flags s

# $(HELIUM)/robot/motorSpooler.cpp 

VSSV410 = $(VSERIAL) $(HELIUM)/vstone/vServoSign.cpp $(HELIUM)/vstone/vServo.cpp
DYNAMIXELMX28 = $(DYNAMIXEL)
BBTOUCH= $(HELIUM)/hiddevice/hidBusyBitTouchSensor.cpp  $(HELIUM)/os/hidRawException.cpp
ANDROID= $(HELIUM)/android/androidMotor.cpp $(HELIUM)/android/androidMotorSign.cpp

SOURCES = slayer.cpp slayerMain.cpp hwComponent.cpp $(HELIUM)/thread/exitHandler.cpp  $(HW) $(MOTOR) $(SENSOR) $(APPLICATION) $(SYSTEM) $(NET) $(TINYXML)  $(VSSV410) $(DYNAMIXELMX28) $(BBTOUCH) $(ANDROID)


USEUSBRC003= 1
USESERIALRC003= 1
#USEHIDLIB= 1
PROGS=papp/slayer

include ../../Makehel