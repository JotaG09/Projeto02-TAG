#include <iostream>
#include <stack>
#include <queue>
#include <map>
#include <algorithm>
#include <iomanip>
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
    cout << "\n"
         << YELLOW << "==========================================================" << RESET << endl;
    cout << YELLOW << "   INICIANDO EMPARELHAMENTO ESTÁVEL (GALE-SHAPLEY)        " << RESET << endl;
    cout << YELLOW << "==========================================================" << RESET << "\n"
         << endl;

    // Fila de alunos livres que possuem intenção de propor
    queue<Student *> alunosLivres;
    for (auto s : this->students)
    {
        alunosLivres.push(s);
    }

    // Mapeamento para controlar qual a próxima preferência que o aluno vai tentar propor (índice do vetor)
    // Usamos um vetor indexado pelo ID do estudante
    int maxStudentId = 0;
    for (auto s : this->students)
        if (s->getId() > maxStudentId)
            maxStudentId = s->getId();
    vector<size_t> proxPref(maxStudentId + 1, 0);

    int iteracao = 1;

    while (!alunosLivres.empty())
    {
        Student *s = alunosLivres.front();
        alunosLivres.pop();

        // Se o aluno já está alocado ou não tem mais preferências, continua
        if (!s->isFree())
            continue;

        vector<int> prefs = s->getPreferencesId();
        size_t idx = proxPref[s->getId()];

        if (idx >= prefs.size())
            continue; // Esgotou as opções

        int pId = prefs[idx];
        proxPref[s->getId()]++; // Incrementa para a próxima preferência futura

        // Encontra o objeto do projeto
        Project *p = nullptr;
        for (auto proj : this->projects)
        {
            if (proj->getId() == pId)
            {
                p = proj;
                break;
            }
        }
        if (!p)
            continue;

        cout << YELLOW << "[PROPOSTA ATIVA] " << RESET << "Aluno A" << s->getId()
             << " (Nota: " << s->getGrade() << ") propõe para o Projeto P" << p->getId() << endl;

        // Critério 1: Nota mínima de corte do projeto
        if (s->getGrade() < p->getMinGrade())
        {
            cout << RED << "[REJEIÇÃO] " << RESET << "Aluno A" << s->getId()
                 << " não possui a nota mínima exigida (" << p->getMinGrade() << ") pelo Projeto P" << p->getId() << endl;
            alunosLivres.push(s);
            continue;
        }

        vector<int> alocados = p->getStudentsId();

        // Critério 2: Se o projeto ainda possui vagas disponíveis
        if ((int)alocados.size() < p->getMaxStudents())
        {
            p->addStudent(s->getId());
            addEdge(s->getId(), p->getId());
            cout << GREEN << "[EMPARELHAMENTO TEMPORÁRIO] " << RESET << "Aluno A" << s->getId()
                 << " alocado no Projeto P" << p->getId() << " (" << p->getStudentsId().size() << "/" << p->getMaxStudents() << " vagas)" << endl;
        }
        // Critério 3: Projeto cheio. É preciso avaliar substituição por mérito de Nota Agregada
        else
        {
            int piorAlunoId = -1;
            int menorNota = 6; // Notas vão de 3 a 5

            // Encontra quem possui a menor nota dentro do projeto atualmente
            for (int idAlocado : alocados)
            {
                for (auto aloc : this->students)
                {
                    if (aloc->getId() == idAlocado && aloc->getGrade() < menorNota)
                    {
                        menorNota = aloc->getGrade();
                        piorAlunoId = idAlocado;
                    }
                }
            }

            // Se o novo proponente tiver nota estritamente maior que o pior alocado atual
            if (s->getGrade() > menorNota && piorAlunoId != -1)
            {
                cout << RED << "[DESEMPARELHAMENTO] " << RESET << "Aluno A" << piorAlunoId
                     << " (Nota: " << menorNota << ") removido do Projeto P" << p->getId()
                     << " para ceder vaga por mérito superior." << endl;

                // Desfaz a aresta e remove o pior aluno do projeto
                removeEdge(piorAlunoId, p->getId());
                p->removeStudent(piorAlunoId);

                // Coloca o aluno desalocado de volta na fila de livres
                for (auto aloc : this->students)
                {
                    if (aloc->getId() == piorAlunoId)
                    {
                        alunosLivres.push(aloc);
                        break;
                    }
                }

                // Insere o novo aluno com nota maior
                p->addStudent(s->getId());
                addEdge(s->getId(), p->getId());

                cout << GREEN << "[EMPARELHAMENTO TEMPORÁRIO] " << RESET << "Aluno A" << s->getId()
                     << " alocado com sucesso no Projeto P" << p->getId() << endl;
            }
            else
            {
                // Rejeita o proponente atual por falta de vaga/nota competitiva
                cout << RED << "[REJEIÇÃO] " << RESET << "Projeto P" << p->getId()
                     << " já preenchido por candidatos com notas equivalentes ou superiores." << endl;
                alunosLivres.push(s);
            }
        }
    }

    cout << "\n"
         << GREEN << "==========================================================" << RESET << endl;
    cout << GREEN << "   EMPARELHAMENTO ESTÁVEL CONCLUÍDO COM SUCESSO!          " << RESET << endl;
    cout << GREEN << "==========================================================" << RESET << "\n"
         << endl;
}

bool Graph::buscarCaminhoAlternado(Student *s, map<int, bool> &visitados)
{
    // Se o estudante já foi visitado nesta busca, evita loops infinitos
    if (visitados[s->getId()])
        return false;
    visitados[s->getId()] = true;

    // Varre todas as preferências de projetos do estudante
    for (int pId : s->getPreferencesId())
    {
        Project *p = nullptr;
        for (auto proj : this->projects)
        {
            if (proj->getId() == pId)
            {
                p = proj;
                break;
            }
        }

        // Verifica se o projeto existe e se o aluno atende à nota mínima
        if (!p || s->getGrade() < p->getMinGrade())
            continue;

        // CASO 1: O projeto tem uma vaga sobressalente.
        // O caminho aumentante fecha diretamente aqui!
        if ((int)p->getStudentsId().size() < p->getMaxStudents())
        {
            p->addStudent(s->getId());
            addEdge(s->getId(), p->getId());
            return true;
        }

        // CASO 2: O projeto está cheio. Tentamos encontrar um caminho alternado
        // movendo recursivamente algum dos alunos que já estão alocados nele.
        vector<int> alocados = p->getStudentsId();
        for (int idAlocado : alocados)
        {
            Student *aloc = nullptr;
            for (auto stud : this->students)
            {
                if (stud->getId() == idAlocado)
                {
                    aloc = stud;
                    break;
                }
            }
            if (!aloc)
                continue;

            // Chamada recursiva: Tenta achar um novo projeto para o aluno 'aloc'
            if (buscarCaminhoAlternado(aloc, visitados))
            {
                // Se a recursão retornou true, significa que 'aloc' conseguiu uma vaga em OUTRO lugar.
                // Portanto, removemos 'aloc' do projeto atual 'p'...
                removeEdge(aloc->getId(), p->getId());
                p->removeStudent(aloc->getId());

                // ... e colocamos o estudante original 's' no lugar que vagou!
                p->addStudent(s->getId());
                addEdge(s->getId(), p->getId());

                cout << GREEN << "[TRANSFERÊNCIA EM CADEIA] " << RESET
                     << "Aluno A" << aloc->getId() << " remanejado. Aluno A" << s->getId()
                     << " assume vaga no Projeto P" << p->getId() << endl;
                return true;
            }
        }
    }
    return false;
}

void Graph::executarIteracoesCaminhosAlternados()
{
    cout << "\n"
         << YELLOW << "==========================================================" << RESET << endl;
    cout << YELLOW << "   INICIANDO OTIMIZAÇÃO POR CAMINHOS ALTERNADOS (2 A 10)  " << RESET << endl;
    cout << YELLOW << "==========================================================" << RESET << "\n"
         << endl;

    // Executa as iterações de 2 a 10 em laço conforme a especificação do projeto
    for (int i = 2; i <= 10; i++)
    {
        cout << "\n--- ITERAÇÃO " << i << " ---" << endl;
        int novosEmparelhamentos = 0;

        // Para cada iteração, tentamos aumentar o emparelhamento partindo
        // dos estudantes que continuam livres (não emparelhados)
        for (auto s : this->students)
        {
            if (s->isFree())
            {
                // Mapa para marcar quais vértices foram visitados NESTA busca específica
                map<int, bool> visitados;

                cout << YELLOW << "[EXPLORANDO] " << RESET << "Buscando caminho alternado para Aluno Livre A" << s->getId() << "..." << endl;

                if (buscarCaminhoAlternado(s, visitados))
                {
                    novosEmparelhamentos++;
                }
            }
        }

        if (novosEmparelhamentos == 0)
        {
            cout << RED << "[ESTÁVEL] " << RESET << "Nenhum caminho alternado adicional encontrado na Iteração " << i << ". O grafo atingiu a alocação máxima possível." << endl;
            // Opcional: break; se o professor permitir encerrar antes caso estabilize,
            // mas o PDF pede para mostrar as 10 iterações em laço na saída.
        }
        else
        {
            cout << GREEN << "[SUCESSO] " << RESET << "Iteração " << i << " concluída. " << novosEmparelhamentos << " novos alunos alocados via caminhos alternados." << endl;
        }
    }

    cout << "\n"
         << GREEN << "==========================================================" << RESET << endl;
    cout << GREEN << "   TODAS AS ITERAÇÕES CONCLUÍDAS ATÉ A ALOCAÇÃO FINAL!   " << RESET << endl;
    cout << GREEN << "==========================================================" << RESET << "\n"
         << endl;
}

void Graph::print()
{
}
