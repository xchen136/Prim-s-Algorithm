
import java.util.Scanner;
import java.io.*;

class graphNode{
	private int nodeID;
	private graphNode next;
	
	graphNode(int n){
		nodeID = n;
		next = null;
	}
	
	public graphNode getNext(){
		return next;
	}
	
	public void setNext(graphNode n){
		next = n;
	}
	
	public int getID(){
		return nodeID;
	}
	
	public void printNodes(FileWriter out){
		PrintWriter write = new PrintWriter(out);
		graphNode nextN = next;
		while(nextN != null){
			write.print(nextN.getID()+"\t");
			nextN = nextN.getNext();
		}
		write.println();
	}
	
	public void insertN(graphNode n){
		graphNode temp = next;
		next = n;
		n.setNext(temp);
	}
	
	public graphNode deleteN(int x){
		graphNode temp;
		graphNode currentN = next;
		if(next == null){
			return null;
		}
		if(next.getID() == x){
			temp = next;
			next = next.getNext();
			return temp;
		}
		while(currentN.getNext() != null){
			if(currentN.getNext().getID() == x){
				temp = currentN.getNext();
				currentN.setNext(currentN.getNext().getNext());
				return temp;
			}
			else{
				currentN = currentN.getNext();
			}
		}
		return null;
	}
	
}		

class edgeNode{
	private int Ni;
	private int Nj;
	private int edgeCost;
	private edgeNode next;
	
	edgeNode(int i, int j, int cost){
		Ni = i;
		Nj = j;
		edgeCost = cost;
		next = null;
	}
	
	public int getI(){
		return Ni;
	}
	
	public int getJ(){
		return Nj;
	}
	
	public int getCost(){
		return edgeCost;
	}
	
	public edgeNode getNext(){
		return next;
	}
	
	public void setNext(edgeNode n){
		next = n;
	}
	
	public void printEdgeNodes(FileWriter out){
		PrintWriter write = new PrintWriter(out);
		edgeNode nextE = next;
		while(nextE != null){
			write.println(nextE.getI() + "\t" + nextE.getJ() + "\t" + nextE.getCost() + "\n");
			nextE = nextE.getNext();
		}
	}
	
	public void insertE(edgeNode edge){											//directly insert newEdge after current edge										
		edgeNode temp = next;
		next = edge;
		edge.setNext(temp);												//since newNode definitely wont have a next, recursive call will direcly check that and insert temp after newNode
	}
	
	public void deleteE(int i, int j){												//call by head
		edgeNode currentN = next;
		if(next.getNext() == null){
			return;
		}
		if(next.getI() == i && next.getJ() == j){
			next = next.getNext();
			return;
		}
		while(currentN.getNext() != null){
			if(currentN.getNext().getI() == i && currentN.getNext().getJ() == j){
				currentN.setNext(currentN.getNext().getNext());
				return;
			}
			else{
				currentN = currentN.getNext();
			}
		}
	}
	
}

class PrimMST{
	private int N;
	private int totalCost;
	private int[] graphNode1Darray;
	private edgeNode edgeList;
	private graphNode setA;
	private graphNode setB;
	private edgeNode MSTofG;
	
	PrimMST(){
		edgeList = new edgeNode(0, 0, 0);
		setA = new graphNode(0);
		setB = new graphNode(0);
		MSTofG = new edgeNode(0, 0, 0);
		totalCost = 0;
	}
	
	public void PrimsMST(File readFile, FileWriter writeFile1, FileWriter writeFile2)throws FileNotFoundException{
		Scanner read = new Scanner(readFile);
		PrintWriter out1 = new PrintWriter(writeFile1);
		PrintWriter out2 = new PrintWriter(writeFile2);
		int x, y, z;
		edgeNode edge; 
		graphNode node;
		
		if(read.hasNext()){
			N = read.nextInt();
			graphNode1Darray = new int[N];
		}
		
		while(read.hasNext()){						//3 consecutive integers are Ni, Nj, cost
			x = read.nextInt();
			y = read.nextInt();
			z = read.nextInt();
			edge = new edgeNode(x, y, z);											//create a new edgeNode
			insertionSort(edge);													//insert newEdge into the edgeList
			graphNode1Darray[x-1]++;												//increment the number of edges
			graphNode1Darray[y-1]++;												
		}
		
		printGraphNodes(writeFile2);												//print count of edges for debugging purpose
		out2.println();															
		printEdgeList(writeFile2);													//print the first 10 edges for debugging purpose																	
		for(int a=0; a<N; a++){
			if(graphNode1Darray[a] > 0){											//check if current node have any edges
				node = new graphNode(a+1);											//if so, create a graphNode for it
				if(setA.getNext() == null){										//if setA is empty, insert first node with edges into setA
					setA.insertN(node);											//insert right after setA dummy head (front of list)
				}
				else{
					setB.insertN(node);											//insert rest of the nodes right after setB dummy head (front of list)
				}
			}
		}	

		while(setB.getNext() != null){										
			edge = findMinEdge();
			edgeList.deleteE(edge.getI(), edge.getJ());
			MSTofG.insertE(edge);
			totalCost += edge.getCost();
			
			if(inSet(setB, edge.getI())){											//find the node that's in setB			
				node = setB.deleteN(edge.getI());									//if Ni is in setB, delete Ni from setB
			}
			else{
				node = setB.deleteN(edge.getJ());									//else Nj is in setB, delete Ni from setB
			}
			setA.insertN(node);													//insert setB into setA
			
			out2.println("------------------------------------------------------------------------------------------------------------------------------------------------------------");
			out2.print("Current Set A: ");
			setA.printNodes(writeFile2);
			out2.print("Current Set B: ");
			setB.printNodes(writeFile2);
			out2.println("Current Minimum Spanning Tree: ");
			MSTofG.printEdgeNodes(writeFile2);
		}
		out1.println("[Final Minimum Spanning Tree]");
		MSTofG.printEdgeNodes(writeFile1);
		out1.println("Total Cost = " + totalCost);
		
		read.close();
		out1.close();
		out2.close();
	}
	
	public boolean inSameSet(edgeNode edge){
		graphNode current = setA.getNext();										//current points to first node after dummy head
		while(current != null){														//while current node is not empty
			if((current.getID() == edge.getI()) || (current.getID() == edge.getJ())){	//check if current nodeID is equal to either of Ni/Nj
				current = current.getNext();										//if so, set current to next to check second match
				while(current != null){												//while there are more nodes to check
					if((current.getID() == edge.getI()) || (current.getID() == edge.getJ())){	//check if current nodeID is equal to the other of Ni/Nj
						return true;												//if so, return true
					}
					current = current.getNext();									//else, set current to next
				}
				return false;														//if reaches end of list and the other nodeID is still not matched, return false
			} 
			current = current.getNext();											//set current to next to check first match
		}
		return true;																//if none of the nodeID is found in setA, then they must both be in setB
	}
	
	public edgeNode findMinEdge(){
		edgeNode minEdge = edgeList.getNext();									//initially point to first edge
		while(minEdge != null){														
			if(inSameSet(minEdge)){												
				minEdge = minEdge.getNext();										
			}	
			else{
				return minEdge;
			}
		}//check until the end of edgeList to find minEdge
		return null;
	}
	
	public boolean inSet(graphNode setHead, int nodeID){										//check if nodeID is in specified set
		graphNode current = setHead.getNext();
		while(current != null){
			if(current.getID() == nodeID){
				return true;
			}
			current = current.getNext();
		}
		return false;
	}
	
	public void insertionSort(edgeNode edge){												//find spot and perform insert into an ascending ordered list
		edgeNode current = edgeList;															//current's next point to the first node																//if list is not empty
		while(current.getNext() != null){											//while there is a next
			if(current.getNext().getCost() < edge.getCost()){        
				current = current.getNext();										//if next cost is also smaller, have current become next
			}
			else{																	//break out of the while loop is no more smaller cost
				break;
			}
		}//after finding the spot to insert newEdge after
		current.insertE(edge);														//insert after current 
	}
	
	public void printGraphNodes(FileWriter write){
		PrintWriter out = new PrintWriter(write);
		out.println("[Number of Edges Per Node]\n");
		for(int i = 0; i < N; i++){
			out.println(i+1 + "\t" + graphNode1Darray[i]);
		}
	}
	
	public void printEdgeList(FileWriter write){
		PrintWriter out = new PrintWriter(write);
		int count = 0;
		out.println("[First 10 Edges]\n");
		edgeNode current = edgeList.getNext();
		while(current != null && count != 10){										//while current edge is not null, print current edge
			out.println(current.getI() + "\t" + current.getJ() + "\t" + current.getCost());
			current = current.getNext();
			count++;																//only print first 10 edges
		}
	}
	
	public graphNode getSetA(){
		return setA;
	}
	
	public graphNode getSetB(){
		return setB;
	}
	
	public edgeNode getEdgeList(){
		return edgeList;
	}
	
}

public class main {
	
	public static void main(String[] args)throws IOException{
		File inFile = new File(args[0]);
		FileWriter outFile1 = new FileWriter(args[1]);														//gets output file #1
		FileWriter outFile2 = new FileWriter(args[2]);

		PrimMST minST =  new PrimMST();
		minST.PrimsMST(inFile, outFile1, outFile2);
	}
		
}
