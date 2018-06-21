#pragma once
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <map>
#include "Montador.h"

// Para essa classe precisamos definir 2 tabelas est�ticas e uma din�mica:
// 1. Tabela de Diretivas
// 2. Tabela de Instru��es
// 3. Tabela de S�mbolos

// Para isso, precisamos de um map para associar as diretivas e outro para as instru��es
// Ap�s isso, precisaremos dizer, para cada instru��o ou diretiva:
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
	EQU,
	IF,
	MACRO,
	ENDMACRO,
};

struct InfoDeInstrucoes {
	int numeroDeOperandos;
	int tamanho;
	opcodes opcodesInstrucoes;
};

struct InfoDeDiretivas {
	int numeroDeOperandos;
	int tamanho;
	diretivas diretivasDiretivas; // nome horr�vel, aceito sugestoes
	bool isPre;
};

struct InfoDeSimbolo {
	int endereco; // -1 significa endere�o indefinido
	int espaco; // -1 significa espa�o indefinido

	bool isConstante; // false funcionar� para
	int valorConstante; // Talvez n�o seja necess�rio, j� que o .o n�o faz c�lculos com o valor absoluto da constante
						// -1 significa valor indefinido
	InfoDeSimbolo(int endereco, int espaco, bool isConstante, int valorConstante);
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

	//Tabela de s�mbolos
	void insereSimboloNaTabelaDeSimbolos(std::string, InfoDeSimbolo);
	InfoDeSimbolo obtemSimboloNaTabelaDeSimbolos(std::string id);
	bool rotuloJaExistenteNaTabelaDeSimbolos(std::string);

	//Def table
	void insereMacroNaTabelaDeDefinicoes(std::string nomeMacro, InfoMacroDef infoMacroDef);
	InfoMacroDef obtemInfoMacroDefNaTabelaDeDefinicoes(std::string id);

	//Name table
	void insereNomeDaMacroNaTabelaDeNomes(std::string nomeMacro, InfoMacroName infoMacroName);
	InfoMacroName obtemInfoMacroNameNaTabelaDeNomes(std::string id);
	bool macroJaExistenteNaTabelaDeNomes(std::string id);

	const std::map<std::string, InfoDeSimbolo> &getTabelaDeSimbolos() const;

	void setTabelaDeSimbolos(const std::map<std::string, InfoDeSimbolo> &TabelaDeSimbolos);

	void esvaziarTabelaDeSimbolosEDeMacros();


private:
	// Todas s�o statics pois usarei em v�rias partes do c�digo e s�o de inst�ncia �nica durante a execu��o
	static std::map<std::string, InfoDeInstrucoes> TabelaDeInstrucoes;
	static std::map<std::string, InfoDeDiretivas> TabelaDeDiretivas;
	static std::map<std::string, InfoDeSimbolo> TabelaDeSimbolos;
	static std::map<std::string, InfoMacroDef> MacroDefinitionTable;
	static std::map<std::string, InfoMacroName> MacroNameTable;
};