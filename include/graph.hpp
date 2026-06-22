#pragma once
#include <iostream>
#include <bits/stdc++.h>
#include "project.hpp"
#include "student.hpp"

using namespace std;

struct Node {
     int dest;
     Node* next;
};

class Graph{
     private:
          int V;
          vector<Node*> vertex; // Vetor de ponteiros para os nós do grafo, onde cada posição representa um vértice (estudante ou projeto) e aponta para a lista de adjacências daquele vértice
          vector<Student*> students; // Vetor de ponteiros para objetos do tipo Student do grafo
          vector<Project*> projects; // Vetor de ponteiros para objetos do tipo Project do grafo
     public:
          Graph();
          Graph(int vStudents, int vProjects, vector<Student*> students, vector<Project*> projects);
          void addEdge(int studentId, int projectId);
          void removeEdge(int studentId, int projectId);
          void galeShapley();
          bool buscarCaminhoAlternado(Student* s, map<int,bool>& visitados);
          void executarIteracoesCaminhosAlternados();
          void print();
};