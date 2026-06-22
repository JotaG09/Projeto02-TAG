#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "graph.hpp"
#include "project.hpp"
#include "student.hpp"

using namespace std;

/**
 * Função de leitura blindada contra falhas de formatação.
 * Lê os 50 projetos e os 200 alunos do ficheiro de texto.
 */
void carregar_dados(const string &nome_arquivo, vector<Student *> &lista_alunos, vector<Project *> &lista_projetos)
{
    ifstream file(nome_arquivo);
    if (!file.is_open())
    {
        cerr << "Erro ao abrir o ficheiro: " << nome_arquivo << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        // Ignora linhas vazias ou comentários
        if (line.empty() || line.substr(0, 2) == "//")
        {
            continue;
        }

        // --- 1. LEITURA DOS PROJETOS ---
        // Formato esperado: (P1, 2, 5)
        if (line[0] == '(' && line[1] == 'P')
        {
            stringstream ss(line);
            char lixo;
            int pId, vagas, notaMin;

            // Extrai os valores descartando os parênteses e vírgulas
            ss >> lixo >> lixo >> pId >> lixo >> vagas >> lixo >> notaMin;

            string nomeProjeto = "Projeto " + to_string(pId);
            lista_projetos.push_back(new Project(pId, nomeProjeto, notaMin, vagas));
        }

        // --- 2. LEITURA DOS ALUNOS (VERSÃO BLINDADA) ---
        // Contorna linhas sem espaço como "(A177):(P37, P21, P18)(5)"
        else if (line[0] == '(' && line[1] == 'A')
        {
            string linha_limpa = line;

            // Substitui todos os carateres não-numéricos por espaços em branco
            for (char &c : linha_limpa)
            {
                if (c == '(' || c == ')' || c == 'A' || c == 'P' || c == ':' || c == ',')
                {
                    c = ' ';
                }
            }

            // Agora a linha contém apenas os 5 números limpos (ex: " 177  37  21  18  5 ")
            stringstream ss(linha_limpa);
            int aId, p1, p2, p3, nota;

            // Lê diretamente os inteiros na ordem correta
            if (ss >> aId >> p1 >> p2 >> p3 >> nota)
            {
                vector<int> preferencias = {p1, p2, p3};
                string nomeAluno = "Aluno " + to_string(aId);

                lista_alunos.push_back(new Student(aId, nomeAluno, nota, preferencias));
            }
        }
    }
    file.close();
}

int main()
{
    vector<Student *> lista_alunos;   // Vetor que armazena ponteiros para objetos Student
    vector<Project *> lista_projetos; // Vetor que armazena ponteiros para objetos Project

    string caminho_arquivo = "docs/entradaProj2.26TAG.txt";

    cout << "A carregar dados do ficheiro: " << caminho_arquivo << "..." << endl;
    carregar_dados(caminho_arquivo, lista_alunos, lista_projetos);

    cout << "Alunos carregados: " << lista_alunos.size() << " (Esperado: 200)" << endl;
    cout << "Projetos carregados: " << lista_projetos.size() << " (Esperado: 50)\n"
         << endl;

    // Se a leitura foi bem-sucedida, inicia o motor de Teoria dos Grafos
    if (!lista_alunos.empty() && !lista_projetos.empty())
    {
        // 1. Instancia o Grafo Bipartido
        Graph g(lista_alunos.size(), lista_projetos.size(), lista_alunos, lista_projetos);

        // 2. Executa a Iteração 1: Emparelhamento Estável de Gale-Shapley
        g.galeShapley();

        // 3. Executa as Iterações 2 a 10: Otimização por Caminhos Alternados
        // (Descomente a linha abaixo se tiver implementado este método no graph.cpp)
        // g.executarIteracoesCaminhosAlternados();

        // 4. Imprime a Matriz Final de Ganhos / Perdas no terminal
        g.print();

        // 5. Opcional: Gera o snapshot visual para o Graphviz
        // g.gerarSnapshotVisual("docs/grafo_final.dot");
    }
    else
    {
        cerr << "Erro: Não foi possível inicializar o Grafo devido a dados ausentes." << endl;
    }

    // --- LIBERTAÇÃO DE MEMÓRIA (BOA PRÁTICA OBRIGATÓRIA EM C++) ---
    // Evita memory leaks libertando os objetos alocados com 'new'
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