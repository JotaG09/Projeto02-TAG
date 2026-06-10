#include <iostream>
#include <bits/stdc++.h>
#include "project.hpp"

using namespace std;

class Student {
     private:
          string name;
          int grade;
          bool free;
          Project** preferences;
          Project* project;
     public:
          Student();
          Student(string name, int grade, vector<int> preferences);
          ~Student();
          string getName();
          int getGrade();
          bool isFree();
          vector<int> getPreferencesId();
          int ProjectId();
};