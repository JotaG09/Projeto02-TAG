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
        return;

    out << "graph G {\n  rankdir=LR;\n  splines=true;\n";
    out << "  node [fontname=\"Helvetica,Arial\", style=filled];\n\n";

    // Cluster de Projetos
    out << "  subgraph cluster_projetos {\n    label=\"PROJETOS OFERTADOS\";\n";
    out << "    bgcolor=\"#F8FAFC\"; color=\"#CBD5E1\";\n";
    out << "    node [shape=box, fillcolor=\"#DBEAFE\", color=\"#1E3A8A\", fontcolor=\"#1E3A8A\"];\n";

    for (Project *p : this->projects)
    {
        int pId = p->getId();
        int ocupadas = 0;
        Node *curr = this->vertex[OFFSET + pId];
        while (curr != nullptr)
        {
            ocupadas++;
            curr = curr->next;
        }

        if (apenasAlocados && ocupadas == 0)
            continue;
        out << "    P" << pId << " [label=\"P" << pId << " (" << ocupadas << "/" << p->getMaxStudents() << " vagas)\\nMin: " << p->getMinGrade() << "\"];\n";
    }
    out << "  }\n\n";

    // Cluster de Alunos
    out << "  subgraph cluster_alunos {\n    label=\"ALUNOS CANDIDATOS\";\n";
    out << "    bgcolor=\"#F8FAFC\"; color=\"#CBD5E1\";\n";

    for (Student *s : this->students)
    {
        int sId = s->getId();
        bool alocado = (this->vertex[sId] != nullptr);
        if (apenasAlocados && !alocado)
            continue;

        string corFundo = alocado ? "#DCFCE7" : "#FEE2E2";
        string corBorda = alocado ? "#166534" : "#991B1B";
        out << "    A" << sId << " [shape=ellipse, fillcolor=\"" << corFundo << "\", color=\"" << corBorda
            << "\", fontcolor=\"" << corBorda << "\", label=\"A" << sId << " (Nota " << s->getGrade() << ")\"];\n";
    }
    out << "  }\n\n";

    // ========================================================================
    // TAXONOMIA DAS 3 CORES DE ARESTAS (PROPOSTA ATIVA, EMPARELHAMENTO, REJEIÇÃO)
    // ========================================================================
    out << "  // Arestas do Grafo\n";
    for (Student *s : this->students)
    {
        int sId = s->getId();
        int projetoAlocado = (this->vertex[sId] != nullptr) ? this->vertex[sId]->dest : -1;
        if (apenasAlocados && projetoAlocado == -1)
            continue;

        vector<int> prefs = s->getPreferencesId();
        bool achouAlocado = false;

        for (int pId : prefs)
        {
            Project *p = getProject(pId);
            if (!p)
                continue;

            // 1. EMPARELHAMENTO FINAL/TEMPORÁRIO (Verde Sólido)
            if (pId == projetoAlocado)
            {
                out << "  A" << sId << " -- P" << pId << " [color=\"#166534\", penwidth=2.5, label=\"Emparelhado\"];\n";
                achouAlocado = true;
            }
            // 2. REJEIÇÃO (Vermelho Pontilhado)
            else if (!achouAlocado || projetoAlocado == -1)
            {
                string motivo = (s->getGrade() < p->getMinGrade()) ? "Rejeição: Nota < " + to_string(p->getMinGrade()) : "Rejeição: Sem Vaga";
                out << "  A" << sId << " -- P" << pId << " [color=\"#991B1B\", style=\"dotted\", penwidth=1.2, fontcolor=\"#991B1B\", fontsize=9, label=\"" << motivo << "\"];\n";
            }
            // 3. PROPOSTA ATIVA / POTENCIAL (Azul Sólido)
            else
            {
                out << "  A" << sId << " -- P" << pId << " [color=\"#2563EB\", style=\"solid\", penwidth=1.0, fontcolor=\"#2563EB\", fontsize=9, label=\"Proposta Ativa\"];\n";
            }
        }
    }
    out << "}\n";
    out.close();
}

// ============================================================================
// BUSCA DETERMINÍSTICA POR CAMINHO ALTERNADO (RESPEITANDO O HPP)
// ============================================================================
bool Graph::buscarCaminhoAlternado(Student *s, std::map<int, bool> &visitados)
{
    if (!s)
        return false;

    for (int pId : s->getPreferencesId())
    {
        Project *p = getProject(pId);
        if (!p)
            continue;

        // Regra de Negócio: Requisito mínimo de nota é intransponível
        if (s->getGrade() < p->getMinGrade())
            continue;

        // Se o projeto já foi testado nesta tentativa de caminho, pula
        if (visitados[pId])
            continue;
        visitados[pId] = true;

        // Coleta todos os alunos atualmente emparelhados neste projeto
        vector<int> alocados;
        Node *curr = this->vertex[OFFSET + pId];
        while (curr != nullptr)
        {
            alocados.push_back(curr->dest);
            curr = curr->next;
        }

        // CASO 1: O projeto ainda tem vaga ociosa. Achamos o fim do caminho aumentante!
        if ((int)alocados.size() < p->getMaxStudents())
        {
            addEdge(s->getId(), pId);
            return true;
        }

        // CASO 2: Projeto cheio. Tenta deslocar algum aluno já alocado para outra vaga.
        // Ordenamos os alocados por ID para manter o determinismo estrito (sem aleatoriedade)
        sort(alocados.begin(), alocados.end());

        for (int outroAlunoId : alocados)
        {
            Student *outroAluno = getStudent(outroAlunoId);
            if (!outroAluno)
                continue;

            // Desfaz temporariamente o emparelhamento existente
            removeEdge(outroAlunoId, pId);

            // Tenta achar recursivamente um novo lugar para o aluno deslocado
            if (buscarCaminhoAlternado(outroAluno, visitados))
            {
                addEdge(s->getId(), pId); // Sucesso! O novo assume a vaga
                return true;
            }

            // Se o deslocado não achou vaga, revertemos a remoção
            addEdge(outroAlunoId, pId);
        }
    }
    return false;
}

// ============================================================================
// LAÇO DAS 10 ITERAÇÕES OBRIGATÓRIAS DE RE-EMPARELHAMENTO
// ============================================================================
void Graph::executarIteracoesCaminhosAlternados()
{
    cout << "\n"
         << CYAN << BOLD << "============================================================================================" << RESET << endl;
    cout << CYAN << BOLD << "               EVOLUÇÃO DO EMPARELHAMENTO: 10 ITERAÇÕES (CAMINHOS ALTERNADOS)               " << RESET << endl;
    cout << CYAN << BOLD << "============================================================================================" << RESET << "\n"
         << endl;

    for (int iter = 1; iter <= 10; iter++)
    {
        int aumentosNaRodada = 0;

        // 1. Coleta todos os alunos que continuam sem projeto
        vector<Student *> livres;
        for (Student *s : this->students)
        {
            if (this->vertex[s->getId()] == nullptr)
            {
                livres.push_back(s);
            }
        }

        // 2. Ordenação determinística por ID (Garante que não seja aleatório)
        sort(livres.begin(), livres.end(), [](Student *a, Student *b)
             { return a->getId() < b->getId(); });

        // 3. Dispara a busca por caminhos alternados para cada solteiro
        for (Student *s : livres)
        {
            // Inicializa o mapa de visitados para os projetos de 1 a 50
            std::map<int, bool> visitados;
            for (Project *p : this->projects)
            {
                visitados[p->getId()] = false;
            }

            if (buscarCaminhoAlternado(s, visitados))
            {
                aumentosNaRodada++;
            }
        }

        // 4. Auditoria da cardinalidade atual |M|
        int cardinalidadeAtual = 0;
        for (Student *s : this->students)
        {
            if (this->vertex[s->getId()] != nullptr)
                cardinalidadeAtual++;
        }

        cout << BOLD << "Iteração " << setw(2) << setfill('0') << iter << setfill(' ') << RESET
             << " -> Novos caminhos aumentantes: " << (aumentosNaRodada > 0 ? GREEN : YELLOW)
             << setw(2) << aumentosNaRodada << RESET
             << " | Cardinalidade Total |M|: " << GREEN << BOLD << cardinalidadeAtual << RESET << endl;
    }
    cout << "\n"
         << endl;
}