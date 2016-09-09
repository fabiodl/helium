/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.08.02
 * \version 0.1
 * Release_flags s
 */

namespace helium{

  template <typename T> 
  GenericIDPath::GenericIDPath(const T &t):ups(NORMAL){    
    downPath(t);
  }



  template <typename T> 
  IDNode<T>* IDNode<T>::getParent() const{
    return parent;
  }
  
  template <typename T> 
  const std::vector<IDNode<T>*> & IDNode<T>::getChildren() const{
    return sons;
  }
  
  template <typename T> 
  IDNode<T>* IDNode<T>::getChild(int i) const{
    return sons[i];
  }

  template <typename T> 
  size_t IDNode<T>::childrenNum() const{
    return sons.size();
  }

  template <typename T> 
  const  std::vector<Match>& IDNode<T>::getAliasesMatches() const{
    return aliases;
  }


  template <typename T> 
  void IDNode<T>::getMainAlias(std::string &s) const{
    if (!aliases.size()) throw exc::MissingData("alias in root");
    s=aliases[0].s;
  }
  
  template <typename T> 
  const std::string& IDNode<T>::getMainAlias() const{
     if (!aliases.size()) throw exc::MissingData("alias in root");
    return aliases[0].s;
  }

  template <typename T> 
  const std::string& IDNode<T>::getAlias(int i) const{
    return aliases[i].s;
  }

  template <typename T> 
  IDNode<T>::IDNode(IDNode<T>* pparent):transparent(false),data(NULL),parent(pparent){
  
  }

  template <typename T> 
  IDNode<T>::~IDNode(){ 
    for (size_t i=0;i<sons.size();i++){
      delete sons[i];
    }
  }
  

  template <typename T> 
  IDNode<T>* IDNode<T>::clone(IDNode<T>* pparent){
    IDNode* n=new IDNode(pparent);
    n->aliases=aliases;
    n->transparent=transparent;
    n->data=data;
    for (int i=0;i<sons.size();i++){
      n->sons.push_back(sons[i]->clone(n));
    }
  }

  template <typename T> 
  void IDNode<T>::getLocalPath(IDPath &p) const{
    if (parent!=NULL){
      parent->getLocalPath(p);
      std::string a;
      getMainAlias(a);
      p.downPath(a);
    }
  }//getLocalPath


  template <typename T> 
  void IDNode<T>::merge(IDNode* b){
    for (size_t i=0;i<b->aliases.size();i++){
      addAlias(b->aliases[i].s);
    }
    if (b->transparent) setTransparent(true);
    if (b->data!=NULL){     
      if (data!=NULL&&*b->data!=*data){
	IDPath p;     
	getLocalPath(p);      
	throw exc::Bug("merging "+toString(p)+" would cause data overwrite.");
      }
      data=b->data;
    }
    for (size_t i=0;i<b->sons.size();i++){
      std::string s;
      b->sons[i]->getMainAlias(s);
      assurePath(s)->merge(b->sons[i]);
    }
  }


  template <typename T> 
  void IDNode<T>::destroyLinkedData(const std::vector<const T*> &toKeep){
    bool canDestroy=(data!=NULL);
    for (size_t i=0;canDestroy&&i<toKeep.size();i++){
      if (data==toKeep[i]) canDestroy=false;
    }
    if (canDestroy) delete data;
    for (size_t i=0;i<sons.size();i++){
      sons[i]->destroyLinkedData(toKeep);
    }
  }


  template <typename T> 
  bool IDNode<T>::hasData(){
    return data!=NULL;
  }


  template <typename T> 
  T* IDNode<T>::getDataP(){
    return data;
  }


  template <typename T> 
  const T* IDNode<T>::getDataP() const{
    return data;
  }

  template <typename T> 
  IDNode<T>* IDNode<T>::setData(T* t){
    if (t!=NULL) data=t;
    return this;
  }

  /*const std::vector<IDNode*> & getSons(){
    return sons;
    } */

  template <typename T> 
  void IDNode<T>::addPartialMatches(std::vector<std::string> &matches,const std::string &s){
    for (size_t i=0;i<aliases.size();i++){
      if (s.size()>aliases[i].s.size()) continue;
      if (isHeadSubstring(s,aliases[i].s)){
	matches.push_back(aliases[i].s);
      }
    }//for
  }

  template <typename T> 
  void IDNode<T>::getAliases(std::vector<std::string> &alias){
    for (size_t i=0;i<aliases.size();i++){
      alias.push_back(aliases[i].s);
    }
  }


  template <typename T> 
  IDNode<T>* IDNode<T>::canEnter(size_t &a,const std::string &s){
    for (a=0;a<aliases.size();a++){
      if ((int)s.length()<aliases[a].minmatch||s.size()>aliases[a].s.length()) continue;
      if (isHeadSubstring(s,aliases[a].s)) return this;
    }
    if (transparent){
      for (size_t j=0;j<sons.size();j++){
	if (sons[j]->canEnter(a,s)) return sons[j];
      }//for
    }
    a=-1;
    return NULL;
  }


  template <typename T> 
  IDNode<T>* IDNode<T>::downHop(size_t& a,const std::string &s) const{
    for (size_t i=0;i<sons.size();i++){
      IDNode* n=sons[i]->canEnter(a,s);
      if (n!=NULL) return n;
    }
    return NULL;
  }
  
  
  template <typename T> 
  IDNode<T>* IDNode<T>::downSurePath(const IDPath &p){
    IDNode* m=this;    
    size_t a;
    for (size_t i=0;i<p.size();i++){
      m=m->downHop(a,p[i]);
    }
    return m;
  }


  template <typename T> 
  IDNode<T>* IDNode<T>::downMaximumPath(size_t& matched,const IDPath &p){
    IDNode* m=this;    
    IDNode* n;
    size_t a;
    for (matched=0;matched<p.size();matched++){
      n=m->downHop(a,p[(int)matched]);
      if (n==NULL) 
	break;
      m=n;
    }
    return m;
  }

  /*IDNode* downMaximumPathUpToParent(size_t& matched,const IDPath &p){
    IDNode* m=this;    
    IDNode* n;
    size_t a;
    for (matched=0;matched+1<p.size();matched++){
      n=m->downHop(a,p[(int)matched]);
      if (n==NULL) 
	break;
      m=n;
    }
    return m;
    }*/


  template <typename T> 
  bool IDNode<T>::hasPath(const IDPath &p){
    IDNode* m=this;    
    size_t a; //which son
    for (int i=0;i<(int)p.size();i++){
      m=m->downHop(a,p[i]);
      if (m==NULL) return false;
    }
    return true;
  }

  template <typename T> 
  IDNode<T>* IDNode<T>::downMaximumPathNull(size_t& matched,const IDPath &p) const{
    IDNode* m=const_cast<IDNode*>(this);    
    size_t a;
    for (matched=0;matched<p.size()&&m!=NULL;matched++){
      m=m->downHop(a,p[matched]);
    }
    return m;
  }

  template <typename T> 
  void IDNode<T>::machinePrint(std::ostream& o,void (*printer)(std::ostream& o,const T* t)){
    if (aliases.size()>0){
      o<<(transparent?mdescChar[MDTRANSPARENT]:mdescChar[MDNONTRANSPARENT]);
      o<<" "<<aliases.size()<<" ";
      for (size_t i=0;i<aliases.size();i++){
	o<<aliases[0]<<" ";
      }
      o<<" "<<(data==NULL?mdescChar[MDNOCELL]:mdescChar[MDHASCELL]);
      (*printer)(o,data);
      o<<" ";
    }
    for (size_t i=0;i<sons.size();i++){
      o<<mdescChar[MDPUSHID];
      sons[i]->machinePrint(o,printer);
      o<<mdescChar[MDPOPID];
    }
  }

  
  template <typename T> 
  IDNode<T>* IDNode<T>::addChild(const std::string &s){
    IDNode* n=new IDNode(this);
    sons.push_back(n);
    n->addAlias(s);    
    return n;
  }


  template <typename T> 
  void IDNode<T>::fixSonsMatches(int i){    
    //reset the sizes
    for (size_t i=0;i<sons.size();++i){
      for (size_t ia=0;ia<sons[i]->aliases.size();++ia){
	sons[i]->aliases[ia].minmatches=0;
      }
    }

    //fix them by pairs
    for (size_t i=0;i<sons.size();++i){
      for (size_t ia=0;ia<sons[i]->aliases.size();++ia){
	
	for (size_t j=i+1;j<sons.size();++j){
	  for (size_t ja=0;ja<sons[j]->aliases.size();++ja){
	    int common=headMatches(sons[i]->aliases[ia].minmatches,sons[j]->aliases[ja].minmatches);
	    sons[i]->aliases[ia].minmatches=
	      (std::max)(sons[i]->aliases[ia].minmatches,common+1);
	    sons[j]->aliases[ja].minmatches=
	      (std::max)(sons[j]->aliases[ja].minmatches,common+1);	   	    	    
	  }//for ja
	}//for j
	sons[i]->aliases[ia]=(std::min)(sons[i]->aliases[ia].length(),sons[i]->aliases[ia].minmatches);
      }//for ia
    }//for i
  }


  template <typename T> 
  void IDNode<T>::unlinkChild(IDNode* n){
    for (size_t i=0;i<sons.size();++i){
      if (sons[i]==n){
	sons.erase(sons.begin()+i);
	fixSonsMatches();
	return;
      }
    }
    throw exc::InvalidOperation("Child not found");
  }

  
  template <typename T> 
  bool IDNode<T>::hasExactAlias(const std::string& s){
    for (size_t i=0;i<aliases.size();i++){
      if (aliases[i].s==s) return true;
    }
    return false;
  }

  
  template <typename T> 
  IDNode<T>* IDNode<T>::assurePath(const IDPath &p){
    if (p.isEmpty()){
      throw exc::InvalidOperation("No need to assure the root");
    }
    size_t m;
    IDNode* n=downMaximumPath(m,p);
    for (;m<p.size();m++){
      n=n->addChild(p[(int)m]);
    }
    if (!n->hasExactAlias(p[int(p.size())-1])){
      n=n->parent->addChild(p[int(p.size())-1]);
    }
    return n;    
  }//assurePath

  template <typename T> 
  bool IDNode<T>::isTransparent() const{
    return transparent;
  }

  template <typename T> 
  void IDNode<T>::setTransparent(bool state){
    transparent=state;
    //sons.push_back(parent);
  }

  
 

  template <typename T> 
  void IDNode<T>::addAlias(const std::string &a){
    //std::cout<<"adding alias "<<a<<std::endl;
    int i=(int)aliases.size();
    for (size_t ac=0;ac<aliases.size();ac++){
      if (aliases[ac].s==a) return;
      //std::cout<<"curr alias"<<aliases[ac].s<<std::endl;
    }
    aliases.push_back(a);
    if (parent==NULL) return;
    for(size_t s=0;s<parent->sons.size();s++){
      if (parent->sons[s]==this) continue;
      IDNode* sibling=parent->sons[s];      
      //std::cout<<"looking brother"<<sibling->getMainAlias()<<std::endl;
      for (size_t j=0;j<sibling->aliases.size();j++){
	if (aliases[i].s==sibling->aliases[j].s){
	  throw exc::DuplicateID(a);
	}
	size_t l=headMatches(a,sibling->aliases[j].s)+1;
	aliases[i].minmatch=(std::max)((int)l,aliases[i].minmatch);//clipped later
	sibling->aliases[j].minmatch=(std::min)((int)sibling->aliases[j].s.length(),
						(std::max)((int)l,sibling->aliases[j].minmatch));
      }//for j
      aliases[i].minmatch=(std::min)((int)aliases[i].s.length(),aliases[i].minmatch);
      
      
    }
    //std::cout<<"return from AddAlias"<<std::endl;
  }


  template <typename T> 
  IDNode<T>* IDNode<T>::downPath(const IDPath &p) const{
    size_t matched;
    IDNode* m=const_cast<IDNode*>(this);    
    IDNode* n;
    size_t a;
    for (matched=0;matched<p.size();matched++){
      n=m->downHop(a,p[(int)matched]);
      if (n==NULL) 
	break;
      m=n;
    }
    if (matched<p.size()){
      std::vector<std::string> matches;
      getMatches(matches,p,this);
      examineProblem(m,p,matches);                   
    }//matched<size()
    return m;
  }//downPath

  template <typename T> 
  T& IDNode<T>::getData() const{
    if (data==NULL){
      IDPath p;
      getLocalPath(p);      
      throw exc::MissingData(toString(p));
    }
    return *data;
  }

  template <typename T> 
  IDNode<T>* IDNode<T>::tracedownPath(std::vector<std::string> &matches, const IDPath &p){
    IDNode* m=this;    
    getMatches(matches,p,m);
    if (matches.size()<p.size()){      
      examineProblem(m,p,matches);                   
    }//matched<size()
    return m;
  }

  template <typename T> 
  void IDNode<T>::print(std::ostream &o,int spaces) const{    
    //throw exc::Bug("stop printing!");
    space(spaces,o);
    o<<"[";
    if (transparent) o<<"(";
    helium::print("|",aliases,o);    
    if (transparent) o<<")";
    o<<"]="<<ptoString(data)<<std::endl;
    for (size_t i=0;i<sons.size();i++){
      sons[i]->print(o,spaces+1);
    }
  }

  template <typename T> 
  void IDNode<T>::getMatches(std::vector<std::string> &matches, const IDPath &p,const IDNode* m) const{
    size_t a;
    IDNode* n;
    for (size_t i=0;i<p.size();i++){
      n=const_cast<IDNode*>(m)->downHop(a,p[(int)i]);
	if (n==NULL) break;
	m=n;
	if (m->transparent){
	  matches.push_back("("+m->aliases[a].s+")");
	}else{
	  matches.push_back(m->aliases[a].s);
	}
	
      }   
  }


  template <typename T> 
   void IDNode<T>::addPossibilities(std::vector<std::string> &tot, const std::vector<std::string> &what,bool transparent) const{
     if (what.size()==0) return;
     std::stringstream ss;
     if (transparent) ss<<"(";
     helium::print("|",what,ss);
     if (transparent) ss<<")";
     tot.push_back(ss.str());
     //std::cout << "addpos " <<ss.str() << std::endl;
   }


  template <typename T> 
   void IDNode<T>::examineProblem(const IDNode* m,const IDPath &p,std::vector<std::string> &matches) const{
     int matched=(int)matches.size();
     std::vector<std::string> possible;
      for (size_t i=0;i<m->sons.size();i++){
	std::vector<std::string> thisSon;
	m->sons[i]->addPartialMatches(thisSon,p[matched]);
	addPossibilities(possible,thisSon,m->sons[i]->transparent);	
      }
      if (possible.size()==0){
	for (size_t i=0;i<m->sons.size();i++){
	 std::vector<std::string> thisSon;
	 m->sons[i]->getAliases(thisSon);
	 addPossibilities(possible,thisSon,m->sons[i]->transparent);		 
	 //std::cout << "addpos" << std::endl;
       }
	//std::cout << "possible size 0 end" << std::endl;
      }
      //std::cout << "outer if " << std::endl;
      std::stringstream ss;
      helium::print("",matches,".",ss);
      ss<<"["<<p[matched]<<"]=";
      ss<<"{";
      helium::print("",possible,"?",ss);
      ss<<"}";
      throw exc::UnknownID(ss.str());
   }

   



  /*IDNode* downMaximumPathUpToParent(size_t& matched,const IDPath &p){
    IDNode* m=this;    
    IDNode* n;
    size_t a;
    for (matched=0;matched+1<p.size();matched++){
      n=m->downHop(a,p[(int)matched]);
      if (n==NULL) 
	break;
      m=n;
    }
    return m;
    }*/



  



}//ns helium
