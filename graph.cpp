#include <iostream>
#include <stack>
#include <queue>
#include "graph.hpp"

using namespace std;

Graph::Graph(){
     this->V = 0;
     this->vertex = vector<Node*>(V);
     this->students = vector<Student*>(V);
     this->projects = vector<Project*>(V);
}

Graph::Graph(int vStudents, int vProjects, vector<Student*> students, vector<Project*> projects){
     this->V = vStudents + vProjects;
     this->vertex = vector<Node*>(this->V, nullptr);
     this->students = vector<Student*>(vStudents);
     this->projects = vector<Project*>(vProjects);
     for(int j = 0; j < vStudents; j++){
          this->students[j] = students[j];
     }
     for(int k = 0; k < vProjects; k++){
          this->projects[k] = projects[k];
     }
}

void Graph::addEdge(int studentId, int projectId){
     Node* newNode = new Node();
     newNode->dest = studentId;
     newNode->next = this->vertex[projectId];
     this->vertex[projectId] = newNode;

     Node* newNode2 = new Node();
     newNode2->dest = projectId;
     newNode2->next = this->vertex[studentId];
     this->vertex[studentId] = newNode2;
}

