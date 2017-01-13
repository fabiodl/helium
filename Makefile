# CROSSCOMPILE_WIN32= 1

SOURCES =main.cpp megankro.cpp $(HELIUM)/os/usbHid.cpp  $(HELIUM)/math/crc.cpp $(HELIUM)/hiddevice/crcException.cpp  $(HELIUM)/core/exception.cpp $(THREAD) $(HELIUM)/os/time.cpp $(HELIUM)/util/printHex.cpp  $(HELIUM)/debug/trace.cpp  $(HELIUM)/util/mout.cpp

NO_DEFAULT_OBJECTS = 1
PROGS=util/megankroset
USEUSB = 1
include ../../Makehel
