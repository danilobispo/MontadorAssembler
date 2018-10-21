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

PreProcessamento::PreProcessamento(std::vector<Montador::TokensDaLinha> tokensDaLinhaList) : tokensDaLinhaList(
	tokensDaLinhaList) {

	bool entrouEmText = false;
	for (auto &tokensDaLinha : tokensDaLinhaList) {
		if (tokensDaLinha.operacao == "section" && tokensDaLinha.operando[0] == "text") {
			entrouEmText = true;
		}
		if (tokensDaLinha.operacao == "equ") {
			if (!entrouEmText) {
				TabelaLib tabelaLib;
				
				if (!tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(tokensDaLinha.label)) {
					tabelaLib.insereSimboloNaTabelaDeSimbolos(tokensDaLinha.label, 
						InfoDeSimbolo(-1, -1, true, converteStringParaInt(tokensDaLinha.operando[0]), false));
				}
				else {
					ErrorLib erro(0, "Simbolo \""+ tokensDaLinha.label +"\" ja definido", "Sem�ntico"); //TODO colocar a linha correta
				}
			}
			else {
				ErrorLib erro(0, "A diretiva EQU deve ser definida antes de SECTION TEXT", "Sem�ntico");//TODO colocar a linha correta
			}
		}
	}



	for (int i = 0; i < tokensDaLinhaList.size(); i++) {
		if (tokensDaLinhaList[i].operacao == "if") {
			TabelaLib tabelaLib;
			bool ehSimbolo = tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(tokensDaLinhaList[i].operando[0]);
			bool apagaProximaLinha = false;
			if (ehSimbolo) {
				//InfoDeSimbolo(-1, -1, true, converteStringParaInt(tokensDaLinha.operando[0]), false);
				InfoDeSimbolo infoDeSimbolo = tabelaLib.obtemSimboloNaTabelaDeSimbolos(tokensDaLinhaList[i].operando[0]);
				if (infoDeSimbolo.valorConstante != 0) {
					apagaProximaLinha = false;
				}
				else {
					apagaProximaLinha = true;
				}
			}
			else if (isOperandoNumero(tokensDaLinhaList[i].operando[0])) {
				std::string::size_type tamanhoTokenList = tokensDaLinhaList.size();
				if (i == tamanhoTokenList) { //Nada a ser feito, tamanho da lista � igual ao indice(ultima linha)
					apagaProximaLinha = false;
				}
				else {
					apagaProximaLinha = true;
				}				
			}
			if (apagaProximaLinha) {
				tokensDaLinhaList.erase(tokensDaLinhaList.begin() + i);
			}
		}
	}
}

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
* Aqui realizamos a montagem do c�digo, que vai sofrer uma an�lise l�xica
*/
void PreProcessamento::montarCodigo(std::string nomeArquivoSaida) {
	std::vector<Montador::TokensDaLinha> tokensDaLinha = getTokensDaLinhaList();
	primeiraPassagem(tokensDaLinha, 2);
}

void PreProcessamento::primeiraPassagem(std::vector<Montador::TokensDaLinha> tokensDaLinhaList, int numeroDeArquivos) {
	TabelaLib tabelaLib;
	int contadorLinha = 1;
	int contadorPosicao = 0;
	bool isSectionText = false;
	bool isSectionData = false;
	bool checaBeginEEnd = numeroDeArquivos == 2 ? true : false;
	bool temBegin = false;
	bool temEnd = false;


	for (unsigned int i = 0; i < tokensDaLinhaList.size(); i++) {
		std::string label = tokensDaLinhaList[i].label;
		std::string operacao = tokensDaLinhaList[i].operacao;
		std::vector<std::string> operando = tokensDaLinhaList[i].operando;
		int numeroDaLinha = tokensDaLinhaList[i].numeroDaLinha;
		std::string::size_type numeroDeOperandos = operando.size();
		InfoDeInstrucoes infoDeInstrucoes;
		InfoDeDiretivas infoDeDiretivas;

		// Procura se label existe, caso exista, devemos procurar pelo label na tabela de simbolos
		// R�tulo:
		if (!label.empty()) {
			if (!tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(label)) {
				tabelaLib.insereSimboloNaTabelaDeSimbolos(label, InfoDeSimbolo(contadorPosicao, -1, false, -1, false));
			}
			else if (tabelaLib.obtemSimboloNaTabelaDeSimbolos(label).endereco == -2) {
				InfoDeSimbolo infoSimbolo = tabelaLib.obtemSimboloNaTabelaDeSimbolos(label);
				infoSimbolo.endereco = contadorPosicao;
				tabelaLib.modificaSimboloNaTabelaDeSimbolos(label, infoSimbolo);
			}
			else {
				ErrorLib errorLib(contadorLinha, "Redeclara��o de r�tulo " + label, "L�xico");
			}
		}

		// Opera��o
		// Se for instru��o
		if (tabelaLib.isInstrucao(operacao)) {
			if (isSectionData) {
				ErrorLib errorLib(contadorLinha, "Instrucao em local incorreto!", "L�xico");
			}
			infoDeInstrucoes = tabelaLib.getInstrucao(operacao);
			// Checa n�mero de operandos:
			if (numeroDeOperandos != infoDeInstrucoes.numeroDeOperandos) {
				ErrorLib errorLib(contadorLinha, "N�mero de operandos incorreto!", "Sint�tico");
			}
			contadorPosicao += infoDeInstrucoes.tamanho;
		}
		else if (tabelaLib.isDiretiva(operacao)) {
			infoDeDiretivas = tabelaLib.getDiretiva(operacao);
			// As diretivas a serem processadas s�o: SPACE CONST, SECTION, PUBLIC, EXTERN, BEGIN E END
			if (operacao == "section") {
				contadorPosicao += infoDeDiretivas.tamanho;
				if (operando[0] == "text") {
					isSectionText = true;
				}
				else if (operando[0] == "data") {
					if (!isSectionText) {
						ErrorLib errorLib(contadorLinha, "Section data declarada antes da se��o text!", "L�xico");
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
							label, InfoDeSimbolo(contadorPosicao, numeroOperando, false, 0, false));
						contadorPosicao += numeroOperando;
					}
					else {
						ErrorLib errorLib(contadorLinha, "Operando incorreto para instru��o SPACE", "Sint�tico");
					}
				}
				else { ErrorLib errorLib(contadorLinha, "Instru��o fora da section correta", "L�xico"); }
			}
			if (operacao == "const") {
				if (isSectionData) {
					contadorPosicao += infoDeDiretivas.tamanho;
					int constVal;
					if (isOperandoNumero(operando[0])) {
						constVal = converteStringParaInt(operando[0]);
						tabelaLib.insereSimboloNaTabelaDeSimbolos(label,
							InfoDeSimbolo(contadorPosicao, 1, true, constVal, false));
					}
					else if (isOperandoHexa(operando[0])) {
						constVal = converteStringHexaParaInt(operando[0]);
						tabelaLib.insereSimboloNaTabelaDeSimbolos(label,
							InfoDeSimbolo(contadorPosicao, 1, true, constVal, false));
					}
				}
				else { ErrorLib errorLib(contadorLinha, "Instru��o fora da section correta", "L�xico"); }
			}
			else if (operacao == "begin") {
				if (!checaBeginEEnd) {
					ErrorLib errorLib(contadorLinha, "Diretiva begin ou end presente em montagem de apenas um arquivo", "Restri��o");
				}
				else {
					temBegin = true;
				}
			}
			else if (operacao == "end") {
				if (!checaBeginEEnd) {
					ErrorLib errorLib(contadorLinha, "Diretiva begin ou end presente em montagem de apenas um arquivo", "Restri��o");
				}
				else {
					temEnd = true;
				}
			}
			else if (operacao == "public") {
				// Obt�m a label, adiciona na tabela de s�mbolos com o valor
				// Na primeira passagem:
				// Quando a diretiva PUBLIC � encontrada, insere o respectivo operando na Tabela de
				// Defini��es, sem incluir qualquer atributo al�m do nome;
				// O que faremos no c�digo � colocar valores reconhec�veis para InfoDeSimbolo quando formos definir endere�o e outras propriedades
				// desse label, de forma que ele seja reconhecido como um valor inv�lido a princ�pio, e quando passar pela diretiva
				// space os valores ser�o resolvidos corretamente.
				// No caso do r�tulo j� ter sido definido, apenas 
				if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[0])) {
					InfoDeSimbolo infoCopy = tabelaLib.obtemSimboloNaTabelaDeSimbolos(operando[0]);
					infoCopy.isExtern = false;
					tabelaLib.modificaSimboloNaTabelaDeSimbolos(operando[0], infoCopy);
				}
				else {
					tabelaLib.insereSimboloNaTabelaDeSimbolos(operando[0], InfoDeSimbolo(-2, -2, false, 0, false));
				}
			}
			else if (operacao == "extern") {
				// Quando a diretiva EXTERN � encontrada, insere o respectivo r�tulo na TS com
				// valor �zero absoluto� e a indicacao de s�mbolo externo;
				if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(label)) {
					InfoDeSimbolo infoCopy = tabelaLib.obtemSimboloNaTabelaDeSimbolos(label);
					infoCopy.isExtern = true;
					tabelaLib.modificaSimboloNaTabelaDeSimbolos(label, infoCopy);
				}
				tabelaLib.insereSimboloNaTabelaDeSimbolos(label, InfoDeSimbolo(-2, -2, false, 0, true));
			}
		}
		else {
			ErrorLib errorLib(contadorLinha, "Opera��o n�o identificada!", "L�xico");
		}
		contadorLinha++;
	}
	tabelaLib.montarTabelaDeDefinicoes();
	//    std::cout << "Fim da primeira passagem!" << std::endl;

	showTabelaDeSimbolos();
	showTabelaDeDefinicoes();
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
	std::string isExterno;
	std::cout << "Tabela de S�mbolos: " << std::endl;
	for (auto &tabelaDeSimbolo : tabelaDeSimbolos) {
		isExterno = tabelaDeSimbolo.second.isExtern ? "true" : "false";
		std::cout << "Simbolo: " << tabelaDeSimbolo.first << std::endl;
		std::cout << "Valor: " << tabelaDeSimbolo.second.endereco << std::endl;
		std::cout << "Externo? : " << isExterno << std::endl;
	}
}

void PreProcessamento::showTabelaDeDefinicoes() {
	TabelaLib tabelaLib;
	std::map<std::string, InfoDeDefinicao> tabelaDeDefinicoes = tabelaLib.getTabelaDeDefinicoes();
	std::string isExterno;
	std::cout << "Tabela de Defini��es: " << std::endl;
	for (auto &tabelaDeDefinicao : tabelaDeDefinicoes) {
		std::cout << "Simbolo: " << tabelaDeDefinicao.first << std::endl;
		std::cout << "Valor: " << tabelaDeDefinicao.second.valor << std::endl;
	}
}

void PreProcessamento::escreveTabelaDeDefinicoesNoArquivoDeSaida(std::string nomeDoArquivo)
{
	std::ofstream arquivoDeSaida;
	arquivoDeSaida.open(nomeDoArquivo, std::ios::app);

	TabelaLib tabelaLib;
	std::map<std::string, InfoDeDefinicao> tabelaDeDefinicoes = tabelaLib.getTabelaDeDefinicoes();
	for (auto &itemDeUso : tabelaDeDefinicoes) {
		arquivoDeSaida << "D,";
		arquivoDeSaida << itemDeUso.first << ",";
		arquivoDeSaida << itemDeUso.second.valor << "\n";
	}
	arquivoDeSaida.close();
}

void PreProcessamento::escreveTabelaDeUsoNoArquivoDeSaida(std::string nomeDoArquivo)
{
	std::ofstream arquivoDeSaida;
	arquivoDeSaida.open(nomeDoArquivo, std::ios::app);

	TabelaLib tabelaLib;
	std::map<std::string, InfoDeUso> tabelaDeUso = tabelaLib.getTabelaDeUso();
	for (auto &itemDeUso : tabelaDeUso) {
		for (unsigned int i = 0; i < itemDeUso.second.valorList.size(); i++) {
			arquivoDeSaida << "U,";
			arquivoDeSaida << itemDeUso.first << ",";
			arquivoDeSaida << itemDeUso.second.valorList[i] << "\n";
		}
	}
	arquivoDeSaida.close();
}

void PreProcessamento::escreveMapaDeBitsNoArquivoDeSaida(std::string nomeDoArquivo, std::vector<int> mapaDeBits)
{
	std::ofstream arquivoDeSaida;
	arquivoDeSaida.open(nomeDoArquivo, std::ios::app);
	arquivoDeSaida << "R:";
	for (auto &bit : mapaDeBits) {
		arquivoDeSaida << bit;
	}
	arquivoDeSaida.close();
}

void PreProcessamento::escreveTamanhoDoCodigoNoArquivoDeSaida(std::string nomeDoArquivo, int tamanho)
{
	std::ofstream arquivoDeSaida;
	arquivoDeSaida.open(nomeDoArquivo, std::ios::app);
	arquivoDeSaida << "S:" << tamanho << "\n";
	arquivoDeSaida.close();
}

void PreProcessamento::showTabelaDeUso()
{
	TabelaLib tabelaLib;
	std::map<std::string, InfoDeUso> tabelaDeUso = tabelaLib.getTabelaDeUso();
	std::string isExterno;
	std::cout << "Tabela de Uso: " << std::endl;
	for (auto &usoItem : tabelaDeUso) {
		std::cout << "Simbolo: " << usoItem.first << std::endl;
		for (unsigned int i = 0; i < usoItem.second.valorList.size(); i++) {
			std::cout << "Valor: " << usoItem.second.valorList[i] << std::endl;
		}

	}
}

void PreProcessamento::showMapaDeBits(std::vector<int> vetor)
{
	for (auto&item : vetor) {
		std::cout << item;
	}
	std::cout << std::endl;
}


void PreProcessamento::segundaPassagem(std::string nomeDoArquivo) {
	std::vector<Montador::TokensDaLinha> tokensDaLinha = getTokensDaLinhaList();
	TabelaLib tabelaLib;
	int contadorLinha = 1;
	int contadorPosicao = 0;
	bool isSectionText = false;
	bool isSectionData = false;
	int tamanhoCodigo = 0;
	int enderecoOperando;
	std::vector<int> mapaDeBits;
	std::ofstream arquivoDeSaida(nomeDoArquivo + ".o");
	arquivoDeSaida << nomeDoArquivo << ":" << "\n";
	arquivoDeSaida << "C:";

	for (unsigned int i = 0; i < tokensDaLinha.size(); i++) {
		std::string label = tokensDaLinha[i].label;
		std::string operacao = tokensDaLinha[i].operacao;
		std::vector<std::string> operando = tokensDaLinha[i].operando;
		int numeroDaLinha = tokensDaLinha[i].numeroDaLinha;
		std::string::size_type numeroDeOperandos = operando.size();
		InfoDeInstrucoes infoDeInstrucoes;
		bool tem2Operandos = false;

		if (tabelaLib.isInstrucao(operacao)) {
			infoDeInstrucoes = tabelaLib.getInstrucao(operacao);
			tamanhoCodigo += infoDeInstrucoes.tamanho;
			mapaDeBits.push_back(0); // Instru��o n�o precisa ser relocada, logo recebe o bit 0


			for (unsigned int j = 0; j < numeroDeOperandos; j++) {
				int valor = 0;
				if (infoDeInstrucoes.opcodesInstrucoes != 14) { // STOP
					enderecoOperando = tamanhoCodigo - (j + 1);
				}

				// Tratamento de somas e subtra��es no operando
				if (operando[j].find('+') != std::string::npos) {
					tem2Operandos = true;
					std::string copia = operando[j];
					std::string operando1 = copia.substr(0, operando[j].find('+'));
					copia = operando[j];
					std::string operando2 = copia.substr(copia.find('+') + 1, copia.size());
					if (isOperandoNumero(operando1)) {
						valor = converteStringParaInt(operando1);
						// Logo, o segundo operando DEVE ser um label
						operando[j] = operando2;
						// Caso de vari�vel PUBLIC
						if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j]) && tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							// Primeiro operando e � vari�vel local, com endere�o j� definido
							valor += tabelaLib.obtemSimboloNaTabelaDeSimbolos(operando[j]).valorConstante;
						}
						// Caso de vari�vel EXTERN
						else if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j]) && !tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							tabelaLib.insereSimboloNaTabelaDeUso(operando[j], InfoDeUso(enderecoOperando));
						}
					}
					else if (isOperandoNumero(operando2)) {
						valor = converteStringParaInt(operando2);
						// Logo, o primeiro operando DEVE ser um label
						operando[j] = operando1;
						// Caso de vari�vel PUBLIC
						if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j]) && tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							// Primeiro operando e � vari�vel local, com endere�o j� definido
							valor += tabelaLib.obtemSimboloNaTabelaDeSimbolos(operando[j]).valorConstante;
						}
						// Caso de vari�vel EXTERN
						else if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j]) && !tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							tabelaLib.insereSimboloNaTabelaDeUso(operando[j], InfoDeUso(enderecoOperando));

						}
					}
					// Caso de erro: Ou os dois operandos s�o n�meros ou nenhum deles � label
					else if ((isOperandoNumero(operando1) && isOperandoNumero(operando2)) ||
						(!isOperandoNumero(operando1) && !isOperandoNumero(operando2))) {
						ErrorLib errorlib = ErrorLib(numeroDaLinha, "Operandos inv�lidos", "Lexico");
					}
				}
				else if (operando[j].find('-') != std::string::npos) {
					tem2Operandos = true;
					std::string copia = operando[j];
					std::string operando1 = copia.substr(0, operando[j].find('+'));
					copia = operando[j];
					std::string operando2 = copia.substr(copia.find('+') + 1, copia.size());
					if (isOperandoNumero(operando1)) {
						valor = converteStringParaInt(operando1);
						// Logo, o segundo operando DEVE ser um label
						operando[j] = operando2;
						// Caso de vari�vel PUBLIC
						if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j]) && tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							// Primeiro operando e � vari�vel local, com endere�o j� definido
							valor -= tabelaLib.obtemSimboloNaTabelaDeSimbolos(operando[j]).valorConstante;
						}
						// Caso de vari�vel EXTERN
						else if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j]) && !tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							tabelaLib.insereSimboloNaTabelaDeUso(operando[j], InfoDeUso(enderecoOperando));

						}
					}
					else if (isOperandoNumero(operando2)) {
						valor = converteStringParaInt(operando2);
						// Logo, o primeiro operando DEVE ser um label
						operando[j] = operando1;
						// Caso de vari�vel PUBLIC
						if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j]) && tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							// Primeiro operando e � vari�vel local, com endere�o j� definido
							valor -= tabelaLib.obtemSimboloNaTabelaDeSimbolos(operando[j]).valorConstante;
						}
						// Caso de vari�vel EXTERN
						else if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j]) && !tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							tabelaLib.insereSimboloNaTabelaDeUso(operando[j], InfoDeUso(enderecoOperando));

						}
						// Caso de erro: Ou os dois operandos s�o n�meros ou nenhum deles � label
						else if ((isOperandoNumero(operando1) && isOperandoNumero(operando2)) ||
							(!isOperandoNumero(operando1) && !isOperandoNumero(operando2))) {
							ErrorLib errorlib = ErrorLib(numeroDaLinha, "Operandos inv�lidos", "Lexico");
						}
					}
				}
				else {
					tem2Operandos = false;
					if (!tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) { // Primeiro operando como vari�vel extern sem adi��o ou subtra��o no operando
						tabelaLib.insereSimboloNaTabelaDeUso(operando[j], InfoDeUso(enderecoOperando));
					}
				}
				// Escrita do c�digo-fonte 
				if (numeroDeOperandos > 0) {
					if (j != 1) {
						arquivoDeSaida << infoDeInstrucoes.opcodesInstrucoes << " ";
					}
					if (!isOperandoNumero(operando[j])) {
						if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando[j]) && tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							arquivoDeSaida << tabelaLib.obtemSimboloNaTabelaDeSimbolos(operando[j]).endereco + valor << " ";
							mapaDeBits.push_back(0);
						}
						else if (!tabelaLib.rotuloJaExistenteNaTabelaDeDefinicoes(operando[j])) {
							if (tem2Operandos) {
								arquivoDeSaida << valor << " ";
							}
							else {
								arquivoDeSaida << 0 << " ";
							}

							mapaDeBits.push_back(1);
						}
					}
					else {
						valor = converteStringParaInt(operando[j]);
						arquivoDeSaida << tabelaLib.obtemSimboloNaTabelaDeSimbolos(operando[j]).endereco + valor << " ";
						mapaDeBits.push_back(0);
					}
				}
			}
			if (numeroDeOperandos == 0) { // Caso do STOP
				arquivoDeSaida << infoDeInstrucoes.opcodesInstrucoes << " ";
			}
		}
		else if (tabelaLib.isDiretiva(operacao) &&
			tabelaLib.getDiretiva(operacao).diretivasKey == 2) { // CONST
			tamanhoCodigo++;
			arquivoDeSaida << operando[0] << " ";
			mapaDeBits.push_back(0);
		}
		else if (tabelaLib.isDiretiva(operacao) &&
			tabelaLib.getDiretiva(operacao).diretivasKey == 1) { // SPACE
			if (operando.size() != 0) {
				int numero = converteStringParaInt(operando[0]);
				tamanhoCodigo += numero;
				for (int i = 0; i < numero; i++) {
					arquivoDeSaida << "00" << " ";
					mapaDeBits.push_back(0);
				}
			}
			else {
				arquivoDeSaida << "00" << " ";
				mapaDeBits.push_back(0);
				tamanhoCodigo++;
			}
		}
	}
	showTabelaDeUso();
	showMapaDeBits(mapaDeBits);
	arquivoDeSaida << "\n"; // Escreve um pulo de linha ao final do arquivo 
	arquivoDeSaida.close();	// para indicarmos o in�cio para a Tabela de Defini��es
	escreveTabelaDeDefinicoesNoArquivoDeSaida(nomeDoArquivo + ".o");
	escreveTabelaDeUsoNoArquivoDeSaida(nomeDoArquivo + ".o");
	escreveTamanhoDoCodigoNoArquivoDeSaida((nomeDoArquivo + ".o"), tamanhoCodigo);
	escreveMapaDeBitsNoArquivoDeSaida((nomeDoArquivo + ".o"), mapaDeBits);
}

//void PreProcessamento::processarDiretivas(int numeroDeArquivos) {
//	std::vector<Montador::TokensDaLinha> tokensDaLinha = getTokensDaLinhaList();
//	// C�pia das linhas tokenizadas do c�digo, como se o if for falso a linha seguinte n�o ser� processada,
//	// ela ser� removida da sa�da
//	std::vector<Montador::TokensDaLinha> tokensDaLinhaSaida = getTokensDaLinhaList();
//
//	bool isSectionText = false;
//	bool isSectionData = false;
//	bool checaBeginEEnd;
//	if (numeroDeArquivos == 2) {
//		checaBeginEEnd = true;
//	} else {
//		checaBeginEEnd = false;
//	}
//
//
//	int contadorPosicao = 0;
//	ParseLib parseLib();
//	TabelaLib tabelaLib;
//
//	int fatorDeCorrecao = 0;
//
//	for (unsigned int i = 0; i < tokensDaLinha.size(); i++) {
//		Montador::TokensDaLinha copiaLinha = tokensDaLinha[i];
//		// TODO: Devemos avaliar 2 novas diretivas nessa etapa do trabalho: BEGIN e END
//		if (tokensDaLinha[i].operacao == "begin") {
//			// Se temos um begin, o label indica o nome do m�dulo, logo iremos adiciona-lo na tabela de s�mbolos
//			tabelaLib.insereSimboloNaTabelaDeSimbolos(tokensDaLinha[i].label, InfoDeSimbolo(contadorPosicao, -1, false, -1, false));
//		}
//
//
//
//
//		if (tokensDaLinha[i].operacao == "section") {
//			// Checa se o operando � .text ou .data
//			if (tokensDaLinha[i].operando[0] == "text") {
//				// Ativa o booleano que impede que diretivas como EQU estejam no text
//				isSectionText = true;
//			}
//			else if (tokensDaLinha[i].operando[0] == "data") {
//				if (!isSectionText) {
//					//Erro: se��o data veio antes da se��o teste
//					ErrorLib errorLib(tokensDaLinha[i].numeroDaLinha, "Se��o DATA veio antes da se��o TEXT",
//						"Sint�tico");
//				}
//				else {
//					isSectionText = false;
//					isSectionData = true;
//				}
//			}
//		}
//
//		if (tokensDaLinha[i].operacao == "equ" && (!isSectionText && !isSectionData)) {
//			// Adicionar a label � Tabela de S�mbolos
//
//			InfoDeDiretivas infoDeDiretivas = tabelaLib.getDiretiva(tokensDaLinha[i].operacao);
//			int valor = parseLib.converteOperandoParaInteiro(tokensDaLinha[i].operando[0]);
//			if (!tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(tokensDaLinha[i].operacao)) {
//				tabelaLib.insereSimboloNaTabelaDeSimbolos(tokensDaLinha[i].label,
//					InfoDeSimbolo(contadorPosicao, -1, true, valor, false));
//				// Ap�s a inser��o, removemos essa linha, pois ela n�o entrar� no arquivo de sa�da:
//				tokensDaLinhaSaida.erase(tokensDaLinhaSaida.begin() + (i - fatorDeCorrecao));
//				fatorDeCorrecao++;
//
//			}
//			else {
//				ErrorLib elib(tokensDaLinha[i].numeroDaLinha, "Redeclara��o de flag para EQU", "L�xico");
//			}
//
//		}
//		else if (tokensDaLinha[i].operacao == "equ" && (isSectionText || isSectionData)) {
//			ErrorLib errorLib(tokensDaLinha[i].numeroDaLinha, "Operando equ dentro da section text ou data!",
//				"Sint�tico");
//		}
//
//		if (tokensDaLinha[i].operacao == "if" && isSectionText) {
//			tokensDaLinhaSaida.erase(tokensDaLinhaSaida.begin() + (i - fatorDeCorrecao));
//			//            std::cout << "Linha: " << tokensDaLinha[i].numeroDaLinha << std::endl;
//			// Procura na tabela de s�mbolos se o s�mbolo j� foi definido anteriormente
//			if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(tokensDaLinha[i].operando[0])) {
//				InfoDeSimbolo infoSimbolo = tabelaLib.obtemSimboloNaTabelaDeSimbolos(tokensDaLinha[i].operando[0]);
//				if (infoSimbolo.valorConstante != 1) {
//					// Se a compara��o for diferente de 1, a pr�xima linha n�o ser� executada, logo podemos remov�-la
//					// dos tokens de linha do resultado
//					tokensDaLinhaSaida.erase(tokensDaLinhaSaida.begin() + (i - fatorDeCorrecao));
//					fatorDeCorrecao += 2;
//				}
//				else {
//					fatorDeCorrecao++;
//				}
//			}
//		}
//	}
//	// Recalculamos o n�mero de linhas:
//	for (unsigned int it = 0; it < tokensDaLinhaSaida.size(); it++) {
//		tokensDaLinhaSaida[it].numeroDaLinha = it + 1;
//		// No mesmo loop, aproveitamos para fazer a substitui��o de valores caso as labels de diretivas estejam sendo
//		// usadas como operandos
//		for (unsigned int j = 0; j < tokensDaLinhaSaida[it].operando.size(); j++) {
//			std::string operando = tokensDaLinhaSaida[it].operando[j];
//			if (tabelaLib.rotuloJaExistenteNaTabelaDeSimbolos(operando)) {
//				tokensDaLinha[it].operando[j] = std::to_string(
//					tabelaLib.obtemSimboloNaTabelaDeSimbolos(operando).valorConstante);
//			}
//		}
//	}
//	setTokensDaLinhaList(tokensDaLinhaSaida);
//	gerarCodigoDeSaidaDiretivas(nomeArquivoSaida);
//}


