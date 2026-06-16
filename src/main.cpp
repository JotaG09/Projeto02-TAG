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

    vector<Student *> lista_alunos;   // Vetor que armazena ponteiros para objetos do tipo Student
    vector<Project *> lista_projetos; // Vetor que armazena ponteiros para objetos do tipo Project

    carregar_dados("docs/entradaProj2.26TAG.txt", lista_alunos, lista_projetos);

    cout << "Alunos carregados:" << lista_alunos.size() << endl;
    cout << "Projetos carregados:" << lista_projetos.size() << endl;

    return 0;
}
