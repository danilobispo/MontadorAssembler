#pragma once
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <map>
#include "Montador.h"

// Para essa classe precisamos definir 2 tabelas estáticas e uma dinâmica:
// 1. Tabela de Diretivas
// 2. Tabela de Instruções
// 3. Tabela de Símbolos

// Para isso, precisamos de um map para associar as diretivas e outro para as instruções
// Após isso, precisaremos dizer, para cada instrução ou diretiva:
// Operandos
// Tamanho

enum opcodes {
	ADD = 1,
	SUB,
	MULT,
	DIV,
	JMP,
	JMPN,
	JMPP,
	JMPZ,
	COPY,
	LOAD,
	STORE,
	INPUT,
	OUTPUT,
	STOP
};

enum diretivas {
	SECTION,
	SPACE,
	CONST,
	BEGIN,
	END,
	PUBLIC,
	EXTERN
};

struct InfoDeInstrucoes {
	int numeroDeOperandos;
	int tamanho;
	opcodes opcodesInstrucoes;
};

struct InfoDeDiretivas {
	int numeroDeOperandos;
	int tamanho;
	diretivas diretivasDiretivas; // nome horrível, aceito sugestoes
	bool isPre;
};

struct InfoDeDefinicao {
	int valor;
	InfoDeDefinicao(int valor);
};

struct InfoDeUso {
	std::vector<int> enderecoList;
};

struct InfoDeSimbolo {
	int endereco; // -1 significa endereço indefinido
	int espaco; // -1 significa espaço indefinido

	bool isConstante; // false funcionará para
	int valorConstante; // Talvez não seja necessário, já que o .o não faz cálculos com o valor absoluto da constante
						// -1 significa valor indefinido
	bool isExtern;
	InfoDeSimbolo(int endereco, int espaco, bool isConstante, int valorConstante, bool isExtern);
};
struct InfoMacroDef {
	int linha;
	std::vector<Montador::TokensDaLinha> tokensDaLinha;

	InfoMacroDef(int linha, const std::vector<Montador::TokensDaLinha> &tokensDaLinha);
};

struct InfoMacroName {
	int numeroDeArgumentos;
	int linha;

	InfoMacroName(int numeroDeArgumentos, int linha);
};

class TabelaLib {
public:
	bool isDiretiva(std::string operacao);
	InfoDeDiretivas getDiretiva(std::string operacao);
	bool isInstrucao(std::string operacao);
	InfoDeInstrucoes getInstrucao(std::string operacao);

	//Tabela de símbolos
	void insereSimboloNaTabelaDeSimbolos(std::string, InfoDeSimbolo);
	InfoDeSimbolo obtemSimboloNaTabelaDeSimbolos(std::string id);
	void modificaSimboloNaTabelaDeSimbolos(std::string id, InfoDeSimbolo infoSimbolo);
	bool rotuloJaExistenteNaTabelaDeSimbolos(std::string);

	//Tabela de definições
	void insereSimboloNaTabelaDeDefinicoes(std::string, InfoDeDefinicao);


	//Tabela de uso


	const std::map<std::string, InfoDeSimbolo> &getTabelaDeSimbolos() const;

	void setTabelaDeSimbolos(const std::map<std::string, InfoDeSimbolo> &TabelaDeSimbolos);

	void esvaziarTabelaDeSimbolos();


private:
	// Todas são statics pois usarei em várias partes do código e são de instância única durante a execução
	static std::map<std::string, InfoDeInstrucoes> TabelaDeInstrucoes;
	static std::map<std::string, InfoDeDiretivas> TabelaDeDiretivas;
	static std::map<std::string, InfoDeSimbolo> TabelaDeSimbolos;
	std::map<std::string, InfoDeDefinicao> TabelaDeDefinicoes;
	std::map<std::string, InfoDeUso> TabelaDeUso;
};