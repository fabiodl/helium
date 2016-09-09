#ifndef HE_BIGLOG
#define HE_BIGLOG



template<int Val,int search=0,bool isSame=val==(1<<search)> struct bitLog;


struct bitLog<int Val,int search,true>{
  static const int value=search;
}

struct bitLog<int Val,int search,true>{
  static const int value=bitLog<Val,search+1>;
}



#endif
