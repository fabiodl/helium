#ifndef HE_TGRAPH_
#define HE_TGRAPH_

#include <vector>
#include <iostream>
#include <sstream>
#include <queue>
#include <map>
#include <set>
#include <list>
#include <limits>

namespace helium{
template <typename NodeData,typename EdgeData> class Tnode;

template <typename NodeData,typename EdgeData>
  class Tedge{
 private:
 public: 
  Tnode<NodeData,EdgeData>* from,*to;
  EdgeData* data;
 Tedge(Tnode<NodeData,EdgeData>* pfrom,Tnode<NodeData,EdgeData>* pto,EdgeData* pdata):from(pfrom),to(pto),data(pdata){}
 Tedge():data(NULL),from(NULL),to(NULL){}
};


template <typename NodeData,typename EdgeData> std::ostream& operator<<(std::ostream &os,const Tedge<NodeData,EdgeData> &e){
  if (e.to!=NULL)
    os<<e.to->id;
  else
    os<<-1;
  if (e.data==NULL)
    os<<" NULL";
  else
    os<<" "<<*e.data;
  return os;
}

template <typename NodeData,typename EdgeData>
class Tnode{
 public:
  int id;
  NodeData* data;
  std::vector<Tedge<NodeData,EdgeData>*> outEdges;
  std::vector<Tedge<NodeData,EdgeData>*> inEdges;
 Tnode(NodeData* pdata,int pid):id(pid),data(pdata){}

};

template <typename NodeData,typename EdgeData> std::ostream& operator<<(std::ostream &os,const Tnode<NodeData,EdgeData> &n){
  os<<n.id<<" ";
  if (n.data!=NULL) os<<*n.data;
  return os;
}

template <typename NodeData,typename EdgeData>
class Tgraph{
 public:
  
  std::vector<Tnode<NodeData,EdgeData>*> nodes;  

  NodeData* getNodeData(int i){
    if (i<0||i>=nodes.size()) return NULL;
    return nodes[i]->data;
  }

  Tnode<NodeData,EdgeData> getNode(int i){
    
  }
  
  
  
  int addNode(NodeData* d){
    nodes.push_back(new Tnode<NodeData,EdgeData>(d,nodes.size()));
    return nodes.size()-1;
  }
  
  
  
  void attachEdgeFrom(Tedge<NodeData,EdgeData>* e,int from){
    if (from!=-1){
      nodes[from]->outEdges.push_back(e);
      e->from=nodes[from];
    }
    //std::cout<<from<<"has "<<nodes[from]->outEdges.size()<<"outEdges"<<std::endl;
  }


void attachEdgeTo(Tedge<NodeData,EdgeData>* e,int to){
  if (to!=-1){
    nodes[to]->inEdges.push_back(e);
    e->to=nodes[to];
  }
}


 Tedge<NodeData,EdgeData>* addEdge(int a,int b,EdgeData* d){
    //if (a==-1||b==-1) return NULL;
    
   if (std::max(a,b)>=0){ 
     while (nodes.size()<=std::max(a,b)){
       nodes.push_back(new Tnode<NodeData,EdgeData>(NULL,-1));    
     }
   }
   Tedge<NodeData,EdgeData>* edge=new Tedge<NodeData,EdgeData>(a!=-1?nodes[a]:NULL,b!=-1?nodes[b]:NULL,d);
   //std::cout<<"adding"<<std::endl;
   attachEdgeFrom(edge,a);
   attachEdgeTo(edge,b);    
   return edge;
 }




  void detatchEdgeFrom(Tedge<NodeData,EdgeData>* e){
    if (e->from!=NULL) 
      for (int i=0;i<e->from->outEdges.size();i++){
	if (e->from->outEdges[i]==e){
	  e->from->outEdges.erase(e->from->outEdges.begin()+i);
	  break;
	}
      }//for outEdges
  }


  void detatchEdgeTo(Tedge<NodeData,EdgeData>* e){
    if (e->to!=NULL)
      for (int i=0;i<e->to->inEdges.size();i++){
	if (e->to->inEdges[i]==e){
	  e->to->inEdges.erase(e->to->inEdges.begin()+i);
	  break;
	}
      }//for inEdges
  }


  void deleteEdge(Tedge<NodeData,EdgeData>* e){
    if (e==NULL) return;
    detatchEdgeFrom(e);
    detatchEdgeTo(e);
    delete e;
  }
  

  void moveEdge(Tedge<NodeData,EdgeData>* e,int from,int to){
    if (e->from==NULL||from!=e->from->id){
      detatchEdgeFrom(e);
      attachEdgeFrom(e,from);
    }
    if (e->to==NULL||to!=e->to->id){
      detatchEdgeTo(e);
      attachEdgeTo(e,to);
    }
  }


  void setNodeData(int id,NodeData* d){
    while (nodes.size()<=id)
      nodes.push_back(new Tnode<NodeData,EdgeData>(NULL,-1));    
    nodes[id]->id=id;
    nodes[id]->data=d;
  }
  std::vector<Tedge<NodeData,EdgeData>*> getEdges(int f,int t){    
     std::vector<Tedge<NodeData,EdgeData>*> r;
     if (f==-1||t==-1) return r;
     for (int i=0;i<nodes[f]->outEdges.size();i++){
       if (nodes[f]->outEdges[i]->to!=NULL&&nodes[f]->outEdges[i]->to->id==t) 
	 r.push_back(nodes[f]->outEdges[i]);
     }
     return r;
  }


  Tedge<NodeData,EdgeData> * getOneEdge(int f,int t){    
    if (f==-1||t==-1||f>=nodes.size()||t>=nodes.size()) return NULL;
    for (int i=0;i<nodes[f]->outEdges.size();i++){
       if (nodes[f]->outEdges[i]->to!=NULL&&nodes[f]->outEdges[i]->to->id==t) 
	 return nodes[f]->outEdges[i];
    }
    return NULL;
  }
  

  bool areLinked(int f,int t){
    return (getOneEdge(f,t)!=NULL);
  }

  NodeData* getLastNodeContent(){
    if (nodes.size()){
      return nodes[nodes.size()-1]->data;
    }
    return NULL;
  }

  //void replaceEdgeData(int f,int t,const EdgeData *d);

 Tgraph():printNodeInfo(NULL),printEdgeInfo(NULL),printGraphHeader(NULL){}

  void (*printNodeInfo)(std::ostream &o,const Tnode<NodeData,EdgeData>* n);
  void (*printEdgeInfo)(std::ostream &o,const Tedge<NodeData,EdgeData>* e);
  void (*printGraphHeader)(std::ostream &o,const Tgraph<NodeData,EdgeData>* g);
  

  void printDotFormat(std::ostream& o){
      o<<"digraph data{";
      if (printGraphHeader!=NULL) printGraphHeader(o,this);
      for (int i=0;i<nodes.size();i++){
    o<<i<<" ";
     if (printNodeInfo!=NULL) printNodeInfo(o,nodes[i]);
    o<<std::endl;
  }//nodes
  for (int i=0;i<nodes.size();i++){
    for (int j=0;j<nodes[i]->outEdges.size();j++){
      o<<nodes[i]->id<<"->"<<(nodes[i]->outEdges[j]->to==NULL?-1:nodes[i]->outEdges[j]->to->id)<<" ";
      if (printEdgeInfo!=NULL) printEdgeInfo(o,nodes[i]->outEdges[j]);
      o<<std::endl;
    }
  }
  o<<"}"<<std::endl; 

  }

  void loadExtendedPathRepresentation(std::istream& is,int ecount=-1){
    EdgeData* e=NULL;
    int count=0;
    int n=0;
    while (is){
      char c;
      //std::cout<<"c"<<c<<std::endl;
      bool eolok=false;      
      while (!eolok){
	c=is.peek();
	//std::cout<<"seen{"<<c<<"}"<<std::endl;
	if ((c==10)||(c==13)){
	  is.read(&c,1);
	  //std::cout<<"ate {"<<c<<"}"<<std::endl;
	}else
	  eolok=true;		      
      }
      //std::cout<<"c is"<<((int)c)<<std::endl;
      if (!('0'<=c&&c<='9'||c==' ')) {
	//std::cout<<"found"<<((int)c)<<std::endl;
	break;
      }else{
	//std::cout<<"digit"<<std::endl;
      }
     
      NodeData* n=new NodeData();
       is>>*n;
       addNode(n);       
       if (e!=NULL){
	 addEdge(count-1,count,e);
       }
       count++;
       std::cout<<"loaded node"<<count<<std::endl;
       //if (count==40) return;
       //if (count==1000) return;
       if (count==ecount) return;
       if (is){
	 e=new EdgeData();
	 is>>*e;
       }//is
    }
  }


  void removeLoops(){
    for (int i=0;i<nodes.size();i++){
      std::vector<Tedge<NodeData,EdgeData>*> aut=getEdges(i,i);
      for (int j=0;j<aut.size();j++)
	deleteEdge(aut[j]);
    }//for i
  }//remove loops


  void multigraphPrune(){
    for (int i=0;i<nodes.size();i++){
      set<int> dsts;      
      int j=0;
      while (j<nodes[i]->outEdges.size()){		
	if (dsts.count(nodes[i]->outEdges[j]->to->id)>0){
	  deleteEdge(nodes[i]->outEdges[j]);
	}
	else{
	  dsts.insert(nodes[i]->outEdges[j]->to->id);
	  j++;
	}
      }//while
    }//for 
  }//multigraphPrune


};

template <typename NodeData,typename EdgeData> std::ostream& operator<<(std::ostream &os,const Tgraph<NodeData,EdgeData> &g){
  os<<g.nodes.size()<<std::endl;
  for (int i=0;i<g.nodes.size();i++){
    os<<*g.nodes[i]<<std::endl;
  }
  for (int i=0;i<g.nodes.size();i++){
    for (int j=0;j<g.nodes[i]->outEdges.size();j++){
      os<<g.nodes[i]->id<<" "<<*g.nodes[i]->outEdges[j]<<std::endl;
    }
  }
  return os;
}

template <typename NodeData,typename EdgeData> std::istream &operator>>(std::istream &is,Tgraph<NodeData,EdgeData> &g){
  int nodes;
  is>>nodes;
  std::string s;
  int id;
  for (int i=0;i<nodes;i++){
    is>>id;
    getline(is,s);
    std::stringstream ss;    
    ss<<s;
    NodeData* data=new NodeData();    
    ss>>*data;
    g.setNodeData(id,data);
  }
  int src;
  int dst;
  while (is){
    is>>src;
    if (!is) break;
    is>>dst;
    getline(is,s);
    std::stringstream ss;    
    ss<<s;
    EdgeData* data=new EdgeData;
    ss>>*data;
    g.addEdge(src,dst,data);
   
  }

  return is;
}

template<typename NodeData,typename EdgeData> class Tpath:public std::vector<int>{   
 public:
  std::vector<EdgeData*> edges;
  
  void addEdge(EdgeData *e,int reached){
    edges.push_back(e);
    push_back(reached);
  }
     
   void loopFreePath(Tpath& p){
     p.clear();
     p.edges.clear();
     if (size()==0) return;
     int currentVal=at(0);     
     int targetVal=at(size()-1);
     int currentPos;

     do{
       p.push_back(currentVal);
       currentPos=size()-1;
       while (at(currentPos)!=currentVal) currentPos--;
       if (currentPos+1>=size()) break;
       p.edges.push_back(edges[currentPos]);
       currentVal=at(currentPos+1);
     }while(true);
     
   }//loopFreePath
   
   void print(std::ostream& os){
     if (size()==0) return;
     os<<at(0);
     for(int i=1;i<size();i++){
       os<<" ";
       if (i-1<edges.size())
	 os<<"="<<edges[i-1]<<"=>";
       os<<" "<<at(i);
     }
     return;
   }

   void subPath(int fromThNode,Tpath &pth){
     pth.clear();
     pth.edges.clear();
     pth.push_back(at(fromThNode));
     
     for (int i=fromThNode+1;i<size();i++){
       pth.push_back(at(i));
       pth.edges.push_back(edges(at(i-1)));
     }
   }//subPath   
};


template <typename NodeData,typename EdgeData> std::ostream& operator<<(std::ostream &os,const Tpath<NodeData,EdgeData> &p){
  if (p.size()==0) return os;
  os<<p.at(0);
  for(int i=1;i<p.size();i++){
    os<<" ";
    if (i-1<p.edges.size())
      os<<" "<<*p.edges[i-1]<<" ";
    os<<" "<<p.at(i);
  }
  return os;
}




// ---------DIJKSTRA BEGIN---------------



template <typename T1, typename T2>
struct pair_first_less{
  bool operator()(std::pair<T1,T2> p1, std::pair<T1,T2> p2) const {
    if(p1.first == p2.first) {
      //Otherwise the initial vertex_queue will have the size 2 { 0,source ; inf;n }
      return p1.second < p2.second;
    }
    return p1.first < p2.first;
  }
};

template <typename NodeData, typename EdgeData>

void dijkstraComputePaths(int source,
			  Tgraph<NodeData,EdgeData> &graph,
                          std::map<int, double>& min_distance,
                          std::map<int, Tedge<NodeData,EdgeData>* > &previous){

  for (int i=0;i<graph.nodes.size();i++){
    min_distance[graph.nodes[i]->id] = std::numeric_limits< double >::infinity();
  }
  
  min_distance[source] = 0;

  std::set< std::pair<double, int>,
    pair_first_less<double, int> > vertex_queue;

  for (int i=0;i<graph.nodes.size();i++){
    for (int j=0;j<graph.nodes[i]->outEdges.size();j++){
      vertex_queue.insert(std::pair<double,int>(min_distance[i],i));
    }
  }
    
  while (!vertex_queue.empty()){
    int u = vertex_queue.begin()->second;
    vertex_queue.erase(vertex_queue.begin());
        
    // Visit each edge exiting u
    for (int i=0;i<graph.nodes[u]->outEdges.size();i++){
      Tedge<NodeData,EdgeData>* edge_iter=graph.nodes[u]->outEdges[i];
      if (edge_iter->to==NULL){
	cerr<<"pending arc from node"<<u<<std::endl;
	continue;
      }
      int v = edge_iter->to->id;
      double weight = edge_iter->data->getCost();

      double distance_through_u = min_distance[u] + weight;
      if (distance_through_u < min_distance[v]) {
	vertex_queue.erase(std::pair<double, int>(min_distance[v], v));
	
	min_distance[v] = distance_through_u;
	previous[v] = edge_iter;
	vertex_queue.insert(std::pair<double, int>(min_distance[v], v));
	
      }

    }
  }
}

template <typename NodeData, typename EdgeData>

std::list<Tedge<NodeData,EdgeData>* > dijkstraGetShortestPath(int source,
					      int target, std::map<int, Tedge<NodeData,EdgeData>* >& previous){
  std::list<Tedge<NodeData,EdgeData>* > path;
  if (previous[target]==NULL||previous[target]->from==NULL) return path;
  int vertex = target;
  while (vertex!=source){
    path.push_front(previous[vertex]);
    vertex=previous[vertex]->from->id;
  }
  return path;
}


template <typename NodeData, typename EdgeData>

std::list<Tedge<NodeData,EdgeData>* > dijkstraGetShortestPath(Tgraph<NodeData,EdgeData>& graph,int source,int target){
  if (source==-1||target==-1){
    std::list<Tedge<NodeData,EdgeData>* > empty;
    return empty;
  }
  std::map<int, double> min_distance;
  std::map<int,Tedge<NodeData,EdgeData>* > previous;
  dijkstraComputePaths(source, graph, min_distance, previous);
  return dijkstraGetShortestPath(source,target,previous);
}


template <typename NodeData,typename EdgeData> std::ostream& operator<<(std::ostream &os,
								   const std::list<Tedge<NodeData,EdgeData>* > path){
  typename std::list<Tedge<NodeData,EdgeData>* >::const_iterator i;
    
  for(i = path.begin(); i != path.end(); ++i){
    os << (*i)->from->id<<" * "<<(*i)->to->id << " ";
  }
  //std::cout << *i;

  /*typename std::list<Tedge<NodeData,EdgeData>* >::iterator path_iter = path.begin();
  for( ; path_iter != path.end(); path_iter++)
    {
    os << (*path_iter)->from->id<<" * "<<(*path_iter)->to->id << " ";
      }*/
  return os;
}

// ---------DIJKSTRA END--------------

};


/*template <typename NodeData,typename EdgeData> class NodeDist{
 public:
  double dist;
  int node;
  bool operator() (const NodeDist<NodeData,EdgeData>& lhs, const NodeDist<NodeData,EdgeData>& rhs) const
  {
    return lhs.dist>rhs.dist;
    }
 NodeDist(double pdist,int pnode):dist(pdist),node(pnode){}
  NodeDist(){}
};


template <typename NodeData,typename EdgeData> void djkstra(int start,int target,Tpath<NodeData,EdgeData> &path){
  priority_queue< NodeDist<NodeData,EdgeData>,std::vector< NodeDist<NodeData,EdgeData> >, NodeDist<NodeData,EdgeData> > queue;
  NodeDist<NodeData,EdgeData> n;
  for (int i=0;i<10;i++){
    n.dist=randu(0,10);
    n.node=(int)randu(0,10);
    queue.push(n);
  }
  
  const NodeDist<NodeData,EdgeData> e;
  for (int i=0;i<10;i++){
    std::cout<<queue.top()<<std::endl;
    queue.pop();
  }

template <typename NodeData,typename EdgeData> std::ostream& operator<<(std::ostream &os,const NodeDist<NodeData,EdgeData> &n){
  os<<n.node<<"@"<<n.dist;
  return os;
}

}*/


#endif
