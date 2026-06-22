#pragma once
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

class Project;
class Student
{
private:
    int id;
    string name;
    int grade;
    bool free;
    vector<int> preferences;
    int allocatedProject = -1;

public:
    Student();
    Student(int id, string name, int grade, vector<int> preferences);
    int getId();
    string getName();
    int getGrade();
    bool isFree();
    vector<int> getPreferencesId();
    void setProject(int projectId);
    int ProjectId();
    void libarate();
    void occupy();
    bool comparePreferences(int currentProject, int proposeProject);
};