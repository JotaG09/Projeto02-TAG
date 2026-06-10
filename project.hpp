#include <iostream>
#include <bits/stdc++.h>
#include "student.hpp"

using namespace std;

class Project{
     private:
          string name;
          int minGrade;
          int maxStudents;
          Student** students;
     public:
          Project();
          Project(string name, int minGrade, int maxStudents, vector<int> students);
          ~Project();
          string getName();
          int getMinGrade();
          int getMaxStudents();
          bool isFree();
          vector<int> getStudentsId();
          
};