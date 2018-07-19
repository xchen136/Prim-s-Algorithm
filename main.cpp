
#include<iostream>
#include<fstream>
using namespace std;

class graphNode{
	private:
		int nodeID;
		graphNode* next;
		
	public:
		graphNode(int n){
			nodeID = n;
			next = NULL;
		}
		
		~graphNode(){
			delete next;
		}
		
		graphNode* getNext(){
			return next;
		}
		
		void setNext(graphNode* n){
			next = n;
		}
		
		int getID(){
			return nodeID;
		}
		
		void printNodes(ofstream& out){
			graphNode* nextN = next;
			while(nextN != NULL){
				out<<nextN->getID()<<"\t";
				nextN = nextN->getNext();
			}
			out<<endl;
		}
		
		void insertN(graphNode* n){												//simply insert newNode after current node 
			graphNode* temp = next;												//gets the address of orig next node
			next = n;															//set next to newNode
			n->setNext(temp);													//set newNode's next to original next, whether f it's NULL
		}
		
		graphNode* deleteN(int x){												//call by head	
			graphNode* temp;
			graphNode* currentN = next;
			if(next == NULL){													//empty 	
				return NULL;
			}
			if(next->getID() == x){												//if first node matches
				temp = next;
				next = next->getNext();											//next updates to be the one after previous next
				return temp;
			}
			while(currentN->getNext() != NULL){
				if(currentN->getNext()->getID() == x){
					temp = currentN->getNext();
					currentN->setNext(currentN->getNext()->getNext());
					return temp;
				}
				else{
					currentN = currentN->getNext();
				}
			}
			return NULL;
		}
		
};

class edgeNode{
	private:
		int Ni;
		int Nj;
		int edgeCost;
		edgeNode* next;
		
	public:
		edgeNode(int i, int j, int cost){
			Ni = i;
			Nj = j;
			edgeCost = cost;
			next = NULL;
		}
		
		~edgeNode(){
			delete next;
		}
		
		int getI(){
			return Ni;
		}
		
		int getJ(){
			return Nj;
		}
		
		int getCost(){
			return edgeCost;
		}
		
		edgeNode* getNext(){
			return next;
		}
		
		void setNext(edgeNode* n){
			next = n;
		}
		
		void printEdgeNodes(ofstream& out){
			edgeNode* nextE = next;
			while(nextE != NULL){
				out<<nextE->getI()<<"\t"<<nextE->getJ()<<"\t"<<nextE->getCost()<<endl;
				nextE = nextE->getNext();
			}
		}
		
		void insertE(edgeNode* edge){											//directly insert newEdge after current edge										
			edgeNode* temp = next;
			next = edge;
			edge->setNext(temp);												//since newNode definitely wont have a next, recursive call will direcly check that and insert temp after newNode
		}
		
		void deleteE(int i, int j){												//call by head
			edgeNode* currentN = next;
			if(next->getNext() == NULL){
				return;
			}
			if(next->getI() == i && next->getJ() == j){
				next = next->getNext();
				return;
			}
			while(currentN->getNext() != NULL){
				if(currentN->getNext()->getI() == i && currentN->getNext()->getJ() == j){
					currentN->setNext(currentN->getNext()->getNext());
					return;
				}
				else{
					currentN = currentN->getNext();
				}
			}
		}
};

class PrimMST{
	private:
		int N;
		int totalCost;
		int* graphNode1Darray;
		edgeNode* edgeList;  													//List of edgeNodes using the head-edgeNode for tracing, initiallly point to a dummy edgeNode												
		graphNode* setA;														//dummy head of setA graphNodes
		graphNode* setB;														//dummy head of setB graphNodes
		edgeNode* MSTofG;														//dummy head of MSTofG edges
		
	public:
		PrimMST(int n){
			N = n;
			graphNode1Darray = new int[N]();
			edgeList = new edgeNode(0, 0, 0);
			setA = new graphNode(0);
			setB = new graphNode(0);
			MSTofG = new edgeNode(0, 0, 0);
			totalCost = 0;
		}
		
		void PrimsMST(ifstream& readFile, ofstream& writeFile1, ofstream& writeFile2){
			int x, y, z;
			edgeNode* edge; 
			graphNode* node;
			
			while(readFile >> x && readFile >> y && readFile >> z){						//3 consecutive integers are Ni, Nj, cost
				edge = new edgeNode(x, y, z);											//create a new edgeNode
				insertionSort(edge);													//insert newEdge into the edgeList
				graphNode1Darray[x-1]++;												//increment the number of edges
				graphNode1Darray[y-1]++;												
			}
			
			printGraphNodes(writeFile2);												//print count of edges for debugging purpose
			writeFile2 << endl;															
			printEdgeList(writeFile2);													//print the first 10 edges for debugging purpose																
			for(int a=0; a<N; a++){
				if(graphNode1Darray[a] > 0){											//check if current node have any edges
					node = new graphNode(a+1);											//if so, create a graphNode for it
					if(setA->getNext() == NULL){										//if setA is empty, insert first node with edges into setA
						setA->insertN(node);											//insert right after setA dummy head (front of list)
					}
					else{
						setB->insertN(node);											//insert rest of the nodes right after setB dummy head (front of list)
					}
				}
			}	

			while(setB->getNext() != NULL){										
				edge = findMinEdge();
				edgeList->deleteE(edge->getI(), edge->getJ());
				MSTofG->insertE(edge);
				totalCost += edge->getCost();
				
				if(inSet(setB, edge->getI())){											//find the node that's in setB			
					node = setB->deleteN(edge->getI());									//if Ni is in setB, delete Ni from setB
				}
				else{
					node = setB->deleteN(edge->getJ());									//else Nj is in setB, delete Ni from setB
				}
				setA->insertN(node);													//insert setB into setA
				
				writeFile2<<"------------------------------------------------------------------------------------------------------------------------------------------------------------"<<endl;
				writeFile2<<"Current Set A: ";
				setA->printNodes(writeFile2);
				writeFile2<<"Current Set B: ";
				setB->printNodes(writeFile2);
				writeFile2<<"Current Minimum Spanning Tree: "<<endl;
				MSTofG->printEdgeNodes(writeFile2);
			}
			writeFile1<<"[Final Minimum Spanning Tree]"<<endl;
			MSTofG->printEdgeNodes(writeFile1);
			writeFile1<<"Total Cost = "<<totalCost;
		}
		
		bool inSameSet(edgeNode* edge){
			graphNode* current = setA->getNext();										//current points to first node after dummy head
			while(current != NULL){														//while current node is not empty
				if((current->getID() == edge->getI()) || (current->getID() == edge->getJ())){	//check if current nodeID is equal to either of Ni/Nj
					current = current->getNext();										//if so, set current to next to check second match
					while(current != NULL){												//while there are more nodes to check
						if((current->getID() == edge->getI()) || (current->getID() == edge->getJ())){	//check if current nodeID is equal to the other of Ni/Nj
							return true;												//if so, return true
						}
						current = current->getNext();									//else, set current to next
					}
					return false;														//if reaches end of list and the other nodeID is still not matched, return false
				} 
				current = current->getNext();											//set current to next to check first match
			}
			return true;																//if none of the nodeID is found in setA, then they must both be in setB
		}
		
		edgeNode* findMinEdge(){
			edgeNode* minEdge = edgeList->getNext();									//initially point to first edge
			while(minEdge != NULL){														
				if(inSameSet(minEdge)){												
					minEdge = minEdge->getNext();										
				}	
				else{
					return minEdge;
				}
			}//check until the end of edgeList to find minEdge
			return NULL;
		}
		
		bool inSet(graphNode* setHead, int nodeID){										//check if nodeID is in specified set
			graphNode* current = setHead->getNext();
			while(current != NULL){
				if(current->getID() == nodeID){
					return true;
				}
				current = current->getNext();
			}
			return false;
		}
		
		void insertionSort(edgeNode* edge){												//find spot and perform insert into an ascending ordered list
			edgeNode* current = edgeList;															//current's next point to the first node																//if list is not empty
			while(current->getNext() != NULL){											//while there is a next
				if(current->getNext()->getCost() < edge->getCost()){        
					current = current->getNext();										//if next cost is also smaller, have current become next
				}
				else{																	//break out of the while loop is no more smaller cost
					break;
				}
			}//after finding the spot to insert newEdge after
			current->insertE(edge);														//insert after current 
		}
		
		void printGraphNodes(ofstream& out){
			out <<"[Number of Edges Per Node]\n";
			for(int i = 0; i < N; i++){
				out << i+1 <<"\t"<<graphNode1Darray[i]<<endl;
			}
		}
		
		void printEdgeList(ofstream& out){
			int count = 0;
			out<<"[First 10 Edges]\n";
			edgeNode* current = edgeList->getNext();
			while(current != NULL && count != 10){										//while current edge is not null, print current edge
				out<<current->getI()<<"\t"<<current->getJ()<<"\t"<<current->getCost()<<endl;
				current = current->getNext();
				count++;																//only print first 10 edges
			}
		}
		
		graphNode* getSetA(){
			return setA;
		}
		
		graphNode* getSetB(){
			return setB;
		}
		
		edgeNode* getEdgeList(){
			return edgeList;
		}
	
};

int main (int argc, char** argv){
	ifstream inFile;
	ofstream outFile1;																	//result
	ofstream outFile2;																	//debugging purpose
	inFile.open(argv[1]);
	outFile1.open(argv[2]);
	outFile2.open(argv[3]);
	int nodes;
	
	//check the number of arguments
	if (argc != 4) {																
        cerr << "Error: Number of arguments unsatisfied."<<endl;
        exit(1);
    }
	outFile2<<"-------------------------------------------------------------------------DEBUGGING--------------------------------------------------------------------------"<<endl;
	
	if(inFile >> nodes){
		PrimMST minST(nodes);
		minST.PrimsMST(inFile, outFile1, outFile2);
	}
	
	inFile.close();
	outFile1.close();
	outFile2.close();
}
