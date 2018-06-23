#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Montador.h"
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

	// Operação -p
	void parseDiretivas(std::string fileStream);

	std::string parseLabel(std::string linha);
	std::string parseOperacao(std::string linha, bool hasLabel);
	std::vector<std::string> parseOperando(std::string linha, int numeroDeOperandos, bool hasLabel);

	Montador::TokensDaLinha parseLinha(std::string linha, int linhaContador, int posicaoContador);

	const std::vector<std::string> &getLinhasDoCodigo() const;

	void setLinhasDoCodigo(const std::vector<std::string> &linhasDoCodigo);

	std::vector<Montador::TokensDaLinha> parseTokens(const std::string & arquivoEntradaNome);

	std::string removeEspacosEmBrancoExtras(const std::string &fileString);
	std::string removeTabulacoes(std::string linha);
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

	int getContadorPosicao() const;

	void setContadorPosicao(int contadorPosicao);

	int getContadorLinha() const;

	void setContadorLinha(int contadorLinha);
};

