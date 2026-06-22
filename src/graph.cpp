#include <iostream>
#include <stack>
#include <queue>
#include "graph.hpp"

// Códigos de cores ANSI para atender à especificação de saída visual colorida
#define RESET "\033[0m"
#define RED "\033[31m"    // Rejeição / Desemparelhamento
#define GREEN "\033[32m"  // Emparelhamento temporário / definitivo
#define YELLOW "\033[33m" // Proposta ativa

using namespace std;

Graph::Graph()
{
    this->V = 0;
    this->vertex = vector<Node *>(V, nullptr); // Inicializa o vetor de vértices com nullptr, indicando que não há arestas inicialmente
}

// This function makes a graph, with a students, projects, and a list of vertex
Graph::Graph(int vStudents, int vProjects, vector<Student *> students, vector<Project *> projects)
{
    this->V = vStudents + vProjects + 1; // +1 para evitar problemas com IDs baseados em 1
    this->vertex = vector<Node *>(this->V, nullptr);
    this->students = students;
    this->projects = projects;
}

void Graph::addEdge(int studentId, int projectId)
{
    // Adiciona na lista de adjacência do projeto
    Node *newNode = new Node();
    newNode->dest = studentId;
    newNode->next = this->vertex[projectId];
    this->vertex[projectId] = newNode;

    // Adiciona na lista de adjacência do estudante
    Node *newNode2 = new Node();
    newNode2->dest = projectId;
    newNode2->next = this->vertex[studentId];
    this->vertex[studentId] = newNode2;

    // Atualiza o estado do estudante
    for (auto s : students)
    {
        if (s->getId() == studentId)
        {
            s->occupy();
            break;
        }
    }
}

void Graph::removeEdge(int studentId, int projectId)
{
    // 1. Remove da lista do estudante
    Node *current = this->vertex[studentId];
    Node *prev = nullptr;
    while (current != nullptr && current->dest != projectId)
    {
        prev = current;
        current = current->next;
    }
    if (current != nullptr)
    {
        if (prev == nullptr)
            this->vertex[studentId] = current->next;
        else
            prev->next = current->next;
        delete current;
    }

    // 2. Remove da lista do projeto
    current = this->vertex[projectId];
    prev = nullptr;
    while (current != nullptr && current->dest != studentId)
    {
        prev = current;
        current = current->next;
    }
    if (current != nullptr)
    {
        if (prev == nullptr)
            this->vertex[projectId] = current->next;
        else
            prev->next = current->next;
        delete current;
    }

    // Libera o estado do estudante
    for (auto s : students)
    {
        if (s->getId() == studentId)
        {
            s->libarate();
            break;
        }
    }
}

void Graph::galeShapley()
{
}

void Graph::print()
{
}
