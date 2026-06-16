#include <iostream>
#include <stack>
#include <queue>
#include "graph.hpp"
#include <algorithm>

using namespace std;

// CÓDIGO DE CORES ANSI PARA O TERMINAL
#define RESET "\033[0m"
#define RED "\033[31m"    // Rejeição
#define GREEN "\033[32m"  // Alocação temporária
#define YELLOW "\033[33m" // Proposta Ativa

Graph::Graph()
{
    this->V = 0;
    this->vertex = vector<Node *>(V);
    this->students = vector<Student *>(V);
    this->projects = vector<Project *>(V);
}

Graph::Graph(int vStudents, int vProjects, vector<Student *> students, vector<Project *> projects)
{
    this->V = vStudents + vProjects;
    this->vertex = vector<Node *>(this->V, nullptr);
    this->students = vector<Student *>(vStudents); // Cria um vetor de ponteiros para objetos do tipo Student do tamanho do número de estudantes
    this->projects = vector<Project *>(vProjects); // Cria um vetor de ponteiros para objetos do tipo Project do tamanho do número de projetos
    for (int j = 0; j < vStudents; j++)
    {
        this->students[j] = students[j]; // Preenche o vetor de estudantes do grafo com os ponteiros para os objetos do tipo Student passados como parâmetro
    }
    for (int k = 0; k < vProjects; k++)
    {
        this->projects[k] = projects[k]; // Preenche o vetor de projetos do grafo com os ponteiros para os objetos do tipo Project passados como parâmetro
    }
}

void Graph::addEdge(int studentId, int projectId)
{
    Node *newNode = new Node();              // Cria um novo nó para representar a aresta entre o estudante e o projeto
    newNode->dest = studentId;               // Define o destino do nó como o id do estudante
    newNode->next = this->vertex[projectId]; // O próximo nó do novo nó é o nó atualmente na posição do projeto no vetor de vértices (pode ser nullptr se não houver nenhum nó lá)
    this->vertex[projectId] = newNode;       // O novo nó se torna o primeiro nó na lista de adjacências do projeto, apontando para o nó que estava lá anteriormente (se houver)

    Node *newNode2 = new Node();              // Cria um novo nó para representar a aresta entre o projeto e o estudante (aresta bidirecional)
    newNode2->dest = projectId;               // Define o destino do nó como o id do projeto
    newNode2->next = this->vertex[studentId]; // O próximo nó do novo nó é o nó atualmente na posição do estudante no vetor de vértices (pode ser nullptr se não houver nenhum nó lá)
    this->vertex[studentId] = newNode2;       // O novo nó se torna o primeiro nó na lista de adjacências do estudante, apontando para o nó que estava lá anteriormente (se houver)
}

void Graph::removeEdge(int studentId, int projectId)
{
    Node *temp = this->vertex[projectId]; // Começa a percorrer a lista de adjacências do projeto para encontrar o nó que representa a aresta com o estudante
    Node *prev = nullptr;                 // Mantém um ponteiro para o nó anterior para facilitar a remoção do nó encontrado
    while (temp != nullptr)
    { // Percorre a lista de adjacências do projeto
        if (temp->dest == studentId)
        { // Se encontrar o nó que representa a aresta com o estudante
            if (prev == nullptr)
            {                                         // Se o nó encontrado for o primeiro nó na lista de adjacências do projeto
                this->vertex[projectId] = temp->next; // O próximo nó se torna o primeiro nó na lista de adjacências do projeto
            }
            else
            {                            // Se o nó encontrado não for o primeiro nó na lista de adjacências do projeto
                prev->next = temp->next; // O próximo nó do nó anterior se torna o próximo nó do nó encontrado, removendo-o da lista de adjacências do projeto
            }
            delete temp; // Libera a memória alocada para o nó encontrado
            break;       // Sai do loop após remover a aresta
        }
        prev = temp;       // Atualiza o ponteiro para o nó anterior
        temp = temp->next; // Move para o próximo nó na lista de adjacências do projeto
    }

    Node *temp2 = this->vertex[studentId]; // Começa a percorrer a lista de adjacências do estudante para encontrar o nó que representa a aresta com o projeto (aresta bidirecional)
    Node *prev2 = nullptr;                 // Mantém um ponteiro para o nó anterior para facilitar a remoção do nó encontrado
    while (temp2 != nullptr)
    { // Percorre a lista de adjacências do estudante
        if (temp2->dest == projectId)
        { // Se encontrar o nó que representa a aresta com o projeto
            if (prev2 == nullptr)
            {                                          // Se o nó encontrado for o primeiro nó na lista de adjacências do estudante
                this->vertex[studentId] = temp2->next; // O próximo nó se torna o primeiro nó na lista de adjacências do estudante
            }
            else
            {                              // Se o nó encontrado não for o primeiro nó na lista de adjacências do estudante
                prev2->next = temp2->next; // O próximo nó do nó anterior se torna o próximo nó do nó encontrado, removendo-o da lista de adjacências do estudante
            }
            delete temp2; // Libera a memória alocada para o nó encontrado
            break;        // Sai do loop após remover a aresta
        }
        prev2 = temp2;       // Atualiza o ponteiro para o nó anterior
        temp2 = temp2->next; // Move para o próximo nó na lista de adjacências do estudante
    }
}

void Graph::galeShapley()
{
    // Implementação do algoritmo de Gale-Shapley para alocação de estudantes em projetos
    // Este método deve ser implementado para realizar a alocação dos estudantes nos projetos com base em suas preferências e nas restrições dos projetos

    cout << "\n===  Iteração 1: Algoritmo de Gale-Shapley inicial ===\n"
         << endl;

    queue<int> AlunosLivres; // Fila para armazenar os ids dos estudantes livres

    for (size_t i = 0; i < this->students.size(); i++)
    {
        AlunosLivres.push(this->students[i]->getId()); // Inicialmente, todos os estudantes estão livres, então adicionamos seus ids à fila
    }

    // Vetor para rastrear a próxima preferência a ser proposta por cada estudante, inicializado com 0 (índice 0, 1 ou 2)
    vector<int> proximaPreferencia(students.size() + 1, 0);

    while (!AlunosLivres.empty())
    {
        int AlunoId = AlunosLivres.front(); // Pega o id do primeiro estudante livre na fila
        AlunosLivres.pop();                 // Remove o estudante da fila

        Student *aluno = nullptr; // Ponteiro para o objeto do tipo Student correspondente ao id do estudante

        // o tipo "auto" é usado para deduzir automaticamente o tipo da variável "s" com base no tipo dos elementos do vetor "students". Neste caso, "s" será deduzido como um ponteiro para um objeto do tipo Student, ou seja, "Student*". Isso permite que o código seja mais conciso e legível, sem a necessidade de declarar explicitamente o tipo da variável "s".
        for (auto s : students) // Percorre o vetor de estudantes para encontrar o objeto do tipo Student correspondente ao id do estudante
        {
            if (s->getId() == AlunoId)
            {
                aluno = s;
                break;
            }
        }
        if (!aluno || !aluno->isFree())
            continue;

        vector<int> preferencias = aluno->getPreferencesId(); // Obtém o vetor de ids dos projetos preferidos pelo estudante, em ordem de preferência

        int indexPreferencia = proximaPreferencia[AlunoId]; // Obtém o índice da próxima preferência a ser proposta pelo estudante

        if (indexPreferencia >= preferencias.size())
        {
            continue; // Se o estudante já tiver proposto para todas as suas 3 preferências, passa para a próxima iteração do loop
        }

        int projetoId = preferencias[indexPreferencia]; // Obtém o id do projeto correspondente à próxima preferência do estudante
        proximaPreferencia[AlunoId]++;                  // Incrementa para a próxima preferência caso seja rejeitado

        Project *projeto = nullptr;

        for (auto p : projects) // Percorre o vetor de projetos para encontrar o objeto do tipo Project correspondente ao id do projeto
        {
            if (p->getId() == projetoId)
            {
                projeto = p;
                break;
            }
        }

        if (!projeto)
            continue;

        cout << YELLOW << "[Proposta Ativa]" << RESET
             << "Aluno A" << AlunoId << "Não possui a nota mínima ("
             << projeto->getMinGrade() << ") para o projeto P" << projetoId << endl;

        AlunosLivres.push(AlunoId); // Se o estudante não atender à nota mínima exigida pelo projeto, ele permanece livre e é adicionado de volta à fila para tentar a próxima preferência
        continue;
    }
}
