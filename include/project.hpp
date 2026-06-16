#pragma once
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

class Student;

class Project
{
private:
    int id;
    string name;
    int minGrade; // Nota mínima exigida para um estudante ser alocado no projeto
    int maxStudents; // Número máximo de estudantes que podem ser alocados no projeto
    vector<int> currentStudents; // Este vetor irá armazenar os ids dos estudantes alocados no projeto
public:
    Project();
    Project(int id, string name, int minGrade, int maxStudents);
    int getId();
    string getName();
    int getMinGrade();
    int getMaxStudents();
    void addStudent(int studentId);
    void removeStudent(int studentId);
    vector<int> getStudentsId();
};