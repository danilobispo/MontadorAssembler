#pragma once
#include <fstream>
#include "Tokenizador.h"
#include "TabelaLib.h"
#include <unordered_map>

class PreProcessamento {
public:
	PreProcessamento(std::vector<Tokenizador::TokensDaLinha> tokensDaLinhaList);

private:
	std::vector<Tokenizador::TokensDaLinha> tokensDaLinhaList;
public:
	
	void gerarCodigoDeSaidaDiretivas(std::string nomeArquivoSaida);
	void gerarCodigoDeSaidaMacros(std::string nomeArquivoSaida);
	void gerarCodigoDeSaida(std::string nomeArquivoSaida);

	std::vector<Tokenizador::TokensDaLinha> getTokensDaLinhaList();

	void setTokensDaLinhaList(const std::vector<Tokenizador::TokensDaLinha> tokensDaLinhaList);

	std::vector<Tokenizador::TokensDaLinha>
		redefineVariaveisDeMacro(std::string nomeMacro,
			std::vector<Tokenizador::TokensDaLinha> macroLinhas,
			std::vector<std::string> listaDeOperandosMacro, TabelaLib tabelaLib);



	// TODO: Se der tempo, mover isso para montador
	void montarCodigo();
	bool primeiraPassagem(std::vector<Tokenizador::TokensDaLinha> tokensDaLinha);
	void segundaPassagem(std::string nomeArquivoSaida, bool isArquivoModulo);
	void showTabelaDeSimbolos();
	void showTabelaDeDefinicoes();
	void escreveTabelaDeDefinicoesNoArquivoDeSaida(std::string nomeDoArquivo);
	void escreveTabelaDeUsoNoArquivoDeSaida(std::string nomeDoArquivo);
	void escreveMapaDeBitsNoArquivoDeSaida(std::string nomeDoArquivo, std::vector<int> mapaDeBits);
	void escreveTamanhoDoCodigoNoArquivoDeSaida(std::string nomeDoArquivo, int tamanho);
	void escreveCodigoNoArquivoDeSaida(std::string nomeDoArquivo, std::string codigo);
	void showTabelaDeUso();
	void showMapaDeBits(std::vector<int>);
	void showCodigo();
	void gerarCodigoObjeto();
	void analiseLexica();
	void analiseSintatica();

	bool isOperandoNumero(std::string operando);
	bool isOperandoHexa(std::string operando);

	int converteStringParaInt(std::string operando);
	int converteStringHexaParaInt(std::string operando);

	void processarDiretivas(int numeroDeArquivos);

	void montarCodigo(std::string nomeArquivoSaida);
};

