#include "student.hpp"

Student::Student(){
     this->id = -1;
     this->name = "";
     this->grade = 0;
     this->free = true;
     this->preferences = {};
}

// This function makes a student object that is naturaly free and with a preferences list of projects
Student::Student(int id, string name, int grade, vector<int> preferences){
     this->id = id;
     this->name = name;
     this->grade = grade;
     this->free = true;
     this->preferences = preferences;
}

int Student::getId() {return this->id;}

string Student::getName() {return this->name;}

int Student::getGrade() {return this->grade;}

bool Student::isFree() {return this->free;}

vector<int> Student::getPreferencesId() {return this->preferences;}

int Student::ProjectId() {
    if (!this->free) { // Se o estudante não estiver livre, significa que ele está alocado em um projeto, então retornamos o id do projeto em que ele está alocado
        return this->id;
    }
    return -1;
}
