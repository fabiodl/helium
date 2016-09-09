CPPFLAGS = -I /usr/include/opencv-2.3.1/ -Wall -I ../../ -O2


all:  stereoBlobClient stereoBlobServer

OBJ= stereoBlobServer.o cvCamera.o stereoBlob.o cvWindow.o

stereoBlobServer: $(OBJ)
	g++ -o stereoBlobServer $(OBJ) -lHelium -lGL -lGLU -lglut -llapack -lopencv_core -lopencv_highgui -lpthread

SCOBJ= stereoBlob.o stereoBlobClient.o cvWindow.o

stereoBlobClient: $(SCOBJ)
	g++ -o stereoBlobClient $(SCOBJ) -lHelium -lGL -lGLU -lglut -llapack -lpthread

clean: 
	@rm $(OBJ) $(SCOBJ)