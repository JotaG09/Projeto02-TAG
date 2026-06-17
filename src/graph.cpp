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

// This function makes a graph, with a students, projects, and a list of vertex
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

     (this->students[studentId])->occupy();

     Node* newNode2 = new Node();
     newNode2->dest = projectId;
     newNode2->next = this->vertex[studentId];
     this->vertex[studentId] = newNode2;
}

void Graph::removeEdge(int studentId, int projectId){
     (this->vertex[studentId])->next = nullptr;
     (this->students[studentId])->libarate();
     Node* current = this->vertex[projectId];
     while(current != nullptr){
          if((current->next)->dest == projectId){
               Node* newNode = current->next;
               current->next = newNode->next;
               delete newNode;
          }
          current = current->next;
     }
}

void Graph::galeShapley(){
     stack<Project*> projectsFree;
     vector<Student*> studentsFree = this->students;
     for(size_t i = 0; i < (this->projects).size(); i++){
          projectsFree.push(this->projects[i]);
     }
     Project* p;
     Student* s;
     int currentProjectId;
     int minGrade;
     int maxStudents;
     size_t j;
     while(!projectsFree.empty()){
          p = projectsFree.top();
          minGrade = p->getMinGrade();
          maxStudents = p->getMaxStudents();
          for( j = 0; j < studentsFree.size(); j++){
               s = studentsFree[j];
               if(s->getGrade() < minGrade) continue;
               if(s->isFree()){
                    addEdge(s->getId(), p->getId());
               }
               else {
                    currentProjectId = ((this->vertex[s->getId()])->next)->dest;
                    if(s->comparePreferences(currentProjectId, p->getId())){
                         // If the student preferer the new project than the current project
                         removeEdge(s->getId(), currentProjectId);
                         addEdge(s->getId(), p->getId());
                    }
                    else continue;
               }
          }
     }
}

void Graph::print(){

}
