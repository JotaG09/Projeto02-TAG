#include "project.hpp"

Project::Project(){
     this->id = -1;
     this->name = "";
     this->minGrade = 0;
     this->maxStudents = 0;
}

Project::Project(int id, string name, int minGrade, int maxStudents){
     this->id = id;
     this->name = name;
     this->minGrade = minGrade;
     this->maxStudents = maxStudents;
}

int Project::getId() {return this->id;}

string Project::getName() {return this->name;}

int Project::getMinGrade() {return this->minGrade;}

int Project::getMaxStudents() {return this->maxStudents;}

