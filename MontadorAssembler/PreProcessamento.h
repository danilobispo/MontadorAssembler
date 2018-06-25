#pragma once
#include <fstream>
#include "Montador.h"
#include "TabelaLib.h"
#include <unordered_map>

class PreProcessamento {
public:
	PreProcessamento(const std::vector<Montador::TokensDaLinha> &tokensDaLinhaList);

private:
	std::vector<Montador::TokensDaLinha> tokensDaLinhaList;
public:
	
	void gerarCodigoDeSaidaDiretivas(std::string nomeArquivoSaida);
	void gerarCodigoDeSaidaMacros(std::string nomeArquivoSaida);
	void gerarCodigoDeSaida(std::string nomeArquivoSaida);

	const std::vector<Montador::TokensDaLinha> &getTokensDaLinhaList() const;

	void setTokensDaLinhaList(const std::vector<Montador::TokensDaLinha> &tokensDaLinhaList);

	std::vector<Montador::TokensDaLinha>
		redefineVariaveisDeMacro(std::string nomeMacro,
			std::vector<Montador::TokensDaLinha> macroLinhas,
			std::vector<std::string> listaDeOperandosMacro, TabelaLib tabelaLib);



	// TODO: Se der tempo, mover isso para montador
	void montarCodigo();
	void primeiraPassagem(std::vector<Montador::TokensDaLinha> tokensDaLinha, int numeroDeArquivos);
	void segundaPassagem(std::string nomeArquivoSaida);
	void showTabelaDeSimbolos();
	void showTabelaDeDefinicoes();
	void escreveTabelaDeDefinicoesNoArquivoDeSaida(std::string nomeDoArquivo);
	void escreveTabelaDeUsoNoArquivoDeSaida(std::string nomeDoArquivo);
	void escreveMapaDeBitsNoArquivoDeSaida(std::string nomeDoArquivo, std::vector<int> mapaDeBits);
	void escreveTamanhoDoCodigoNoArquivoDeSaida(std::string nomeDoArquivo, int tamanho);
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

