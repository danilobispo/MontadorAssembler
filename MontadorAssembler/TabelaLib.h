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
	EXTERN,
	IF,
	EQU
};

struct InfoDeInstrucoes {
	int numeroDeOperandos;
	int tamanho;
	opcodes opcodesInstrucoes;
};

struct InfoDeDiretivas {
	int numeroDeOperandos;
	int tamanho;
	diretivas diretivasKey;
	bool isPre; //ve se ela é avaliada no pré-processamento
};

struct InfoDeDefinicao {
	int valor;
	InfoDeDefinicao(int valor);
};

struct InfoDeUso {
	std::vector<int> valorList;
	InfoDeUso(int valor);
};

struct InfoDeSimbolo {
	int endereco; // -1 significa endereço indefinido
	int espaco; // -1 significa tamanho indefinido (usado no space)

	bool isConstante; // para saber se o Simbolo é uma constante
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
	void montarTabelaDeDefinicoes();
	bool rotuloJaExistenteNaTabelaDeDefinicoes(std::string rotulo);
	InfoDeDefinicao obtemSimboloNaTabelaDeDefinicoes(std::string id);

	//Tabela de Uso
	void insereSimboloNaTabelaDeUso(std::string, InfoDeUso);
	InfoDeUso obtemSimboloNaTabelaDeUso(std::string id);
	bool rotuloJaExistenteNaTabelaDeUso(std::string id);

	const std::map<std::string, InfoDeDefinicao>& getTabelaDeDefinicoes() const;

	const std::map<std::string, InfoDeSimbolo> &getTabelaDeSimbolos() const;
	const std::map<std::string, InfoDeUso> &getTabelaDeUso() const;

	void setTabelaDeSimbolos(const std::map<std::string, InfoDeSimbolo> &TabelaDeSimbolos);

	void esvaziarTabelaDeSimbolos();

	void esvaziarTabelas();


private:
	// Todas são statics pois usarei em várias partes do código e são de instância única durante a execução
	static std::map<std::string, InfoDeInstrucoes> TabelaDeInstrucoes;
	static std::map<std::string, InfoDeDiretivas> TabelaDeDiretivas;
	static std::map<std::string, InfoDeSimbolo> TabelaDeSimbolos;
	static std::map<std::string, InfoDeDefinicao> TabelaDeDefinicoes;
	static std::map<std::string, InfoDeUso> TabelaDeUso;
};