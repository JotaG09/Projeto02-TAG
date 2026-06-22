#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "graph.hpp"
#include "project.hpp"
#include "student.hpp"

using namespace std;

void carregar_dados(const string &nome_arquivo, vector<Student *> &lista_alunos, vector<Project *> &lista_projetos)
{
    ifstream file(nome_arquivo);
    if (!file.is_open())
    {
        cerr << "Erro ao abrir o arquivo: " << nome_arquivo << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {

        if (line.empty() || line.substr(0, 2) == "//")
        { // Ignora linhas vazias ou comentários
            continue;
        }

        if (line[0] == '(' && line[1] == 'P')
        { // Verifica se a linha é do tipo projeto
            stringstream ss(line);
            char lixo; // Variável para descartar carateres como '(', 'P', ',', ')'
            int pId, vagas, notaMin;

            ss >> lixo >> lixo >> pId >> lixo >> vagas >> lixo >> notaMin; // Lê os dados do projeto, descartando os caracteres desnecessários

            string nomeProjeto = "Projeto " + to_string(pId);
            // Cria o objeto dinamicamente e adiciona ao vetor
            lista_projetos.push_back(new Project(pId, nomeProjeto, notaMin, vagas));
        }

        else if (line[0] == '(' && line[1] == 'A')
        { // Verifica se a linha é do tipo aluno
            stringstream ss(line);
            char lixo; // Variável para descartar carateres como '(', 'A', ',', ')'
            int aId, nota;
            int p1, p2, p3;
            string nomeAluno;

            ss >> lixo >> lixo >> aId >> lixo >> lixo >> lixo;
            ss >> lixo >> p1 >> lixo >> lixo >> p2 >> lixo >> lixo >> p3 >> lixo;
            ss >> lixo >> nota >> lixo;

            vector<int> preferencias = {p1, p2, p3};
            nomeAluno = "Aluno " + to_string(aId);

            // Cria o objeto dinamicamente e adiciona ao vetor
            lista_alunos.push_back(new Student(aId, nomeAluno, nota, preferencias));
        }
    }
    file.close();
}

int main()
{
    vector<Student *> lista_alunos;
    vector<Project *> lista_projetos;

    // 1. Carrega os dados do ficheiro de texto
    carregar_dados("docs/entradaProj2.26TAG.txt", lista_alunos, lista_projetos);

    if (!lista_alunos.empty() && !lista_projetos.empty())
    {
        // 2. Instancia o Grafo passando os vetores carregados
        Graph g(lista_alunos.size(), lista_projetos.size(), lista_alunos, lista_projetos);

        // 3. Executa a Iteração 1 (Algoritmo de Gale-Shapley Estável)
        g.galeShapley();

        // 4. Executa as Iterações 2 a 10 (Maximização por Caminhos Alternados)
        g.executarIteracoesCaminhosAlternados();

        // 5. Exibe a Matriz Final de Ganhos/Perdas (Tabela de Rankings)
        g.print();
    }
    else
    {
        cerr << "Erro: Dados não foram carregados corretamente." << endl;
    }

    // 6. LIBERAÇÃO DE MEMÓRIA (Obrigatório em C++ para evitar memory leak)
    for (auto a : lista_alunos)
    {
        delete a;
    }
    for (auto p : lista_projetos)
    {
        delete p;
    }

    return 0;
}
