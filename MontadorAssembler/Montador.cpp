// MontadorAssembler.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "ParseLib.h"
#include "Tokenizador.h"
#include "PreProcessamento.h"
#include "Montador.h"

int main(int argc, char* argv[]) {
	int numberOfFiles;
	std::ifstream file1; // Arquivo 1
	std::string nomePrimeiroArquivo;
	std::stringstream conteudoArquivo;

	ParseLib parseLib;
	std::vector<Tokenizador::TokensDaLinha> listaDeTokens;
	std::cout << "Bem vindo ao Montador Assembler(Trabalho 1, parte 3.1). " << std::endl
		<< "A analise sintatica (parse) dos arquivos so ocorrera corretamente se operandos com adicao ou subtracao integrada nao estiverem separados por espaco." << std::endl
		<< "EXEMPLO: Um arquivo com o operando 'X+5' vai funcionar, ja um arquivo com o operando escrito como 'X + 5' ira fazer o parse incorretamente" << std::endl
		<< "Verifique se seu arquivo de testes possui algum operando dessa forma e faca a correcao. " << std::endl
		<< "Aperte ENTER para continuar a execucao" << std::endl;
	getchar();

	numberOfFiles = argc - 1;
	if (numberOfFiles == 1) {
		nomePrimeiroArquivo = argv[1];

		file1.open(nomePrimeiroArquivo);
		if (file1.is_open() && file1.good()) {
			conteudoArquivo << file1.rdbuf();
			file1.close();
			listaDeTokens = parseLib.parseTokens(conteudoArquivo.str());
			PreProcessamento preprocess = PreProcessamento(listaDeTokens);
			preprocess.gerarCodigoDeSaidaDiretivas(nomePrimeiroArquivo);
			bool isArquivoModulo = preprocess.primeiraPassagem(listaDeTokens);
			preprocess.segundaPassagem(nomePrimeiroArquivo, isArquivoModulo);
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

	// Ligador:
	// Resolver dependências pendentes 

	// Para realizarmos a ligação corretamente, precisaremos:
	// 2 tabelas de uso e de definição(Uma para cada arquivo)

	std::cout << "Aperte qualquer botão para sair";
	getchar();

	return 0;
}