// MontadorAssembler.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "ParseLib.h"
#include "Montador.h"
#include "PreProcessamento.h"
#include "MontadorAssembler.h"

int main(int argc, char* argv[]) {
	int numberOfFiles;
	std::ifstream file1; // Arquivo 1
	std::string nomePrimeiroArquivo;
	std::ifstream file2; // Arquivo 2
	std::string nomeSegundoArquivo;

	std::stringstream conteudoArquivo;
	std::stringstream conteudoSegundoArquivo;

	ParseLib parseLib;
	std::vector<Montador::TokensDaLinha> listaDeTokens;

	// verificar argc, se for = 2, temos dois arquivos
	numberOfFiles = argc - 1;
	std::cout << "Número de arquivos: " << numberOfFiles << std::endl;
	if (numberOfFiles == 2) {  // Dois arquivos
		nomePrimeiroArquivo = argv[1];
		nomeSegundoArquivo = argv[2];

		file1.open(nomePrimeiroArquivo);
		if (file1.is_open() && file1.good()) {
			conteudoArquivo << file1.rdbuf();
			file1.close();
			listaDeTokens = parseLib.parseTokens(conteudoArquivo.str());
			PreProcessamento preprocess = PreProcessamento(listaDeTokens);
			preprocess.primeiraPassagem(listaDeTokens, numberOfFiles);
			preprocess.segundaPassagem(nomePrimeiroArquivo);
			// Primeiro arquivo pronto, vamos limpar as tabelas antes de partir para o próximo
			TabelaLib tabelaLib;
			tabelaLib.esvaziarTabelas();
			listaDeTokens.clear();
			conteudoArquivo.clear();
		}
		else {
			std::cout << "Erro! Arquivo não encontrado!" << std::endl;
			getchar();
			return -1;
		}

		file2.open(nomeSegundoArquivo);
		if (file2.is_open() && file2.good()) {
			conteudoSegundoArquivo << file2.rdbuf();
			file2.close();
			listaDeTokens = parseLib.parseTokens(conteudoSegundoArquivo.str());
			PreProcessamento preprocess = PreProcessamento(listaDeTokens);
			preprocess.primeiraPassagem(listaDeTokens, numberOfFiles);
			preprocess.segundaPassagem(nomeSegundoArquivo);
			file2.close();
			// Primeiro arquivo pronto, vamos limpar as tabelas antes de partir para o próximo
		}
		else {
			std::cout << "Erro! Arquivo não encontrado!" << std::endl;
			getchar();
			return -1;
		}
	}
	else if (numberOfFiles == 1) { // Um arquivo
		nomePrimeiroArquivo = argv[1];
		file1.open(argv[1]);
		if (file1.is_open() && file1.good()) {
			conteudoArquivo << file1.rdbuf();
			listaDeTokens = parseLib.parseTokens(conteudoArquivo.str());
			PreProcessamento preprocess = PreProcessamento(listaDeTokens);
			preprocess.primeiraPassagem(listaDeTokens, numberOfFiles);
			preprocess.segundaPassagem(nomePrimeiroArquivo);
			file1.close();
			// Primeiro arquivo pronto, vamos limpar as tabelas antes de partir para o próximo
			TabelaLib tabelaLib;
			tabelaLib.esvaziarTabelas();
		}
		else {
			std::cout << "Erro! Arquivo não encontrado!" << std::endl;
			getchar();
			return -1;
		}
	}
	else { // Número de parâmetros inválido
		std::cout << "Erro! Número de parâmetros inválido";
		getchar();
		return -1;
	}

	// Preprocessamento.preprocess(file, int number_of_files){ if(number_of_files == 2) {blablabla}


	// TODO: Ler arquivos,
	// fazer o parse de tokens
	// Se for 1 arquivo
	// Pré-Processamento: Verificar se não tem diretivas begin e end, se tiver, dá erro
	// Se forem 2 arquivos
	// Pré processamento: Encontrar as diretivas begin e end, se não encontrar, dar erro
	// Após o pré-processamento, realizar montagem, levando em conta o processo de ligação

	// Ligador:
	// Resolver dependências pendentes 

	// Para realizarmos a ligação corretamente, precisaremos:
	// 2 tabelas de uso e de definição(Uma para cada arquivo)

	std::cout << "Aperte qualquer botão para sair";
	getchar();

	return 0;
}