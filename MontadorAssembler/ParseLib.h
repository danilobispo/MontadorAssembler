#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Tokenizador.h"


class ParseLib
{
protected:
	std::vector<std::string> linhasDoCodigo;
	std::string arquivo;
	int contadorPosicao;
	int contadorLinha;
	bool parsingMacro;
	bool sectionText;
	bool sectionData;

public:
	ParseLib();
	~ParseLib();

	void toLower(std::string);

	// Opera��o -p
	void parseDiretivas(std::string fileStream);

	std::string parseLabel(std::string linha);
	std::string parseOperacao(std::string linha, bool hasLabel);
	std::vector<std::string> parseOperando(std::string linha, int numeroDeOperandos, bool hasLabel);

	Tokenizador::TokensDaLinha parseLinha(std::string linha, int linhaContador);

	const std::vector<std::string> &getLinhasDoCodigo() const;

	void setLinhasDoCodigo(const std::vector<std::string> &linhasDoCodigo);
	std::string removeCaracteresEspeciais(std::string str);
	std::string removeEspacosEmBrancoExtras(std::string fileString);
	std::string removeTabulacoes(std::string linha);
	std::string juntaLabelEOperacao(std::string arquivo);
	std::vector<std::string> separaEmLinhas(std::string fileString);
	std::string removeComentarios(std::string linha);
	void preparaCodigo(std::string, const std::string &string, const std::string &basic_string);
	void printLinha(std::string linha);
	int converteOperandoHexaParaInteiro(std::string operando);

	const std::string &getArquivo() const;
	void setArquivo(const std::string &arquivo);

	bool isParsingMacro() const;
	void setParsingMacro(bool parsingMacro);

	bool isSectionText() const;
	void setSectionText(bool sectionText);

	bool isSectionData() const;
	void setSectionData(bool sectionData);

	int converteOperandoParaInteiro(std::string operando);

	bool is_number(const std::string& s);

	std::vector<Tokenizador::TokensDaLinha> parseTokens(std::string arquivoEntradaNome);

	int getContadorPosicao() const;

	void setContadorPosicao(int contadorPosicao);

	int getContadorLinha() const;

	void setContadorLinha(int contadorLinha);
};

bool isNotAlnum(char c);
