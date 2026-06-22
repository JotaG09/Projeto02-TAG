#include <iostream>
#include <iomanip>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include "graph.hpp"

using namespace std;

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"

// O SEGREDO: Constante para deslocar os Projetos para os índices 201 a 250
const int OFFSET = 200;

Graph::Graph()
{
    this->V = 0;
}

Graph::Graph(int vStudents, int vProjects, vector<Student *> students, vector<Project *> projects)
{
    // Tamanho total precisa abrigar até o índice 250 (200 + 50). Logo, size = 251.
    this->V = OFFSET + vProjects + 1;
    this->vertex = vector<Node *>(this->V, nullptr);

    this->students = students;
    this->projects = projects;
}

// Busca segura do ponteiro do estudante pelo ID real (1 a 200)
Student *Graph::getStudent(int id)
{
    for (auto s : this->students)
    {
        if (s->getId() == id)
            return s;
    }
    return nullptr;
}

// Busca segura do ponteiro do projeto pelo ID real (1 a 50)
Project *Graph::getProject(int id)
{
    for (auto p : this->projects)
    {
        if (p->getId() == id)
            return p;
    }
    return nullptr;
}

void Graph::addEdge(int studentId, int projectId)
{
    int pIndex = OFFSET + projectId; // Projeto P1 vira índice 201, P50 vira 250

    // 1. Nó do Projeto apontando para o ID real do Aluno
    Node *newNode = new Node();
    newNode->dest = studentId;
    newNode->next = this->vertex[pIndex];
    this->vertex[pIndex] = newNode;

    // 2. Nó do Aluno apontando para o ID real do Projeto
    Node *newNode2 = new Node();
    newNode2->dest = projectId;
    newNode2->next = this->vertex[studentId];
    this->vertex[studentId] = newNode2;

    Student *s = getStudent(studentId);
    if (s)
        s->occupy();
}

void Graph::removeEdge(int studentId, int projectId)
{
    int pIndex = OFFSET + projectId;

    // 1. Remove o projeto da lista do estudante
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

    // 2. Remove o estudante da lista do projeto
    current = this->vertex[pIndex];
    prev = nullptr;
    while (current != nullptr && current->dest != studentId)
    {
        prev = current;
        current = current->next;
    }
    if (current != nullptr)
    {
        if (prev == nullptr)
            this->vertex[pIndex] = current->next;
        else
            prev->next = current->next;
        delete current;
    }

    Student *s = getStudent(studentId);
    if (s)
        s->libarate();
}

// Algoritmo de Gale-Shapley Estável (Student-Optimal / Controlado por Vagas)
void Graph::galeShapley()
{
    queue<Student *> alunosLivres;
    for (auto s : this->students)
        alunosLivres.push(s);

    vector<int> proximaPref(201, 0);

    while (!alunosLivres.empty())
    {
        Student *s = alunosLivres.front();
        alunosLivres.pop();

        if (!s->isFree())
            continue;

        vector<int> prefs = s->getPreferencesId();
        int idx = proximaPref[s->getId()];

        if (idx >= (int)prefs.size())
            continue; // Esgotou as 3 preferências

        int pId = prefs[idx];
        proximaPref[s->getId()]++;

        Project *p = getProject(pId);
        if (!p)
            continue;

        // Regra 1: Nota mínima de corte
        if (s->getGrade() < p->getMinGrade())
        {
            alunosLivres.push(s);
            continue;
        }

        // Coleta os alunos atualmente aceitos inspecionando a lista do projeto
        vector<Student *> aceitosAtual;
        Node *curr = this->vertex[OFFSET + pId];
        while (curr != nullptr)
        {
            Student *aloc = getStudent(curr->dest);
            if (aloc)
                aceitosAtual.push_back(aloc);
            curr = curr->next;
        }

        // Regra 2: Tem vaga sobrando?
        if ((int)aceitosAtual.size() < p->getMaxStudents())
        {
            addEdge(s->getId(), pId);
        }
        // Regra 3: Lotado. Trocar se o novo proponente for estritamente melhor que o pior
        else
        {
            Student *piorAluno = nullptr;
            int menorNota = 6;

            for (auto aloc : aceitosAtual)
            {
                if (aloc->getGrade() < menorNota)
                {
                    menorNota = aloc->getGrade();
                    piorAluno = aloc;
                }
            }

            if (piorAluno && s->getGrade() > piorAluno->getGrade())
            {
                removeEdge(piorAluno->getId(), pId);
                alunosLivres.push(piorAluno); // O rebaixado volta pra fila
                addEdge(s->getId(), pId);
            }
            else
            {
                alunosLivres.push(s); // Proposta rejeitada
            }
        }
    }
}

void Graph::print()
{
    cout << "\n"
         << CYAN << BOLD << "============================================================================================" << RESET << endl;
    cout << CYAN << BOLD << "                      MATRIZ FINAL DE EMPARELHAMENTOS (GANHOS / PERDAS)                     " << RESET << endl;
    cout << CYAN << BOLD << "============================================================================================" << RESET << "\n"
         << endl;

    cout << left << setw(12) << "Aluno" << setw(15) << "Proj. Alocado"
         << setw(45) << "Classificação do Aluno (No Projeto)" << "Classificação do Projeto (No Aluno)" << endl;
    cout << string(100, '-') << endl;

    int totalEmparelhados = 0;

    for (auto s : this->students)
    {
        // Se o ponteiro do vértice do aluno é nulo, ele ficou sem projeto
        if (this->vertex[s->getId()] == nullptr)
            continue;

        totalEmparelhados++;
        int pId = this->vertex[s->getId()]->dest; // Lê o ID real do projeto (1 a 50)

        // 1. CLASSIFICAÇÃO DO PROJETO NO ALUNO
        vector<int> prefsAluno = s->getPreferencesId();
        int rankProjetoNoAluno = 0;
        for (size_t i = 0; i < prefsAluno.size(); i++)
        {
            if (prefsAluno[i] == pId)
            {
                rankProjetoNoAluno = i + 1;
                break;
            }
        }

        // 2. CLASSIFICAÇÃO DO ALUNO NO PROJETO
        vector<Student *> candidatosDoProjeto;
        for (auto outroAluno : this->students)
        {
            vector<int> outPrefs = outroAluno->getPreferencesId();
            if (find(outPrefs.begin(), outPrefs.end(), pId) != outPrefs.end())
            {
                candidatosDoProjeto.push_back(outroAluno);
            }
        }

        sort(candidatosDoProjeto.begin(), candidatosDoProjeto.end(), [](Student *a, Student *b)
             {
            if (a->getGrade() != b->getGrade()) return a->getGrade() > b->getGrade();
            return a->getId() < b->getId(); });

        int rankAlunoNoProjeto = 0;
        for (size_t i = 0; i < candidatosDoProjeto.size(); i++)
        {
            if (candidatosDoProjeto[i]->getId() == s->getId())
            {
                rankAlunoNoProjeto = i + 1;
                break;
            }
        }

        string strAluno = "A" + to_string(s->getId());
        string strProject = "P" + to_string(pId);
        string strClassifProjeto = to_string(rankAlunoNoProjeto) + "º (Top " + to_string(rankAlunoNoProjeto) +
                                   " de " + to_string(candidatosDoProjeto.size()) + " na lista P" + to_string(pId) + ")";
        string strClassifAluno = to_string(rankProjetoNoAluno) + "º escolha do Aluno";

        cout << left << setw(12) << strAluno << setw(15) << strProject
             << setw(45) << strClassifProjeto << strClassifAluno << endl;
    }

    cout << string(100, '-') << endl;
    cout << GREEN << BOLD << "Total de alunos alocados com sucesso: " << totalEmparelhados
         << " de " << this->students.size() << " candidatos." << RESET << "\n"
         << endl;
}

void Graph::saveToDot(const string &nome_arquivo, bool apenasAlocados)
{
    ofstream out(nome_arquivo);
    if (!out.is_open())
    {
        cerr << "Erro ao criar o ficheiro DOT: " << nome_arquivo << endl;
        return;
    }

    out << "graph G {\n";
    out << "  rankdir=LR;\n";   // Desenha da Esquerda (Projetos) para a Direita (Alunos)
    out << "  splines=true;\n"; // Linhas curvas e suaves
    out << "  node [fontname=\"Helvetica,Arial\", style=filled];\n\n";

    // ---------------------------------------------------------
    // 1. COLUNA DA ESQUERDA: PROJETOS
    // ---------------------------------------------------------
    out << "  subgraph cluster_projetos {\n";
    out << "    label=\"PROJETOS OFERTADOS\";\n";
    out << "    bgcolor=\"#F8FAFC\"; color=\"#CBD5E1\";\n";
    out << "    node [shape=box, fillcolor=\"#DBEAFE\", color=\"#1E3A8A\", fontcolor=\"#1E3A8A\"];\n";

    for (Project *p : this->projects)
    {
        int pId = p->getId();

        // Conta quantos alunos estão fisicamente ligados a este projeto
        int ocupadas = 0;
        Node *curr = this->vertex[OFFSET + pId];
        while (curr != nullptr)
        {
            ocupadas++;
            curr = curr->next;
        }

        // Se o filtro estiver ativo e o projeto estiver vazio, não desenha
        if (apenasAlocados && ocupadas == 0)
            continue;

        out << "    P" << pId << " [label=\"P" << pId << " (" << ocupadas << "/" << p->getMaxStudents() << " vagas)\"];\n";
    }
    out << "  }\n\n";

    // ---------------------------------------------------------
    // 2. COLUNA DA DIREITA: ALUNOS
    // ---------------------------------------------------------
    out << "  subgraph cluster_alunos {\n";
    out << "    label=\"ALUNOS CANDIDATOS\";\n";
    out << "    bgcolor=\"#F8FAFC\"; color=\"#CBD5E1\";\n";

    for (Student *s : this->students)
    {
        int sId = s->getId();
        bool possuiProjeto = (this->vertex[sId] != nullptr);

        if (apenasAlocados && !possuiProjeto)
            continue;

        if (possuiProjeto)
        {
            // Aluno com vaga -> Elipse Verde
            out << "    A" << sId << " [shape=ellipse, fillcolor=\"#DCFCE7\", color=\"#166534\", fontcolor=\"#166534\", label=\"A" << sId << " (Nota " << s->getGrade() << ")\"];\n";
        }
        else
        {
            // Aluno sem vaga -> Elipse Vermelha
            out << "    A" << sId << " [shape=ellipse, fillcolor=\"#FEE2E2\", color=\"#991B1B\", fontcolor=\"#991B1B\", label=\"A" << sId << " (Sem Vaga)\"];\n";
        }
    }
    out << "  }\n\n";

    // ---------------------------------------------------------
    // 3. ARESTAS (AS LIGAÇÕES DO EMPARELHAMENTO)
    // ---------------------------------------------------------
    out << "  // Conexões de Emparelhamento Estável\n";
    for (Student *s : this->students)
    {
        int sId = s->getId();
        if (this->vertex[sId] != nullptr)
        {
            int pId = this->vertex[sId]->dest; // O destino real do aluno é o ID do projeto
            out << "  A" << sId << " -- P" << pId << " [color=\"#166534\", penwidth=2.0];\n";
        }
    }

    out << "}\n";
    out.close();
}