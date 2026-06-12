#include "project.hpp"

Project::Project(){
     this->id = -1;
     this->name = "";
     this->minGrade = 0;
     this->maxStudents = 0;
}

// This function makes a project object that have a min grade and a max of students
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

vector<int> Project::getStudentsId() {
    vector<int> studentsId; // Este vetor irá armazenar os ids dos estudantes alocados no projeto
    for (int i = 0; i < this->maxStudents; i++) { // Loop for para preencher o vetor de ids dos estudantes com -1, indicando que não há estudantes alocados inicialmente
        studentsId.push_back(-1);
    }
    return studentsId;
}

