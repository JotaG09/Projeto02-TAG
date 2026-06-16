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
          vector<Node*> vertex;
          vector<Student*> students;
          vector<Project*> projects;
     public:
          Graph();
          Graph(int vStudents, int vProjects, vector<Student*> students, vector<Project*> projects);
          void addEdge(int studentId, int projectId);
          void removeEdge(int studentId, int projectId);
          void galeShapley();
          void print();
};