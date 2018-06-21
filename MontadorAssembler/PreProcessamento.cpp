#include "PreProcessamento.h"
#include <algorithm>
#include "Montador.h"
#include "ParseLib.h"
#include "TabelaLib.h"
#include "ErrorLib.h"
#include <utility>
#include <iostream>
#include <unordered_map>
#include <map>
#include <sstream>
#include <cctype>

PreProcessamento::PreProcessamento(const std::vector<Montador::TokensDaLinha> &tokensDaLinhaList) : tokensDaLinhaList(
	tokensDaLinhaList) {}

const std::vector<Montador::TokensDaLinha> &PreProcessamento::getTokensDaLinhaList() const {
	return tokensDaLinhaList;
}

void PreProcessamento::setTokensDaLinhaList(const std::vector<Montador::TokensDaLinha> &tokensDaLinhaList) {
	PreProcessamento::tokensDaLinhaList = tokensDaLinhaList;
}

void PreProcessamento::gerarCodigoDeSaidaDiretivas(std::string nomeArquivoSaida) {
	gerarCodigoDeSaida(nomeArquivoSaida + ".pre");
}

void PreProcessamento::gerarCodigoDeSaidaMacros(std::string nomeArquivoSaida) {
	gerarCodigoDeSaida(nomeArquivoSaida + ".mcr");
}

void PreProcessamento::gerarCodigoDeSaida(std::string nomeArquivoSaida) {
	std::vector<Montador::TokensDaLinha> tokensDaLinha = getTokensDaLinhaList();
	std::ofstream arquivoSaida(nomeArquivoSaida);
	for (const auto &e : tokensDaLinha) {
		if (e.label != "") {
			arquivoSaida << e.label << ": ";
		}
		arquivoSaida << e.operacao << " ";
		if (e.operando.size() != 0) {
			if (e.operando.size() == 1) {
				arquivoSaida << e.operando[0] << "\n";
			}
			else { // COPY E MACRO
				for (unsigned int j = 0; j < e.operando.size(); j++) {
					if (j == e.operando.size() - 1) {
						arquivoSaida << e.operando[j] << "\n";
					}
					else {
						arquivoSaida << e.operando[j] << ",";
					}

				}
			}
		}
		else {
			arquivoSaida << "\n";
		}
	}
	arquivoSaida.close();
}

std::vector<Montador::TokensDaLinha> PreProcessamento::redefineVariaveisDeMacro(
	std::string nomeMacro,
	std::vector<Montador::TokensDaLinha> macroLinhas,
	std::vector<std::string> listaDeOperandosMacro,
	TabelaLib tabelaLib) {
	std::map<std::string, std::string> variaveis;
	for (unsigned int i = 0; i < listaDeOperandosMacro.size(); i++) {
		variaveis[listaDeOperandosMacro[i]] = "#" + std::to_string(i);
	}

	for (auto &macroLinha : macroLinhas) {
		if (!macroLinha.operando.empty()) {
			for (unsigned int k = 0; k < macroLinha.operando.size(); k++) {
				macroLinha.operando[k] = variaveis.at(macroLinha.operando[k]);
			}
		}
	}

	return macroLinhas;
}

/**
* Aqui realizamos a montagem do código, que vai sofrer uma análise léxica
*/
void PreProcessamento::montarCodigo(std::string nomeArquivoSaida) {
	std::vector<Montador::TokensDaLinha> tokensDaLinha = getTokensDaLinhaList();
	primeiraPassagem(tokensDaLinha, nomeArquivoSaida);
}

void PreProcessamento::primeiraPassagem(std::vector<Montador::TokensDaLinha> tokensDaLinha, std::string nomeArquivoSaida) {
	TabelaLib tabelaLib;
	int contadorLinha = 1;
	int contadorPosicao = 0;
	bool isSectionText = false;
	bool isSectionData = false;


	for (unsigned int i = 0; i < tokensDaLinha.size(); i++) {
		std::string label = tokensDaLinha[i].label;
		std::string operacao = tokensDaLinha[i].operacao;
		std::vector<std::string> operando = tokensDaLinha[i].operando;
		int numeroDaLinha = tokensDaLinha[i].numeroDaLinha;
		std::string::size_type numeroDeOperandos = operando.size();
		InfoDeInstrucoes infoDeInstrucoes;
		InfoDeDiretivas infoDeDiretivas;

		// Procura se label existe, caso exista, devemos procurar pelo label na tabela de simbolos
		// Rótulo:
		if (!label.empty()) {
			if (!tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(label)) {
				tabelaLib.insereSimboloNaTabelaDeSimbolos(label, InfoDeSimbolo(contadorPosicao, -1, false, -1));
			}
			else {
				ErrorLib errorLib(contadorLinha, "Redeclaração de rótulo " + label, "Léxico");
			}
		}

		// Operação
		// Se for instrução
		if (tabelaLib.isInstrucao(operacao)) {
			if (isSectionData) {
				ErrorLib errorLib(contadorLinha, "Instrucao em local incorreto!", "Léxico");
			}
			infoDeInstrucoes = tabelaLib.getInstrucao(operacao);
			// Checa número de operandos:
			if (numeroDeOperandos != infoDeInstrucoes.numeroDeOperandos) {
				ErrorLib errorLib(contadorLinha, "Número de operandos incorreto!", "Sintático");
			}
			contadorPosicao += infoDeInstrucoes.tamanho;
		}
		else if (tabelaLib.isDiretiva(operacao)) {
			infoDeDiretivas = tabelaLib.getDiretiva(operacao);
			// As diretivas IF e EQU já foram descartadas no processamento de diretivas
			// As macros já foram resolvidas no processamento de macros
			// Logo, só sobram 3 diretivas a serem resolvidas: SPACE CONST e SECTION
			if (operacao == "section") {
				contadorPosicao += infoDeDiretivas.tamanho;
				if (operando[0] == "text") {
					isSectionText = true;
				}
				else if (operando[0] == "data") {
					if (!isSectionText) {
						ErrorLib errorLib(contadorLinha, "Section data declarada antes da seção text!", "Léxico");
					}
					else {
						isSectionText = false;
						isSectionData = true;
					}
				}
			}
			if (operacao == "space") {
				if (isSectionData) {
					if (isOperandoNumero(operando[0])) {
						int numeroOperando = converteStringParaInt(operando[0]);
						tabelaLib.insereSimboloNaTabelaDeSimbolos(
							label, InfoDeSimbolo(contadorPosicao, numeroOperando, false, 0));
						contadorPosicao += numeroOperando;
					}
					else {
						ErrorLib errorLib(contadorLinha, "Operando incorreto para instrução SPACE", "Sintático");
					}
				}
				else { ErrorLib errorLib(contadorLinha, "Instrução fora da section correta", "Léxico"); }
			}
			if (operacao == "const") {
				if (isSectionData) {
					contadorPosicao += infoDeDiretivas.tamanho;
					int constVal;
					if (isOperandoNumero(operando[0])) {
						constVal = converteStringParaInt(operando[0]);
						tabelaLib.insereSimboloNaTabelaDeSimbolos(label,
							InfoDeSimbolo(contadorPosicao, 1, true, constVal));
					}
					else if (isOperandoHexa(operando[0])) {
						constVal = converteStringHexaParaInt(operando[0]);
						tabelaLib.insereSimboloNaTabelaDeSimbolos(label,
							InfoDeSimbolo(contadorPosicao, 1, true, constVal));
					}
				}
				else { ErrorLib errorLib(contadorLinha, "Instrução fora da section correta", "Léxico"); }
			}
		}
		else {
			ErrorLib errorLib(contadorLinha, "Operação não identificada!", "Léxico");
		}
		contadorLinha++;
	}

	//    std::cout << "Fim da primeira passagem!" << std::endl;
	//    showTabelaDeSimbolos();
	segundaPassagem(nomeArquivoSaida);
}

bool PreProcessamento::isOperandoNumero(std::string operando) {
	return !operando.empty() && std::find_if(operando.begin(),
		operando.end(), [](char c) { return !std::isdigit(c); }) == operando.end();
}

int PreProcessamento::converteStringParaInt(std::string operando) {
	return std::stoi(operando);
}

bool PreProcessamento::isOperandoHexa(std::string operando) {
	return operando.compare(0, 2, "0x") == 0
		&& operando.size() > 2
		&& operando.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
}

int PreProcessamento::converteStringHexaParaInt(std::string operando) {
	return std::stoi(operando, nullptr, 16);
}

void PreProcessamento::showTabelaDeSimbolos() {
	TabelaLib tabelaLib;
	std::map<std::string, InfoDeSimbolo> tabelaDeSimbolos = tabelaLib.getTabelaDeSimbolos();
	for (auto &tabelaDeSimbolo : tabelaDeSimbolos) {
		//        std::cout << "Simbolo: " << tabelaDeSimbolo.first << std::endl;
		//        std::cout << "Valor: " << tabelaDeSimbolo.second.endereco << std::endl;
	}

}


void PreProcessamento::segundaPassagem(std::string nomeArquivoSaida) {
	std::vector<Montador::TokensDaLinha> tokensDaLinha = getTokensDaLinhaList();
	TabelaLib tabelaLib;
	int contadorLinha = 1;
	int contadorPosicao = 0;
	bool isSectionText = false;
	bool isSectionData = false;
	std::ofstream arquivoDeSaida(nomeArquivoSaida + ".o");

	for (unsigned int i = 0; i < tokensDaLinha.size(); i++) {
		std::string label = tokensDaLinha[i].label;
		std::string operacao = tokensDaLinha[i].operacao;
		std::vector<std::string> operando = tokensDaLinha[i].operando;
		int numeroDaLinha = tokensDaLinha[i].numeroDaLinha;
		std::string::size_type numeroDeOperandos = operando.size();
		InfoDeInstrucoes infoDeInstrucoes;
		InfoDeDiretivas infoDeDiretivas;

		if (tabelaLib.isInstrucao(operacao)) {
			infoDeInstrucoes = tabelaLib.getInstrucao(operacao);
			for (int j = 0; j < numeroDeOperandos; j++) {
				int valor = 0;
				if (operando[j].find('+') != std::string::npos) {
					std::string copia = operando[j];
					operando[j] = operando[j].substr(0, operando[j].find('+'));
					std::string temp = copia.substr(copia.find('+') + 1, copia.size());
					if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(temp)) {
						valor = tabelaLib.obtemSimboloNaTabelaDeSimbolos(temp).valorConstante;
					}
					else {
						valor = converteStringParaInt(temp);
						//                        std::cout << "Valor: " << valor << std::endl;
					}
				}
				else if (operando[j].find('-') != std::string::npos) {
					std::string copia = operando[j];
					operando[j] = operando[j].substr(0, operando[j].find('-'));
					std::string temp = copia.substr(copia.find('-') + 1, copia.size());
					if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(temp)) {
						valor = tabelaLib.obtemSimboloNaTabelaDeSimbolos(temp).valorConstante;
					}
					else {
						valor = converteStringParaInt(temp);
						//                        std::cout << "Valor: " << valor << std::endl;
					}
				}
				if (!isOperandoNumero(operando[j])) {
					if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j])) {
						arquivoDeSaida << infoDeInstrucoes.opcodesInstrucoes << " ";
						arquivoDeSaida << tabelaLib.obtemSimboloNaTabelaDeSimbolos(operando[j]).endereco + valor << " ";
					}
				}
			}
		}
	}

	arquivoDeSaida.close();

}


