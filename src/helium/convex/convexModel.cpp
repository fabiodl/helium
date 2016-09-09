/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#include "convexModel.h"
//#include <math/mathUtil.h>
//#include <helium/math/priorityBuffer.h>
#include <algorithm>
#include <helium/destroy/autoDestroy.h>
#include <helium/util/mathUtil.h>
#include <helium/math/algebra.h>
#include <helium/vector/operations.h>
#include <helium/util/ioUtil.h>
#include <map>
using namespace std;
using namespace helium::convex;





/*bool ConvexModel::canCollide(int o1,int p1,int o2,int p2) const{
  for (size_t i=0;i<noCollision.size();i++){
    if ((noCollision[i].first.first==o1&&
	 noCollision[i].first.second==p1&&
	 
	 noCollision[i].second.first==o2&&
	 noCollision[i].second.second==p2)||
	(noCollision[i].first.first==o2&&
	 noCollision[i].first.second==p2&&
	 
	 noCollision[i].second.first==o1&&
	 noCollision[i].second.second==p1))
      return false;
  }
  return true;
  }*/


double ConvexModel::getMargin(int o1,int p1,int o2,int p2) const{
  pair<int,int> op1=make_pair(o1,p1);
  pair<int,int> op2=make_pair(o2,p2);
  if (margin.find(make_pair(op1,op2))!=margin.end()){
    return margin.find(make_pair(op1,op2))->second;
  }
  if (margin.find(make_pair(op2,op1))!=margin.end()){
    return margin.find(make_pair(op2,op1))->second;
  }
  return defaultMargin;
}


void ConvexCollisionModel::getJointRoto(double* m,const JointInfo* b){
  double r[16];
  helium::eye(r);
  helium::getRotation(r,b->jointRot[0],b->jointRot[1],b->jointRot[2],b->jointRot[3]);
  helium::setTranslation(r,b->jointTran);
  helium::mul(m,r,jointOffsetMatrix);
}

void getNodeRoto(double* m,const BoxNode* b){
  helium::eye(m);
  helium::getRotation(m,b->boxRot[0],b->boxRot[1],b->boxRot[2],b->boxRot[3]);
  helium::setTranslation(m,b->boxTran.data);
}


void getFlatNode(BoxNode* b,const BoxNode* src,const double* mirror){
  double absRoto[16],m[16],pm[16];
  getNodeRoto(m,src);
  for (BoxNode* p=src->boxParent;p!=NULL;p=p->boxParent){
    getNodeRoto(pm,p);
    helium::mul(absRoto,pm,m);
    memcpy(m,absRoto,sizeof(double)*16);
    //cout<<"1 parent up"<<endl;
  }
    
  int baseCoord=(int)b->boxPoint.size();

  for (size_t i=0;i<src->boxPoint.size();i++){
    //double* p=new double[3]; //deleted by ~BoxNode //replaced by array 20111017

    helium::Array<double,3> p;

    helium::mulPoint(p.data,absRoto,src->boxPoint[i].data);
    
    if (mirror!=NULL){
      double pm[3];
      for (int c=0;c<3;c++) pm[c]=p[c];
      helium::mulPoint(p.data,mirror,pm);
    }
    //cout<<src->boxPoint[i][0]<<","<<src->boxPoint[i][1]<<","<<src->boxPoint[i][2]<<endl;
    //cout<<p[0]<<","<<p[1]<<","<<p[2]<<endl;

    b->boxPoint.push_back(p);
  }
  //cout<<"src has "<<src->boxCoord.size()<<" coordinates"<<endl;
  for (size_t i=0;i<src->boxCoord.size();i++){
    //int* c=new int[3];    //delted by ~BoxNode //replaced by array 20111017
    helium::Array<int,3> c;
    for (int j=0;j<3;j++){
      c[j]=src->boxCoord[i][j]+baseCoord;
      if (c[j]>(int)b->boxPoint.size()){
	cerr<<"bad coordinate!"<<endl;
	exit(1);
      }
    }
  
    b->boxCoord.push_back(c);
  }
  for (size_t i=0;i<src->boxChildren.size();i++){
    getFlatNode(b,src->boxChildren[i],mirror);
  }
  
}  


//http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline3d/
bool getClosestPoints(const helium::Vector3d &p1, const helium::Vector3d &p2,const helium::Vector3d &p3, const helium::Vector3d &p4,
		       double *mua, double *mub, helium::Vector3d *pa=NULL,helium::Vector3d *pb=NULL)
{
  double EPS=1E-17;
  helium::Vector3d p13,p43,p21;
  double d1343,d4321,d1321,d4343,d2121;
  double numer,denom;
  p13.sub(p1,p3);
  p43.sub(p4,p3);
  
  if (helium::infinityNorm(p43)<EPS)return false;
   p21.sub(p2,p1);
   if (helium::infinityNorm(p21)<EPS) return false;
   

   d1343 = p13.x() * p43.x() + p13.y() * p43.y() + p13.z() * p43.z();
   d4321 = p43.x() * p21.x() + p43.y() * p21.y() + p43.z() * p21.z();
   d1321 = p13.x() * p21.x() + p13.y() * p21.y() + p13.z() * p21.z();
   d4343 = p43.x() * p43.x() + p43.y() * p43.y() + p43.z() * p43.z();
   d2121 = p21.x() * p21.x() + p21.y() * p21.y() + p21.z() * p21.z();

   denom = d2121 * d4343 - d4321 * d4321;

   if (fabs(denom) < EPS) return false;

   numer = d1343 * d4321 - d1321 * d4343;

   *mua = numer / denom;
   *mub = (d1343 + d4321 * (*mua)) / d4343;

   if (pa!=NULL){
     pa->x() = p1.x() + *mua * p21.x();
     pa->y() = p1.y() + *mua * p21.y();
     pa->z() = p1.z() + *mua * p21.z();
   }
   if (pb!=NULL){
     pb->x() = p3.x() + *mub * p43.x();
     pb->y() = p3.y() + *mub * p43.y();
     pb->z() = p3.z() + *mub * p43.z();
   }

   return true;
}

bool intersects(double* p1,double* p2,double* p3,double* p4){
  double mua,mub;
  helium::Vector3d pa,pb;
    if (!getClosestPoints(helium::Vector3d(p1),helium::Vector3d(p2),helium::Vector3d(p3),helium::Vector3d(p4),&mua,&mub,&pa,&pb))
      return false;

  //lineLineIntersect(helium::Vector3d(p1),helium::Vector3d(p2),helium::Vector3d(p3),helium::Vector3d(p4),&mua,&mub,&pa,&pb);
  //cout<<mua<<","<<mub<<endl;
    if (!helium::isInRange(0.0,1.0,mua)||!helium::isInRange(0.0,1.0,mub)) return false;
    
  static const double eps=1E-5;
  
  pa.sub(pb);
  //cout<<"norm is "<<pa.norm()<<endl;
  return (helium::norm(pa)<eps);
}

bool isParallel(double* p1,double* p2,double* p3,double* p4){
  static const double eps=1E-3;
  helium::Vector3d ab(p2[0]-p1[0],p2[1]-p1[1],p2[2]-p1[2]);
  helium::Vector3d cd(p4[0]-p3[0],p4[1]-p3[1],p4[2]-p3[2]);
  helium::normalize(ab);
  helium::normalize(cd);
  //cout<<"dot is "<<ab.dot(cd)<<endl;
  return fabs(helium::dot(ab,cd))>1-eps;
}

inline double getSeparation(double a,double b,double c,double d){
  if (c>b) return c-b;
  if (a>d) return a-d;
  return 0;
}



ConvexData::PartData::~PartData(){
  if (!isCopy){
    //cout<<"here "<<this<<" deleting "<<obj<<std::endl;
    //delete obj;
  }
  //destroyA(absolutePnt);//when copied by const ctor, these are new data //replaced by array
}

void ConvexData::PartData::makeEdges(){
  vector<pair<int,int> > candidates;
  //cout<<"obj has"<<obj.boxCoord.size()<<" coordinates "<<endl;
  for (size_t i=0;i<obj.boxCoord.size();i++){
    for (int j=0;j<3;j++){
      //cout << obj.boxCoord[i][j] << endl;
      candidates.push_back(make_pair(obj.boxCoord[i][j],obj.boxCoord[i][(j+1)%3]));
    }//for j
  }



  //cout<<"cdts has"<<candidates.size()<<endl;
  for (size_t i=0;i<candidates.size();i++){
    bool ok=true;
    unsigned int a=candidates[i].first;
    unsigned int b=candidates[i].second;
    for (size_t c=0;c<obj.boxPoint.size();c++){

      for (size_t d=c+1;d<obj.boxPoint.size();d++){
	
	if (a==c||a==d||b==c||b==d){
	  //cout<<"shared vertex"<<endl;
	  continue;      
	}
	//cout<<a<<" "<<b<<" intersects "<<c<<" "<<d<<endl;
	if (a<0||a>=obj.boxPoint.size()){
	  cerr<<"bad coordinate bp"<<endl;
	  exit(1);
	}
	

	if (::intersects(obj.boxPoint[a].data,obj.boxPoint[b].data,
		       obj.boxPoint[c].data,obj.boxPoint[d].data)){
	  
	  ok=false;

	  break;
	}//if intersects	        
	else{
	  //diagonals.push_back(make_pair(c,d));
	}
      }//for d
    }//for c
    if (ok)
      peripheralEdges.push_back(candidates[i]);
    if (ok){
      for (size_t k=0;k<edges.size();k++){
	int c=edges[k].from;
	int d=edges[k].to;
	//cout<<a<<" "<<b<<" vs "<<c<<" "<<d<<endl;
	if (isParallel(obj.boxPoint[a].data,obj.boxPoint[b].data,
		       obj.boxPoint[c].data,obj.boxPoint[d].data)){
	  //cout<<a<<" "<<b<<" is parallel to "<<c<<" "<<d<<endl;
	  ok=false;
	  break;
	}
      }//for k
    }//if ok


    if (ok){ 
      //cout<<"pushing "<<candidates[i].first<<" "<<candidates[i].second<<endl;
      //cout <<  obj.boxPoint[candidates[i].first][0] << " " <<obj.boxPoint[candidates[i].second].data[0] <<endl;
      double scale=distance(obj.boxPoint[candidates[i].first].data,obj.boxPoint[candidates[i].second].data);
      //cout << "scale " << scale << endl;
      edges.push_back(EdgeData(candidates[i],scale));
    }       
  }//for i

  for (size_t k=0;k<edges.size();k++){
    int mini=0;
    int maxi=0;
    double minv,maxv;   
    EdgeData &e=edges[k];
    helium::Vector3d dir(obj.boxPoint[e.to].data);
    helium::Vector3d head(obj.boxPoint[e.from].data);
    dir.sub(head);
    minv=maxv=helium::dot(dir,obj.boxPoint[0]);
    //cout<<"fixing over"<<obj.boxPoint.size()<<"pts"<<endl;
    for (size_t i=1;i<obj.boxPoint.size();i++){
      double v=helium::dot(dir,obj.boxPoint[i]);
      //cout<<"v is "<<v<<endl;
      if (v<minv){
	minv=v;
	mini=(int)i;
      }
      if (v>maxv){
	maxv=v;
	maxi=(int)i;
      }
    }//for i
    edges[k].setLimits(mini,maxi);
  }//for k
    
}


inline void ConvexData::PartData::getDirection(double* dir,double &mine,double &maxe,int eid) const {    
  //cout << "subnow" << endl;
  helium::sub(dir,absolutePnt[edges[eid].to],absolutePnt[edges[eid].from]);
  //cout << "dotnow" << endl;
  //cout <<"nnow "<< absolutePnt[edges[eid].mini].size() << endl;
  mine=helium::dot(dir,absolutePnt[edges[eid].mini]);
  maxe=helium::dot(dir,absolutePnt[edges[eid].maxi]);
}

inline void ConvexData::PartData::getDirection(double* dir,int eid) const {
  dir[0]=absolutePnt[edges[eid].to][0]-absolutePnt[edges[eid].from][0];
  dir[1]=absolutePnt[edges[eid].to][1]-absolutePnt[edges[eid].from][1];
  dir[2]=absolutePnt[edges[eid].to][2]-absolutePnt[edges[eid].from][2];
}

//double countProcessing;

inline void ConvexData::PartData::getExtent(double &mine,double &maxe,const double* dir) const{
  mine=maxe=
    dir[0]*absolutePnt[0][0]+
    dir[1]*absolutePnt[0][1]+
    dir[2]*absolutePnt[0][2];
  double vi;
  int x=absolutePnt.size();
  for (int i=0;i<x;i++){
    vi=dir[0]*absolutePnt[i][0]+
      dir[1]*absolutePnt[i][1]+
      dir[2]*absolutePnt[i][2];    
    if (vi>maxe) maxe=vi; else
      if (vi<mine) mine=vi;
  }

  //countProcessing++;
}


inline void ConvexData::PartData::getSphereExtent(double &mine,double &maxe,const double* dir) const{
  double v=
    dir[0]*sphere.center.data[0]+
    dir[1]*sphere.center.data[1]+
    dir[2]*sphere.center.data[2];
  mine=v-sphere.radius;
  maxe=v+sphere.radius;
  //cout<<"radius="<<sphere.radius<<endl;;
}

ConvexData::EnclosingSphere::EnclosingSphere():center(3){
}

ConvexData::ConvexData():src(NULL),angle(NULL){
  helium::eye(relative.data);
  helium::eye(absolute.data);
}

ostream &operator<<(ostream& o,const ConvexData::EnclosingSphere &s){
  o<<s.radius<<"("<<s.part1<<","<<s.point1<<"~"<<s.part2<<","<<s.point2<<endl;
  return o;
}


void ConvexData::PartData::makeSphere(){
  sphere.radius=0;
  sphere.part1=sphere.point1=sphere.part2=sphere.point2=-1;
  for (size_t p1=0;p1<absolutePnt.size();p1++){
    for (size_t p2=0;p2<absolutePnt.size();p2++){ 
      double d=helium::pointPointDistance<double>(absolutePnt[p1],absolutePnt[p2],3)/2;
      if (d>sphere.radius){
	sphere.radius=d;
	sphere.point1=p1;
	sphere.point2=p2;
      }
    }
  }
  updateSphere();
  
}

inline void ConvexData::PartData::updateSphere(){
  if (sphere.point1==-1) return;
  for (int i=0;i<3;i++){
    sphere.center[i]=
      (absolutePnt[sphere.point1][i]+
       absolutePnt[sphere.point2][i])/2;
  }
}

void ConvexData::makeSphere(){
  sphere.radius=0;
  sphere.part1=sphere.point1=sphere.part2=sphere.point2=-1;
  for (size_t j1=0;j1<parts.size();j1++){
    for (size_t p1=0;p1<parts[j1].absolutePnt.size();p1++){
      for (size_t j2=0;j2<parts.size();j2++){
	for (size_t p2=0;p2<parts[j2].absolutePnt.size();p2++){
	  double d=helium::pointPointDistance<double>(parts[j1].absolutePnt[p1],
					      parts[j2].absolutePnt[p2],3)/2;
	  if (d>sphere.radius){
	    sphere.radius=d;
	    sphere.part1=j1;
	    sphere.point1=p1;
	    sphere.part2=j2;
	    sphere.point2=p2;
	  }
	}//p2
      }//j2
    }//p1
  }//j1

  //cout<<"made sphere"<<sphere<<endl;
  updateSphere();
}

inline void ConvexData::updateSphere(){
  if (sphere.part1==-1) return;
  for (int i=0;i<3;i++){
    sphere.center[i]=
      (parts[sphere.part1].absolutePnt[sphere.point1][i]+
       parts[sphere.part2].absolutePnt[sphere.point2][i])/2;
  }
}

ConvexCollisionModel::CollisionCheck::~CollisionCheck(){
  delete nextCheck;
}

ConvexCollisionModel::~ConvexCollisionModel(){
  delete checks;  
}

void ConvexCollisionModel::CollisionCheck::push_back(ConvexCollisionModel::CollisionCheck * c){
  CollisionCheck* tail=this;
  while (tail->nextCheck!=NULL){
    tail=tail->nextCheck;
  }
  tail->nextCheck=c;
  CollisionCheck* p=this;
  CollisionCheck* lc=NULL;
  pair<int,int> lastCouple(-1,1);
  while (p!=c){
    int mi=min(p->obj1.first,p->obj2.first);
    int ma=max(p->obj1.first,p->obj2.first);
    if (mi!=lastCouple.first||ma!=lastCouple.second){      
      lastCouple.first=mi;
      lastCouple.second=ma;      
      lc=p;
    }  
    p=p->nextCheck;
  }
  if (lastCouple.first!=min(c->obj1.first,c->obj2.first)||
      lastCouple.second!=max(c->obj1.first,c->obj2.first)){
    lc->nextCouple=c;
  }
}

int ConvexCollisionModel::CollisionCheck::size(){
  CollisionCheck* tail=this;
  int l=0;
  while ((tail=tail->nextCheck)!=NULL){
    l++;
  }
  return l;
}


void ConvexCollisionModel::initAngles(const double* angles){
  if (angles==NULL){
    int mid=-1; //motor joint num
    for (size_t i=0;i<cd.size();i++){
      mid=max(mid,cd[i].src->motorId);
    }
    mid++; //tranform mid to a size
    if (mid){
      internalAngles.resize(mid);                //internalAngle = 22
      //helium array: internalAngles.zero();
      helium::zero(internalAngles);
      angles=internalAngles.data;            //DEBUG TODO setting a NULL????
    }
    //cout << "mid " << mid << " cd" << cd.size()<< endl;
  }
  //cout << "angles " << internalAngles << endl;  
  for (size_t i=0;i<cd.size();i++){              //cd.size() = 35
    if (cd[i].src->motorId>=0){                  //
      cd[i].angle=&angles[cd[i].src->motorId];   //angles = 22
      //cd[i].angle=&internalAngles.data[cd[i].src->motorId];   //angles = 22
      //cout << *cd[i].angle << " ";
    }else{
      //cout << "n";
      cd[i].angle=NULL;
    }
  }//for
}

ConvexCollisionModel::ConvexCollisionModel(const ConvexModel* cm,const double* angles):
  src(cm),access("ConvexCollisionModelaccess"){  

  ///cout << "src links " << src->links.size() << endl;

  helium::eye(mirrorMatrix);
  helium::eye(jointOffsetMatrix);
  helium::setTranslation(jointOffsetMatrix,0.00249,0.01999,-0.00249);
  
  if (src->mirrorPointR!=NULL&&src->mirrorPointL!=NULL){
    helium::Array<double,3> mirrorDir;
    //cout<<"ptR "<<src->mirrorPointR->jointTran[0]<<","<<src->mirrorPointR->jointTran[1]<<","<<src->mirrorPointR->jointTran[2]<<endl;
    //cout<<"ptl "<<src->mirrorPointL->jointTran[0]<<","<<src->mirrorPointL->jointTran[1]<<","<<src->mirrorPointL->jointTran[2]<<endl;
    //operations.h: mirrorDir.sub(src->mirrorPointR->jointTran,src->mirrorPointL->jointTran);
    sub(mirrorDir,src->mirrorPointR->jointTran,src->mirrorPointL->jointTran);
    //cout<<"Mirror dir is"<<mirrorDir<<endl;
    helium::getReflection(mirrorMatrix,mirrorDir.data);
    //helium::print(mirrorMatrix);
  }else{
    //cout<<"No mirror"<<endl;
  }


  int maxID=-1;
  const vector<ConvexLink*> &links=src->links;
  for (size_t i=0;i<links.size();i++){    
    maxID=max(links[i]->id,maxID);
    if (links[i]->id<links[i]->parent){
      cerr<<"Invalid ids:parent should be < son for link "<<links[i]->info->getName()<<":id="<<links[i]->id<<" parent="<<links[i]->parent<<endl;
      exit(1);
    }
  }
  //cout<<"maxid="<<maxID<<endl;
  cd.resize(maxID+1);      
  //cout<<"cdsize"<<cd.size()<<endl;
  for (size_t i=0;i<links.size();i++){
    cd[links[i]->id].src=links[i];
  }
  for (size_t i=0;i<cd.size();i++){    
    helium::eye(cd[i].absolute.data);
    if (cd[i].src!=NULL&&cd[i].src->info){//if (cd[i].src!=NULL&&cd[i].absolute,cd[i].src->info){
      getJointRoto(cd[i].absolute.data,cd[i].src->info);
      if (cd[i].src->toMirror){
	double tmp[16];
	helium::copyMatrix(tmp,cd[i].absolute.data);
	helium::mul(cd[i].absolute.data,mirrorMatrix,tmp);
      }
    }
    else{
      helium::eye(cd[i].absolute.data);
    }
  }
  double eyed[16];
  helium::eye(eyed);
  double* parentRT=NULL;
  double invm[16];
  initAngles(angles);
  //cout<<"Initing"<<endl;
  for (size_t i=0;i<cd.size();i++){
    if (cd[i].src==NULL) continue;
    //cout<<"Processing cd "<<i<<endl;
    if (cd[i].src->parent==-1){
      parentRT=eyed;
    }else{
      parentRT=cd[cd[i].src->parent].absolute.data;
    }
    helium::invert(invm,parentRT);
    helium::mul(cd[i].relative.data,invm,cd[i].absolute.data);    
    helium::invert(invm,cd[i].absolute.data);
    cd[i].parts.resize((int)cd[i].src->part.size());
    for (int j=0;j<(int)cd[i].src->part.size();j++){
      //BoxNode* part=new BoxNode(); //deleted by ~PartData// converted to object


      cd[i].parts[j].obj.setName(cd[i].src->part[j]->getName()); //copy name
      cd[i].parts[j].obj.boxType = cd[i].src->part[j]->boxType; //copy type
      //cout << cd[i].parts[j].obj.getName() << " type " << cd[i].parts[j].obj.boxType  << endl;







      //part->boxRot=new double[4]; //deleted by ~BoxNode //replaced by array 20111017
      cd[i].parts[j].obj.boxRot[0]=1;
      cd[i].parts[j].obj.boxRot[1]=cd[i].parts[j].obj.boxRot[2]=cd[i].parts[j].obj.boxRot[3]=0;
      //part->boxTran=new double[3]; //deleted by ~BoxNode //replaced by array 20111017
      cd[i].parts[j].obj.boxTran[0]=cd[i].parts[j].obj.boxTran[1]=cd[i].parts[j].obj.boxTran[2]=0;
      getFlatNode(&cd[i].parts[j].obj,cd[i].src->part[j],
		  //cd[i].src->part[j].second?mirrorPoint->jointRot:NULL

		  cd[i].src->toMirror?mirrorMatrix:NULL
		  );
      //make the point coordinates relative
      cd[i].parts[j].absolutePnt.resize((int)cd[i].parts[j].obj.boxPoint.size());


      for (size_t p=0;p<cd[i].parts[j].obj.boxPoint.size();p++){
	double tmp[3];
	helium::mulPoint(tmp,invm,cd[i].parts[j].obj.boxPoint[p].data);
	//cd[i].parts[j].absolutePnt[p]=new double[3];//deleted by ~PartData //replaced by array 20111017
	for (int k=0;k<3;k++){
	  cd[i].parts[j].absolutePnt[p][k]=cd[i].parts[j].obj.boxPoint[p][k];


	  cd[i].parts[j].obj.boxPoint[p][k]=tmp[k];
	}//for k
      }//for p

      //cout<<"Created part "<<j<<endl;
     
      //cd[i].parts[j].obj=part; // converted to object


      cd[i].parts[j].makeEdges();
      cd[i].parts[j].makeSphere();
    }//for j in cd.src->part 
    cd[i].makeSphere();
  }//for i in cd

  //cout << "creating new collisioncheck" << endl;
  checks=new CollisionCheck(); //deleted by ~ConvexCollisionModel()
  double dir[3];double mine1,maxe1,mine2,maxe2;


  for (size_t i1=0;i1<cd.size();i1++){ //obj1 
    for (size_t i2=i1+1;i2<cd.size();i2++){ //obj2
      for (size_t j1=0;j1<cd[i1].parts.size();j1++){ //part of obj1 
	for (size_t j2=0;j2<cd[i2].parts.size();j2++){ //part of obj2
	  double margin=src->getMargin(i1,j1,i2,j2);
	  if (margin!=-1){
	    CollisionCheck* c; 

	    if (cd[i1].parts[j1].getVertexNum()>cd[i2].parts[j2].getVertexNum()){
	      c=new CollisionCheck(i1,j1,i2,j2,margin);	 //chain deleted by  ~CollisionCheck()
	    }else{
	      c=new CollisionCheck(i2,j2,i1,j1,margin);  //chain deleted by  ~CollisionCheck()
	    }

	    ConvexData::PartData &p1=cd[c->obj1.first].parts[c->obj1.second]; 
	    ConvexData::PartData &p2=cd[c->obj2.first].parts[c->obj2.second]; 
	    int bestEdge=0;
	    double bestSeparation=0;
	    for (int e=0;e<(int)p1.edges.size();e++){
	      p1.getDirection(dir,mine1,maxe1,e);
	      p2.getExtent(mine2,maxe2,dir); //was segfault, &p2->p2, continued
	      double s=getSeparation(mine1,maxe1,mine2,maxe2);
	      if (s>bestSeparation){
		bestSeparation=s;
		bestEdge=e;
	      }
	    }
	    c->suggestedEdge=bestEdge;
	    checks->push_back(c);
	  }//canCollide
      }//j1
      
      }//j1
    }//i2 
  }//i1
  //cout << "collisioncheck loop ends" << endl;
  /*CollisionCheck* p=checks;
  checks=checks.nextCheck();
  delete p;*/
  rotate();
  isColliding(false);
}//init


void ConvexCollisionModel::oldRotate(){
  double eyed[16];
  helium::eye(eyed);
  double* parentRT=NULL;
  double tmp[16];
  double currRot[16];
  helium::eye(currRot);
  for (size_t i=0;i<cd.size();i++){
    if (cd[i].src==NULL) continue;
    if (cd[i].src->parent==-1){
      parentRT=eyed;
    }else{
      parentRT=cd[cd[i].src->parent].absolute.data;
    }
    helium::mul(tmp,parentRT,cd[i].relative.data);

    if(cd[i].angle != NULL) {
      helium::setRotation(currRot,helium::Axis::Y,*cd[i].angle*cd[i].src->multiply+cd[i].src->offset);
    }
    helium::mul(cd[i].absolute.data,tmp,currRot);
    

    for (size_t j=0;j<cd[i].parts.size();j++){
      for (size_t p=0;p<cd[i].parts[j].absolutePnt.size();p++){
	helium::mulPoint(cd[i].parts[j].absolutePnt[p].data,
			 cd[i].absolute.data,
			 cd[i].parts[j].obj.boxPoint[p].data);
      }
    }
    cd[i].updateSphere();
  }
}



void ConvexCollisionModel::rotate(){

  //double eyed[16];
  //eye(eyed);
  //double currRot[16];
  
  double angle,c,s,*m,*r,*p,*v,*w;
  int size=cd.size();
  //cout << "rotate size " << size << endl;
  //cout << "rotate " << this<< " " ;

  for (int i=0;i<size;i++){    
    if (cd[i].src==NULL) continue;
    //cout << "m"<<i<<":"<<(cd[i].src?cd[i].src->offset:NAN) << " ";
    if (cd[i].angle==NULL)
      angle=0;
    else
      angle=*cd[i].angle*cd[i].src->multiply+cd[i].src->offset;
    c=cos(angle);
    s=sin(angle);

    m=cd[i].absolute.data;
    r=cd[i].relative.data;

    //DEBUG temporarily commented
    if (cd[i].src->parent!=-1){
      p=cd[cd[i].src->parent].absolute.data;
      
      m[0]=((p[0]*r[0]+p[1]*r[4]+p[2]*r[8])*c-(p[0]*r[2]+p[1]*r[6]+p[2]*r[10])*s);
      m[1]=(p[0]*r[1]+p[1]*r[5]+p[2]*r[9]);
      m[2]=((p[0]*r[0]+p[1]*r[4]+p[2]*r[8])*s+(p[0]*r[2]+p[1]*r[6]+p[2]*r[10])*c);
      m[3]=p[0]*r[3]+p[1]*r[7]+p[2]*r[11]+p[3];
      m[4]=((p[4]*r[0]+p[5]*r[4]+p[6]*r[8])*c-(p[4]*r[2]+p[5]*r[6]+p[6]*r[10])*s);
      m[5]=(p[4]*r[1]+p[5]*r[5]+p[6]*r[9]);
      m[6]=((p[4]*r[0]+p[5]*r[4]+p[6]*r[8])*s+(p[4]*r[2]+p[5]*r[6]+p[6]*r[10])*c);
      m[7]=p[4]*r[3]+p[5]*r[7]+p[6]*r[11]+p[7];
      m[8]=((p[8]*r[0]+p[9]*r[4]+p[10]*r[8])*c-(p[8]*r[2]+p[9]*r[6]+p[10]*r[10])*s);
      m[9]=(p[8]*r[1]+p[9]*r[5]+p[10]*r[9]);
      m[10]=((p[8]*r[0]+p[9]*r[4]+p[10]*r[8])*s+(p[8]*r[2]+p[9]*r[6]+p[10]*r[10])*c);
      m[11]=p[8]*r[3]+p[9]*r[7]+p[10]*r[11]+p[11];
    }else{
      m[0]=(r[0]*c-r[2]*s);
      m[1]=r[1];
      m[2]=(r[0]*s+r[2]*c);
      m[3]=r[3];
      m[4]=(r[4]*c-r[6]*s);
      m[5]=r[5];
      m[6]=(r[4]*s+r[6]*c);
      m[7]=r[7];
      m[8]=(r[8]*c-r[10]*s);
      m[9]=r[9];
      m[10]=(r[8]*s+r[10]*c);
      m[11]=r[11];      
    }

    //absolutePnt updated here!
    for (size_t j=0;j<cd[i].parts.size();j++){

      for (size_t z=0;z<cd[i].parts[j].absolutePnt.size();z++){
	v=cd[i].parts[j].absolutePnt[z].data;
	w=cd[i].parts[j].obj.boxPoint[z].data;
	
	
	v[0]=m[0]*w[0]+m[1]*w[1]+m[2]*w[2]+m[3];
	v[1]=m[4]*w[0]+m[5]*w[1]+m[6]*w[2]+m[7];
	v[2]=m[8]*w[0]+m[9]*w[1]+m[10]*w[2]+m[11];
	
	//cd[i].parts[j].updateSphere();
      }
                 
    }
    cd[i].updateSphere();
  }
  //cout << endl;
}


inline bool ConvexCollisionModel::isDisjoint(const ConvexData::PartData &p1,const ConvexData::PartData &p2,int e,const double& margin){
  p1.getDirection(dir,mine1,maxe1,e);
  /*p2.getSphereExtent(mine2,maxe2,dir);
  if (segmentsAreDisjoint(mine1,maxe1,mine2,maxe2)){
    cout<<"sphere skipped"<<endl;
    return true;
  }else{
    //cout<<"sphere is"<<mine2<<"~"<<maxe2<<"friend is "<<mine1<<"~"<<maxe1<<endl;
    }*/
  //cout << "mine1 " << mine1 << " " << maxe1 << " " << dir << endl;
  p2.getExtent(mine2,maxe2,dir);  
  //cout << "mine2 " << mine2 << " " << maxe2 << " " << dir << endl;
  //if (segmentsAreDisjoint(mine1,maxe1,mine2,maxe2,margin))
  //cout<<"using margin"<<margin<<"with "<<mine2<<"~"<<maxe2<<"friend is "<<mine1<<"~"<<maxe1<<endl;
  return helium::segmentsAreDisjoint(mine1,maxe1,mine2,maxe2,margin*p1.edges[e].scale);
}


inline bool ConvexCollisionModel::isDisjoint(const ConvexData::PartData &p1,const ConvexData::PartData &p2,const double* dir,const double& margin){
  p1.getExtent(mine1,maxe1,dir);
  p2.getExtent(mine2,maxe2,dir);
  return helium::segmentsAreDisjoint(mine1,maxe1,mine2,maxe2,margin*helium::normn<3>(dir));
}


bool ConvexCollisionModel::isColliding(bool fastCheck){
  //access.countLock();
  bool coll=false;
  int si=cd.size();
  int sj;
  for (int i=0;i<si;i++){
    sj=cd[i].parts.size();
    for (int j=0;j<sj;j++){
      cd[i].parts[j].colliding=false;
    }
  }
  //static int opt=0;
  //opt++;
  //static double maxAvg=0;
  //double avgCountProcessing=0;
  CollisionCheck* c=checks;

  //double jumpCo=0;
  //double jumpNe=0;
  while ((c=c->nextCheck)!=NULL){

    //if (c->nextCouple!=NULL) cout<<"could jump.."<<endl;
    //else cout<<"follow"<<endl;
    /*    if (c->nextCouple!=NULL){
      cout<<cd[c->obj1.first].sphere.center.euclideanDistance(cd[c->obj2.first].sphere.center)
	  <<" vs "<<cd[c->obj1.first].sphere.radius<<"+"<<cd[c->obj2.first].sphere.radius<<endl;
	  }*/

    
      

    while (c->nextCouple!=NULL&&
	   //operations.h: cd[c->obj1.first].sphere.center.euclideanDistance(cd[c->obj2.first].sphere.center)>
	   helium::euclideanDistance(cd[c->obj1.first].sphere.center,cd[c->obj2.first].sphere.center) >
	   cd[c->obj1.first].sphere.radius+cd[c->obj2.first].sphere.radius+c->margin){      
      c=c->nextCouple;
      //jumpCo++;
    }
    
      //jumpNe++;
      //cout<<"next node"<<endl;

   
      
    //if (countProcessing>0) avgCountProcessing+=countProcessing;
    //if (countProcessing==0) cout<<"No check!"<<endl;
    //if (countProcessing!=1)
    //cout<<countProcessing<<" checks"<<" for "<<checks.size()<<endl;
    //countProcessing=0;
    ConvexData::PartData &p1=cd[c->obj1.first].parts[c->obj1.second];
    ConvexData::PartData &p2=cd[c->obj2.first].parts[c->obj2.second];
    int suggestedEdge=c->suggestedEdge;


    //suggested
    if (isDisjoint(p1,p2,suggestedEdge,c->margin)) continue;


    bool disjoint=false;;
    for (int e=0;e<suggestedEdge;e++){
      if (isDisjoint(p1,p2,e,c->margin)){
	disjoint=true;
	c->suggestedEdge=e;
	break;	
      }
    }

    //<suggested
    if (disjoint) continue;



    si=(int)p1.edges.size();
    for (int e=suggestedEdge+1;e<si;e++){
      if (isDisjoint(p1,p2,e,c->margin)){
	disjoint=true;
	c->suggestedEdge=e;
	break;	
      }
    }



    //>suggested
    if (disjoint) continue;



    sj=(int)p2.edges.size();
    for (int e=0;e<sj;e++){
      if (isDisjoint(p2,p1,e,c->margin)){
	disjoint=true;
	c->flipObj();
	c->suggestedEdge=e;
	break;	
      }
    }

    //part 2
    if (disjoint) continue;



    for (int e1=0;e1<si;e1++){    
      p1.getDirection(dir1,e1);
      for (int e2=0;e2<sj;e2++){
	p2.getDirection(dir2,e2);
	//cout << "crossnow" << endl;
	helium::cross(dir,dir1,dir2);
	//cout << "crossfin" << endl;
	if (isDisjoint(p1,p2,dir,c->margin)){		  
	  disjoint=true;
	  break;
	}
      }//for e2
    }//for e1
    
    if (disjoint) continue;
    
 
    p1.colliding=true;
    p2.colliding=true;   
    if (fastCheck) return true;
    coll=true;

    //cout << c->obj1.first << " " << c->obj1.second << " " << c->obj2.first << " " << c->obj2.second << " name  "<< p1.obj.getName() << " " << p2.obj.getName()<< endl;

    //countProcessing=-countProcessing;
  }//for checks
  /*avgCountProcessing=avgCountProcessing/checks->size();
  if (opt>6)
    maxAvg=max(maxAvg,avgCountProcessing);
  cout<<"avg"<<avgCountProcessing<<"max "<<maxAvg<<" for "<<checks->size()<<endl;
  */
  //cout<<"jumping couple "<<jumpCo/(jumpCo+jumpNe)<<endl;
  //access.countUnlock();
  return coll;
}


void ConvexCollisionModel::rotate(const double *v){
  internalAngles=v;  
  helium::Array<double> prevr(22); //DEBUG TODO
  prevr = v;
  //cout << "rotate " <<prevr << endl;
  rotate();
}

void ConvexCollisionModel::rotate(int j,double v){
  internalAngles[j]=v;  
  //cout<<"convex rotate "<<j<<" to "<<v<<endl;
  rotate();
}

//void ConvexCollisionModel:: getLowest(int k,double* dir){
  
//}


ConvexData::PartData::PartData():
isCopy(false),
obj(NULL),colliding(false) 
{ 
  //std::cout<<"PD ctor"<<this<<std::endl;    
}

ConvexData::PartData& ConvexData::PartData::operator=(const PartData& b)
{ 
  isCopy=true;
  obj=b.obj;
  colliding=false;
  absolutePnt.resize(b.absolutePnt.size());
  edges=b.edges;
  peripheralEdges=b.peripheralEdges; //TODO: this stuff could be shared
  diagonals=b.diagonals;
  sphere=b.sphere;
  for (size_t i=0;i<b.absolutePnt.size();i++){
    //absolutePnt[i]=new double[3]; //DEBUG HERE //deleted by ~PartData
    for (int j=0;j<3;j++){
      absolutePnt[i][j]=b.absolutePnt[i][j];
    }//for j
  }//for i
  return *this;
}



ConvexData::PartData::PartData(const ConvexData::PartData& b)
{  
  *this=b; 
}


ConvexData& ConvexData::operator=(const ConvexData& b){
  src=b.src;
  angle=NULL;
  parts=b.parts;
  sphere=b.sphere;  
  
  //20111017 added
  relative = b.relative;
  absolute = b.absolute;
  return *this;
 }

 ConvexData::ConvexData(const ConvexData& b){
   //cout << "convexdata "<<this << " " << &b << endl;
   *this=b;
}



namespace helium{
namespace convex{

void copyCheckChain( ConvexCollisionModel::CollisionCheck  **dst,ConvexCollisionModel::CollisionCheck  *src){  
  std::map<ConvexCollisionModel::CollisionCheck*,ConvexCollisionModel::CollisionCheck*> corresp;
  ConvexCollisionModel::CollisionCheck* s=src;
  while (s!=NULL){
    ConvexCollisionModel::CollisionCheck* d=new ConvexCollisionModel::CollisionCheck();
    d->obj1=s->obj1;
    d->obj2=s->obj2;
    d->suggestedEdge=s->suggestedEdge;
    d->margin=s->margin;
    corresp[s]=d;
    *dst=d;
    dst=&d->nextCheck;
    s=s->nextCheck;
  };
  s=src;
  while (s!=NULL){
    corresp[s]->nextCheck=corresp[s->nextCheck];
    corresp[s]->nextCouple=corresp[s->nextCouple];
    s=s->nextCheck;
  }  
  //  memcpy(&dst,&src,sizeof(ConvexCollisionModel::CollisionCheck));
  //dst=s;
}

}//namespace convex

}//namespace helium
//copy constructor
ConvexCollisionModel::ConvexCollisionModel(const ConvexCollisionModel& b,double* angles):src(b.src),access("copiedModel"){
  memcpy(mirrorMatrix,b.mirrorMatrix,sizeof(double)*16);
  memcpy(jointOffsetMatrix,b.jointOffsetMatrix,sizeof(double)*16);
  cd = b.cd;
  copyCheckChain(&checks,b.checks);  
  initAngles(angles);
}
