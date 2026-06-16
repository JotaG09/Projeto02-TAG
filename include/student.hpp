#pragma once
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

class Student
{
private:
    int id;
    string name;
    int grade;
    bool free;
    int allocatedProjectId = -1; // Variável para armazenar o id do projeto em que o estudante está alocado, caso ele não esteja livre
    vector<int> preferences;

public:
    Student();
    Student(int id, string name, int grade, vector<int> preferences);
    void setProject(int projectId);
    int getId();
    string getName();
    int getGrade();
    bool isFree();
    void setFree(bool isFree);
    vector<int> getPreferencesId();
    int ProjectId();
};