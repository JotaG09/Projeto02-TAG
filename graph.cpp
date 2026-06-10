#include <iostream>
#include <stack>
#include <queue>
#include "graph.hpp"

using namespace std;

Graph::Graph(){
     this->V = 0;
     this->vertex = nullptr;
}

Graph::Graph(int V){
     this->V = V;
     this->vertex = new Node*[this->V](); 
     for(int i = 0; i < this->V; i++){
          this->vertex[i] = nullptr;
     }
}

void Graph::addEdge(int dest, int src){
     Node* newNode = new Node();
     newNode->dest = dest;
     newNode->next = this->vertex[src];
     this->vertex[src] = newNode;

     Node* newNode2 = new Node();
     newNode2->dest = src;
     newNode2->next = this->vertex[dest];
     this->vertex[dest] = newNode2;
}