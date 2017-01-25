#include <map>
#include <helium/os/usbHid.h>
#include <fstream>
#include <stdlib.h>
#include "megankro.h"

static const int KEYBOARDKEYS=256;
static const char* keyboardKeys[KEYBOARDKEYS]={
  /*00*/ "noevent", "rollover","postfail","undefinedError",  
  /*04*/"a","b","c","d",                                    
  /*08*/"e","f","g","h",                                    
  /*0C*/"i","j","k","l",                                    
  /*10*/"m","n","o","p",                                    
  /*14*/"q","r","s","t",
  /*18*/"u","v","w","x",
  /*1C*/"y","z","1","2",
  /*20*/"3","4","5","6",  
  /*24*/"7","8","9","0",
  /*28*/"enter","escape","backspace","tab",
  /*2C*/"space","minus","equal","openbracket",
  /*30*/"closebracket","backslash","nonus_sharp","semicolon",
  /*34*/"singlequote","backquote","comma","period",
  /*38*/"slash","capslock","f1","f2", 
  /*3C*/"f3","f4","f5","f6",
  /*40*/"f7","f8","f9","f10",
  /*44*/"f11","f12","printscreen","scrolllock",
  /*48*/"pause","insert","home","pageup",
  /*4C*/"delete","end","pagedown","right",
  /*50*/"left","down","up","keypad_numlock",  
  /*54*/"keypad_divide","keypad_multiply","keypad_minus","keypad_plus",
  /*58*/"keypad_enter","keypad_1","keypad_2","keypad_3",
  /*5C*/"keypad_4","keypad_5","keypad_6","keypad_7",
  /*60*/"keypad_8","keypad_9","keypad_0","keypad_dot",
  /*64*/"nonus_backslash","application","power","keypad_equal",
  /*68*/"f13","f14","f15","f16",  
  /*6C*/"f17","f18","f19","f20",
  /*70*/"f21","f22","f23","f24",
  /*74*/"execute","help","menu","select",
  /*78*/"stop","again","undo","cut",  
  /*7C*/"copy","paste","find","mute",
  /*80*/"volumeup","volumedown","lockingcapslock","lockingnumlock",  
  /*84*/"lockingscrolllock","keypad_comma","keypad_equal_as400","international1",
  /*88*/"international2","international3","international4","international5",
  /*8C*/"international6","international7","international8","international9",
  /*90*/"lang1","lang2","lang3","lang4",
  /*94*/"lang5","lang6","lang7","lang8",
  /*98*/"lang9","erase","sysreq","cancel",  
  /*9C*/"clear","prior","return","separator",
  /*A0*/"out","oper","clearagain","crsel",  
  /*A4*/"exsel","#a5","#a6","#a7",
  /*A8*/"#a8","#a9","#aa","#ab",
  /*AC*/"#ac","#ad","#ae","#af",
  /*B0*/ "keypad_00","keypad_000","thousandsseparator","decimalseprator",
  /*B4*/"currencyunit","currencysubunit","keypad_openparenthesis","keypad_closeparenthesis",
  /*B8*/  "keypad_openbrace","keypad_closebrace","keypad_tab","keypad_backspace",
  /*BC*/"keypad_a","keypad_b","keypad_c","keypad_d",
  /*C0*/"keypad_e","keypad_f","keypad_xor","keypad_caret",
  /*C4*/"keypad_percent","keypad_lessthan","keypad_greaterthan","keypad_and",
  /*C8*/ "keypad_andand","keypad_or","keypad_oror","keypad_colon",
  /*CC*/"keypad_sharp","keypad_space","keypad_at","keypad_exclamationmark",
  /*D0*/ "keypad_memorystore","keypad_memoryrecall","keypad_memoryclear","keypadd_memoryadd",
  /*D4*/"keypad_memorysubtract","keypad_memorymultiply","keypad_memorydivide","keypad_plusminus",
  /*D8*/"keypad_clear","keypad_clearentry","keypad_binary","keypad_octal",
  /*DC*/"keypad_decimal","keypad_hexadecimal","#de","#df",
  /*E0*/"leftcontrol","leftshift","leftalt","leftgui",  
  /*E4*/"rightcontrol","rightshift","rightalt","rightgui",
  /*E8*/"#e8","#e9","#ea","#eb",
  /*EC*/"#ec","#ed","#ee","#ef",
  /*F0*/"#f0","#f1","#f2","#f3",  
  /*F4*/"#f4","#f5","#f6","#f7",  
  /*F8*/"#f8","#f9","#fa","#fb",    
  /*FC*/"#fc","#fd","#fe","#ff"
};


static const int PADS=2;
static const int PADBUTTONS=12;
static const char* padKeys[PADBUTTONS]={
"up","down","left","right",
  "a","b","c","start",
  "x","y","z","mode"
};




static const int btnDestination[PADS][PADBUTTONS]={

  {
  0,1,2,3,
  6,4,5,7,
  18,17,16,19
  },

  {
  8,9,10,11,
  14,12,13,15,
  22,21,20,23
    }

}  ;






bool equals(const char* s1,const char* s2){
  for (;*s1!=0&&*s2!=0;s1++,s2++){
    while(*s1=='_'){s1++;}
    while(*s2=='_'){s2++;}
    if (toupper(*s1)!=toupper(*s2)){
      return false;
    }
  }
  return !(*s1||*s2);
}

int getKey(const std::string& s,const char** keys,const std::string& dictName){
  for (int i=0;i<256;i++){
    if (equals(keys[i],s.c_str())){
      return i;
    }
  }
  throw helium::exc::InvalidDataValue(dictName,s);
}


void sanityCheck(){
  for (int i=0;i<256;i++){
      for (int j=0;j<256;j++){
        if (i==j) continue;
        if (equals(keyboardKeys[i],keyboardKeys[j])){
          std::cout<<"key internal match!!!"<<keyboardKeys[i]<<" "<<keyboardKeys[j]<<std::endl;
        }
      }
  }

}


void getKeySequence(uint8_t* outs,const int* destination,const char* fname){
  std::ifstream f(fname);
  std::map<int,int> mapping;
  while(!f.eof()){
    std::string pad,keyboard;
    f>>pad>>keyboard;
    if (!f) break;
    int padKey=getKey(pad,padKeys,"pad key");
    int keyboardKey=getKey(keyboard,keyboardKeys,"keyboard key");
    if (mapping.find(padKey)!=mapping.end()){
      throw helium::exc::DuplicateID(pad+" button assigned to multiple keys in file "+fname);
    }
    mapping[padKey]=keyboardKey;
  }
  for (int i=0;i<PADBUTTONS;i++){
    if (mapping.find(i)==mapping.end()){
      throw helium::exc::MissingData(std::string(padKeys[i])+" button in file "+fname);
    }
    outs[destination[i]]=mapping[i];
  }  
}


std::string describeButton(int btnId){

  for (int p=0;p<PADS;p++){
    for (int b=0;b<PADBUTTONS;b++){
      if (btnDestination[p][b]==btnId){
        std::stringstream ss;
        ss<<"pad "<<(p+1)<<" button "<<padKeys[b];
        return ss.str();
      }
    }
  }
  return "";//should not reach here
}

void checkDuplicates(uint8_t* padbuttons){
  for (int i=0;i<2*PADBUTTONS;i++){
    for (int j=0;j<2*PADBUTTONS;j++){
      if (i==j) continue;
      if (padbuttons[i]==padbuttons[j]){
        throw helium::exc::DuplicateID(std::string("key ")+keyboardKeys[padbuttons[i]]+" associated to both "+
                                       describeButton(i)+" and "+describeButton(j));
                                       
      }

    }
  }
}


int main(int argc,char** argv){


  
  if (argc<PADS+1){
    std::cout<<"Usage "<<argv[0]<<" followed by two filenames."<<std::endl<<
      "each filename has 12 lines, with the format <pad button><space><keyboard key>"<<std::endl<<
      "AVAILABLE PAD BUTTONS"<<std::endl;
    
    for (int i=0;i<PADBUTTONS;i++){
      std::cout<<" "<<padKeys[i]<<std::endl;
    }
    std::cout<<"AVAILABLE KEYBOARD KEYS"<<std::endl;
    for (int i=0;i<KEYBOARDKEYS;i++){
      char buffer[3];
      sprintf(buffer,"%02x ",i);
      std::cout<<" "<<buffer<<keyboardKeys[i]<<std::endl;
    }      
    return 0;
  }


  uint8_t keys[PADS*PADBUTTONS];
  for (int i=0;i<PADS;i++){
    getKeySequence(keys,btnDestination[i],argv[i+1]);
  }
  checkDuplicates(keys);

 
  MegaNkro nkro;
  nkro.set(keys);


}
