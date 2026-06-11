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
          int V = 250;
          Node** vertex;
          Student** students;
          Project** projects;
     public:
          Graph();
          Graph(Student** students, Project** projects);
          void addEdge(int dest, int src);
          void removeEdge(int dest, int src);
          void galeShapley();
          void print();
};