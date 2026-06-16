#pragma once
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

class Student
{
private:
    int id; // Id do estudante
    string name;
    int grade; // Nota do estudante
    bool free; // Variável para indicar se o estudante está livre ou alocado em um projeto
    int allocatedProjectId = -1; // Variável para armazenar o id do projeto em que o estudante está alocado, caso ele não esteja livre
    vector<int> preferences; // Este vetor irá armazenar os ids dos projetos preferidos pelo estudante, em ordem de preferência

public:
    Student();
    Student(int id, string name, int grade, vector<int> preferences);
    void setProject(int projectId);
    int getId();
    string getName();
    int getGrade();
    bool isFree();
    void setFree(bool isFree);
    vector<int> getPreferencesId(); // Retorna o vetor de ids dos projetos preferidos pelo estudante, em ordem de preferência
    int getProjectId(); // Retorna o id do projeto em que o estudante está alocado, ou -1 se ele estiver livre
};