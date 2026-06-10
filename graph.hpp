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
          Node** vertex;
          Student** students;
          Projects** projects;
     public:
          Graph();
          Graph(int V);
          void addEdge(int dest, int src);
          void removeEdge(int dest, int src);
          void print();
};