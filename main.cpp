#include <map>
#include <helium/os/usbHid.h>
#include <stdlib.h>
#include "megankro.h"

static const char* keys[256]={
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
  /*28*/"enter","escape","delete","tab",
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
  /*84*/"lockingscrolllock","keypad_comma","keypad_equal","international1",
  /*88*/"international2","international3","international4","international5",
  /*8C*/"international6","international7","international8","international9",
  /*90*/"lang1","lang2","lang3","lang4",
  /*94*/"lang5","lang6","lang7","lang8",
  /*98*/"lang9","erase","sysreq","cancel",  
  /*9C*/"clear","prior","return","separator",
  /*A0*/"out","oper","again","crsel",  
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


int main(int argc,char** argv){

  for (int i=0;i<256;i++){
    //std::cout<<i<<":"<<keys[i]<<std::endl;
    if (equals(keys[i],argv[1])){
      std::cout<<keys[i]<<std::endl;
    }
  }

  
  
  if (argc<static_cast<int>(MegaNkro::NUMKEYS+1)){
    std::cout<<"Usage "<<argv[0]<<" followed by "<<MegaNkro::NUMKEYS<<" hex values, according to Universal Serial Bus HID Usage Tables, Table 12: Keyboard/Keypad Page "<<std::endl;
    return 0;
  }


  
 
  uint8_t keys[MegaNkro::NUMKEYS];
  for (size_t i=0;i<MegaNkro::NUMKEYS;i++){
    keys[i]=static_cast<uint8_t>(strtol(argv[i+1],NULL,16));
    std::cout<<"key "<<(int)keys[i]<<std::endl;
  }
  MegaNkro nkro;
  nkro.set(keys);


}
