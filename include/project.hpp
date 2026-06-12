#pragma once
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

class Student;

class Project{
     private:
          int id;
          string name;
          int minGrade;
          int maxStudents;
     public:
          Project();
          Project(int id, string name, int minGrade, int maxStudents);
          int getId();
          string getName();
          int getMinGrade();
          int getMaxStudents();
          vector<int> getStudentsId();
};