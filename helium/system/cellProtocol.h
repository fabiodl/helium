/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CELLPROTOCOL
#define HE_CELLPROTOCOL

namespace helium{

  static const char ALIASCHAR='@';
  static const char IDCHAR='*';
  static const char COMMANDTERMINATORCHAR=';';
  static const char* PATHSEPARATORCHAR=".";
  static const char* TREEDESCRIPTIONLOCATION="tree.machine";
   
  enum MessageType {GETVALUEMSG,GETVALUEPARAMMSG,
		    SETVALUEMSG,SETVALUEPARAMMSG,
		    CONNECTMSG,CONNECTPARAMMSG,
		    DISCONNECTMSG,DISCONNECTPARAMMSG,
		    UPDATEMSG,ERRORMSG,WELCOMEMSG,MESSAGETYPES};

  static const char requestChar[]={'>','}','<','{','+','^','-','|','_','_','_'};  
  static const char replyChar[]={'=','#','_','$','c','C','d','D','%','!','~'};

  enum MachineDescription{
    MDTRANSPARENT,MDNONTRANSPARENT,
    MDHASCELL,MDNOCELL,
    MDPUSHID,MDPOPID
  };

  static const char mdescChar[]={'/','*','@','#','+','-'};
  
  const int CELLPORT=15001;
  
  inline void noCompilerWarningCell(){
    char c=requestChar[0];
    c+=replyChar[0];
    c+=PATHSEPARATORCHAR[0];
    c+=TREEDESCRIPTIONLOCATION[0];
  }
}

#endif
