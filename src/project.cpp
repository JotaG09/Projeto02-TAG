#include "project.hpp"

Project::Project()
{
    this->id = -1;
    this->name = "";
    this->minGrade = 0;
    this->maxStudents = 0;
}

// This function makes a project object that have a min grade and a max of students
Project::Project(int id, string name, int minGrade, int maxStudents)
{
    this->id = id;
    this->name = name;
    this->minGrade = minGrade;
    this->maxStudents = maxStudents;
}

int Project::getId() { return this->id; }

string Project::getName() { return this->name; }

int Project::getMinGrade() { return this->minGrade; }

int Project::getMaxStudents() { return this->maxStudents; }

vector<int> Project::getStudentsId()
{
    return this->currentStudents; // Retorna o vetor de ids dos estudantes alocados no projeto
}

void Project::addStudent(int studentId)
{
    this->currentStudents.push_back(studentId); // Adiciona o id do estudante ao vetor de ids dos estudantes alocados no projeto
}

void Project::removeStudent(int studentId)
{
    auto it = find(this->currentStudents.begin(), this->currentStudents.end(), studentId); // Encontra o id do estudante no vetor de ids dos estudantes alocados no projeto
    if (it != this->currentStudents.end())
    {                                    // Se o id do estudante for encontrado no vetor
        this->currentStudents.erase(it); // Remove o id do estudante do vetor de ids dos estudantes alocados no projeto
    }
}