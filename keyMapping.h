#ifndef _KEYMAPPING_H_
#define _KEYMAPPING_H_

#include <stdint.h>
#include <string>

enum KeyType{KEY_PAD1,KEY_PAD2,KEY_EXTRA};

void getKeySequence(uint8_t* keyConfig,KeyType type ,const char* fname);
void checkDuplicates(uint8_t* keyConfig);
int getOffset(KeyType type,const char* name);

int getIndex(const std::string& s,const char** dict,size_t dictLength,const std::string& dictName);

extern const char* extraKeys[];

void listKeys();
void listPadKeys();
void listExtraKeys();

#endif
