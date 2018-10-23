#include "ParseLib.h"
#include <iostream>
#include <regex>
#include <cctype>
#include <sstream>
#include "TabelaLib.h"
#include "ErrorLib.h"
#include "Tokenizador.h"
#include "PreProcessamento.h"


ParseLib::ParseLib()
{
}


ParseLib::~ParseLib()
{
}

// Retorna ou a label ou "" se nao houver
std::string ParseLib::parseLabel(std::string linha) {
	std::string label;
	// Provavelmente a label virá no seguinte formato:
	// LABEL:
	// Então o programa precisa separar a string dos dois pontos e retornar a label original, a ser inserida na
	// tabela de símbolos.
	std::string::size_type posicaoLabel = linha.find(':');
	if (posicaoLabel != std::string::npos) {
		label = linha.substr(0, posicaoLabel);
		// DEBUG
		//        std::cout << "parseLabel label: " << label << std::endl;
		return label;
	}
	else {
		//        std::cout << "Nao existem labels nessa linha!" << std::endl;
		return "";
	}
}

std::string ParseLib::parseOperacao(std::string linha, bool hasLabel) {
	// Uma operacao é uma palavra após uma label(caso a linha tenha label)
	// ou a primeira palavra caso a linha não tenha label
	// Em caso de label, separamos a linha por espaços em brancos (o código garante que, nesse ponto, a label e a
	// operação são separados por pelo menos um espaço) e obtemos a primeira palavra:
	std::vector<std::string> tokensLinhas;
	std::istringstream iss(linha);
	std::copy(std::istream_iterator<std::string>(iss),
		std::istream_iterator<std::string>(),
		std::back_inserter(tokensLinhas));

	if (hasLabel) {
		//std::cout << "Linha Operacao/Diretiva:" << tokensLinhas[1] << std::endl;
		return tokensLinhas[1];
	}
	else {
		//std::cout << "Linha Operacao/Diretiva:" << tokensLinhas[0] << std::endl;
		return tokensLinhas[0];
	}
}

//TODO: Verificar contador de linha
std::vector<std::string> ParseLib::parseOperando(std::string linha, int numeroDeOperandos, bool hasLabel) {
	// Um operando vem logo após a operação, mas se a linha tiver label, a posição do operando pode mudar, logo passamos
	// isso como parâmetro também
	// Obtém os tokens da linha separadamente
	std::vector<std::string> tokensLinhas;
	std::istringstream iss(linha);
	std::copy(std::istream_iterator<std::string>(iss),
		std::istream_iterator<std::string>(),
		std::back_inserter(tokensLinhas));
	// se tiver label, a token[0] é o label e token[1] é a operação:
	std::vector<std::string> operandosString;
	std::vector<int> operandos;
	std::regex operandosHexa("0[xX][0-9a-fA-F]+");
	std::regex operandosHexaNegativo("-0[xX][0-9a-fA-F]+");

	if (hasLabel) {
		if (numeroDeOperandos == 0) { // Caso de algumas diretivas
			// Nada a ser feito
			if (tokensLinhas.size() != 2) {
				// TODO: Colocar número correto da linha
				ErrorLib errorLib = ErrorLib(contadorLinha, "Numero de operandos invalido!", "Sintatico");
			}
		}
		else if (numeroDeOperandos == 1) { // Todos os outros
			if (tokensLinhas.size() != 3) {
				// TODO: Colocar número correto da linha
				ErrorLib errorLib = ErrorLib(contadorLinha, "Numero de operandos invalido!", "Sintatico");
			}
			operandosString.push_back(tokensLinhas[2]);
		}
		else if (numeroDeOperandos == 2) { // Caso do COPY
			if (linha.find(", ") == std::string::npos) {
				ErrorLib errorLib(contadorLinha, "Operandos de COPY não estão separados com uma vírgula com espaço!",
					"Léxico");
			}
			// Os dois estão juntos e separados por uma vírgula, logo é só fazer uma substr para cada operando
			if (tokensLinhas.size() != 4) {
				// TODO: Colocar número correto da linha
				ErrorLib errorLib = ErrorLib(contadorLinha, "Numero de operandos invalido!", "Sintatico");
			}
			std::string op1, op2;
			op1 = tokensLinhas[2].substr(0, tokensLinhas[2].find(','));
			op2 = tokensLinhas[3].substr(0, tokensLinhas[3].size());
			operandosString.push_back(op1);
			operandosString.push_back(op2);
		}
		else if (numeroDeOperandos == -1) { // Caso do SPACE 
			std::string operacao = tokensLinhas[1];
			if (operacao == "space") {
				if (tokensLinhas.size() == 3) {
					operandosString.push_back(tokensLinhas[2]); // Se tiver valor, o valor a frente é o espaço
				}
				else if(tokensLinhas.size() == 2) { 
					operandosString.push_back("1"); // Se não tiver, consideramos que o operando é 1
				}
				else {
					// TODO: Colocar número correto da linha
					ErrorLib errorLib = ErrorLib(contadorLinha, "Numero de operandos invalido!", "Sintatico");
				}
			}
		}
		// Caso sem label, tokens relativos diminuem em uma posição no vetor
	}
	else {
		if (numeroDeOperandos == 0) { // Caso de algumas diretivas
			// Nada a ser feito
		}
		else if (numeroDeOperandos == 1) { // Todos os outros
			operandosString.push_back(tokensLinhas[1]);
			if (tokensLinhas.size() != 2) {
				// TODO: Colocar número correto da linha
				ErrorLib errorLib = ErrorLib(contadorLinha, "Numero de operandos invalido!", "Sintatico");
			}
		}
		else if (numeroDeOperandos == 2) { // Caso do COPY
			if (linha.find(", ") == std::string::npos) {
				ErrorLib errorLib(contadorLinha, "Operandos de COPY não estão separados com uma vírgula com espaço!",
					"Léxico");
			}
			if (tokensLinhas.size() != 3) {
				// TODO: Colocar número correto da linha
				ErrorLib errorLib = ErrorLib(contadorLinha, "Numero de operandos invalido!", "Sintatico");
			}
			//Os dois estão juntos e separados por uma vírgula COM espaço, logo é só fazer uma substr para cada operando
			std::string op1, op2;
			std::string::size_type virgulaPos = tokensLinhas[1].find(',');
			op1 = tokensLinhas[1].substr(0, virgulaPos);
			op2 = tokensLinhas[2].substr(0, tokensLinhas[2].size());
			
			operandosString.push_back(op1);
			operandosString.push_back(op2);
		}
		else if (numeroDeOperandos == -1) { // Caso do SPACE 
			std::string operacao = tokensLinhas[0];
			int tamanho = tokensLinhas.size();
			if (operacao == "space") {
				if (tokensLinhas.size() == 2) {
					operandosString.push_back(tokensLinhas[1]); 
				}
				else if(tokensLinhas.size() == 1) { operandosString.push_back("1"); }
				else {
					ErrorLib errorLib = ErrorLib(contadorLinha, "Numero de operandos invalido!", "Sintatico");
				}
			}
		}
	}

	// Verifica nessa sessão se o operando em questão é um número(decimal ou hexadecimal), ou um label
	for (auto &i : operandosString) {
		if (std::regex_match(i, operandosHexa) ||
			std::regex_match(i, operandosHexaNegativo)) { // Caso de operando ser hexadecimal:
			i = std::to_string(converteOperandoHexaParaInteiro(i));
		}
	}

	return operandosString;
}

std::string ParseLib::removeComentarios(std::string arquivoConteudo) {
	// Comentários são definidos por um ponto e vírgula antes do comentário em si, então
	// separaremos a arquivoConteudo pelo delimitador ;
	// Verificamos antes se a arquivoConteudo possui comentários, e então fazemos a remoção de string
	std::string::size_type posicaoComentario = posicaoComentario = arquivoConteudo.find(';');
	while (posicaoComentario != std::string::npos) {
				
		std::string::size_type posicaoFinal = posicaoComentario;
		while (arquivoConteudo[posicaoFinal] != '\n') {
			posicaoFinal++;
		}

		arquivoConteudo.erase(arquivoConteudo.begin() + posicaoComentario, arquivoConteudo.begin() + posicaoFinal);
		posicaoComentario = arquivoConteudo.find(';');
	}
	return arquivoConteudo;
}

// Faz a separação dos tokens de uma linha, podemos ter 3 opções:
// Linhas com um token(END, STOP)
// Linhas com 3 tokens(COPY command ou LABEL + Operação)
// Linhas com 4 tokens(LABEL+COPY)
Tokenizador::TokensDaLinha ParseLib::parseLinha(std::string linha, int linhaContador) {
	TabelaLib tabelaLib;
	// Transforma em lowercase
	std::transform(linha.begin(), linha.end(), linha.begin(), ::tolower);

	// Separa os elementos da linha, para isso, usamos um parse para obter cada token da linha
	// <Rótulo>: <Operação> <Operando>; <Comentários>
	// <Rótulo>
	// Para obtermos a label, executamos o método parseLabel, que retorna uma string com a label ou uma string vazia
	// quando não temos a label na linha
	std::string label = parseLabel(linha);

	// <Operação>
	// Para obtermos a operação, simplesmente precisamos verificar se uma label foi declarada, caso não tenha sido, o
	// primeiro elemento daquela linha será a operação
	std::string operacao = parseOperacao(linha, !label.empty());
	std::vector<std::string> operandos;

	if (tabelaLib.isDiretiva(operacao)) {
		InfoDeDiretivas infoDeDiretivas = tabelaLib.getDiretiva(operacao);
		// DEBUG Diretiva
		//        std::cout << "infos de diretiva: " << std::endl;
		//        std::cout << "diretiva: " << infoDeDiretivas.diretivasKey << std::endl;
		//        std::cout << "/numero de operandos: " << infoDeDiretivas.numeroDeOperandos << std::endl;
		//        std::cout << "tamanho: " << infoDeDiretivas.tamanho << std::endl ;
		//        std::cout << "isPre: " << infoDeDiretivas.isPre << std::endl << std::endl;

		operandos = parseOperando(linha, infoDeDiretivas.numeroDeOperandos, !label.empty());
		// DEBUG Operando
		for (auto &labelOperando : operandos) {
			//            std::cout << "Operando: " << labelOperando << std::endl;
		}
		if (infoDeDiretivas.tamanho == -1) {
		}
		//  TODO: Realocar essa parte do método em outro lugar
		//        setContadorPosicao(contadorPosicao + infoDeDiretivas.tamanho);
	}
	else if (tabelaLib.isInstrucao(operacao)) {
		InfoDeInstrucoes infoDeInstrucoes = tabelaLib.getInstrucao(operacao);
		// DEBUG Instrução
		//        std::cout << "Infos de instrucao: " << std::endl;
		//        std::cout << "Instrucao: " << infoDeInstrucoes.opcodesInstrucoes << std::endl;
		//        std::cout << "Numero de operandos: " << infoDeInstrucoes.numeroDeOperandos << std::endl;
		//        std::cout << "Tamanho: " << infoDeInstrucoes.tamanho << std::endl << std::endl;
		operandos = parseOperando(linha, infoDeInstrucoes.numeroDeOperandos, !label.empty());
		// DEBUG Operando
		for (auto &labelOperando : operandos) {
			//            std::cout << "Operando: " << labelOperando << std::endl;
		}
		//        setContadorPosicao(contadorPosicao + infoDeInstrucoes.tamanho);
	}
	else {
		ErrorLib errorLib(linhaContador, "Operação inexistente", "Léxico");
	}
	return Tokenizador::TokensDaLinha(label, operacao, operandos, linhaContador);
}


std::string ParseLib::removeEspacosEmBrancoExtras(std::string arquivoConteudo) {
	
	//remove duplos espaços
	std::string::size_type posicaoEspaco = arquivoConteudo.find("  ");
	while (posicaoEspaco != std::string::npos) {
		arquivoConteudo.erase(arquivoConteudo.begin() + posicaoEspaco);
		posicaoEspaco = arquivoConteudo.find("  ");
	}

	//remove duplos pulos de linha
	posicaoEspaco = arquivoConteudo.find("\n\n");
	while (posicaoEspaco != std::string::npos) {
		arquivoConteudo.erase(arquivoConteudo.begin() + posicaoEspaco);
		posicaoEspaco = arquivoConteudo.find("\n\n");
	}

	////remove duplos pulos no final do arquivo
	//posicaoEspaco = arquivoConteudo.find("\n" + EOF);
	//while (posicaoEspaco != std::string::npos) {
	//	arquivoConteudo.erase(arquivoConteudo.begin() + posicaoEspaco);
	//	posicaoEspaco = arquivoConteudo.find("\n" + EOF);
	//}

	//remove duplos pulos no inicio do arquivo
	while (arquivoConteudo[0] == '\n') {
		arquivoConteudo.erase(arquivoConteudo.begin());
	}

	//remove espaços antes de \n
	posicaoEspaco = arquivoConteudo.find(" \n");
	while (posicaoEspaco != std::string::npos) {
		arquivoConteudo.erase(arquivoConteudo.begin() + posicaoEspaco);
		posicaoEspaco = arquivoConteudo.find(" \n");
	}

	//remove espaços depois de \n
	posicaoEspaco = arquivoConteudo.find("\n ");
	while (posicaoEspaco != std::string::npos) {
		arquivoConteudo.erase(arquivoConteudo.begin() + posicaoEspaco + 1);
		posicaoEspaco = arquivoConteudo.find("\n ");
	}

	//remove espaços antes de :
	posicaoEspaco = arquivoConteudo.find(" :");
	while (posicaoEspaco != std::string::npos) {
		arquivoConteudo.erase(arquivoConteudo.begin() + posicaoEspaco);
		posicaoEspaco = arquivoConteudo.find(" :");
	}

	return arquivoConteudo;

	/*std::regex labelComPuloDeLinha("(.+:)(\n)");
	arquivoConteudo = std::regex_replace(arquivoConteudo, labelComPuloDeLinha, "$1 ");
	std::regex labelComLetraLogoAposDoisPontos("(.+:)(\\w)");
	arquivoConteudo = std::regex_replace(arquivoConteudo, labelComLetraLogoAposDoisPontos, "$1 $2");*/

	//std::string output;
	//output.clear();
	//unique_copy(arquivoConteudo.begin(),
	//	arquivoConteudo.end(),
	//	std::back_insert_iterator<std::string>(output),
	//	[](char a, char b) { return isspace(static_cast<unsigned char>(a)) && isspace(static_cast<unsigned char>(b)); });

	

	//return output;
}

std::string ParseLib::removeTabulacoes(std::string fileString) {
	// Remove tabulações transformando-as em espaços em branco, caso os espaços em branco sejam redundantes, 
	// o passo de remover espaços em branco irá remover a redundância.
	fileString = std::regex_replace(fileString, std::regex("\t"), " ");
	// fileString.erase(std::remove(fileString.begin(), fileString.end(), '\t'), fileString.end());
	return fileString;
}

std::string ParseLib::juntaLabelEOperacao(std::string arquivo)
{
	std::string::size_type posicao = arquivo.find(":\n");
	while (posicao != std::string::npos) {
		arquivo[posicao + 1] = ' ';
		posicao = arquivo.find(":\n");
	}
	return arquivo;
}

std::vector<std::string> ParseLib::separaEmLinhas(std::string fileString) {
	std::vector<std::string> strings;
	const std::string delimiter = "\n";

	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while ((pos = fileString.find(delimiter, prev)) != std::string::npos) {
		strings.push_back(fileString.substr(prev, pos - prev));
		prev = pos + 1;
	}

	// To get the last substring (or only, if delimiter is not found)
	strings.push_back(fileString.substr(prev));

	return strings;
}

void ParseLib::preparaCodigo(std::string operacao, const std::string &arquivoEntradaNome, const std::string &arquivoSaidaNome) {
	std::string arquivo = this->getArquivo();
	arquivo = this->removeTabulacoes(arquivo);
	arquivo = this->removeEspacosEmBrancoExtras(arquivo);
	setLinhasDoCodigo(this->separaEmLinhas(arquivo));
	std::vector<std::string> codeLines = getLinhasDoCodigo();
	int contadorLinha = 1;
	setContadorLinha(contadorLinha);
	std::vector<Tokenizador::TokensDaLinha> listTokensDaLinha;
	for (auto &codeLine : codeLines) {
		Tokenizador::TokensDaLinha tokensDaLinha = parseLinha(codeLine, contadorLinha);
		listTokensDaLinha.push_back(tokensDaLinha);
		contadorLinha++;
	}
	//    DEBUG
	//    for (auto &i : listTokensDaLinha) {
	//        std::cout << "Label: " << i.label << std::endl;
	//        std::cout << "Operacao: " << i.operacao << std::endl;
	//        for (auto it = i.operando.begin(); it != i.operando.end(); ++it) {
	//            std::cout << "Operando: " << *it << std::endl;
	//        }
	//        std::cout << "Numero da Linha: " << i.numeroDaLinha << std::endl << std::endl;
	//    }
}

const std::vector<std::string> &ParseLib::getLinhasDoCodigo() const {
	return linhasDoCodigo;
}

void ParseLib::setLinhasDoCodigo(const std::vector<std::string> &linhasDoCodigo) {
	ParseLib::linhasDoCodigo = linhasDoCodigo;
}

std::string ParseLib::removeCaracteresEspeciais(std::string str)
{
	str.erase(std::remove_if(str.begin(), str.end(), isNotAlnum), str.end());
	return str;
}

bool isNotAlnum (char c) {
	// Alguns caracteres extras foram adicionados pois não podemos removê-los ou comprometemos a tokenização
	return std::isalnum(static_cast<unsigned char>(c)) == 0 && (c != ';' && c != '\n' && c != ' ' && c!= ':');
}

const std::string &ParseLib::getArquivo() const {
	return arquivo;
}

void ParseLib::setArquivo(const std::string &arquivo) {
	ParseLib::arquivo = arquivo;
}

void ParseLib::printLinha(std::string linha) {
	    std::cout << "Linha atual: " << linha << std::endl;
}

bool ParseLib::isParsingMacro() const {
	return parsingMacro;
}

void ParseLib::setParsingMacro(bool parsingMacro) {
	ParseLib::parsingMacro = parsingMacro;
}

int ParseLib::converteOperandoHexaParaInteiro(std::string operando) {
	return std::stoi(operando, nullptr, 16);
}

int ParseLib::converteOperandoParaInteiro(std::string operando) {
	return std::stoi(operando);
}

bool ParseLib::isSectionText() const {
	return sectionText;
}

void ParseLib::setSectionText(bool sectionText) {
	ParseLib::sectionText = sectionText;
}

bool ParseLib::isSectionData() const {
	return sectionData;
}

void ParseLib::setSectionData(bool sectionData) {
	ParseLib::sectionData = sectionData;
}

int ParseLib::getContadorPosicao() const {
	return contadorPosicao;
}

void ParseLib::setContadorPosicao(int contadorPosicao) {
	ParseLib::contadorPosicao = contadorPosicao;
}

int ParseLib::getContadorLinha() const {
	return contadorLinha;
}

void ParseLib::setContadorLinha(int contadorLinha) {
	ParseLib::contadorLinha = contadorLinha;
}

bool ParseLib::is_number(const std::string &s) {
	return false;
}

std::vector<Tokenizador::TokensDaLinha> ParseLib::parseTokens(std::string arquivoConteudo) {
	arquivoConteudo = removeComentarios(arquivoConteudo);
	arquivoConteudo = removeTabulacoes(arquivoConteudo);
	arquivoConteudo = removeEspacosEmBrancoExtras(arquivoConteudo);
	arquivoConteudo = juntaLabelEOperacao(arquivoConteudo);
	std::vector<std::string> codeLines = this->separaEmLinhas(arquivoConteudo);
	//for (auto& linha : codeLines) {
		// Remoção dessa área do código pois operandos negativos como no CONST eram parseados incorretamente
		/*else if (linha.find('+') != std::string::npos || linha.find('-') != std::string::npos) {
			std::string::size_type i = 0;
			std::string::size_type posicao = linha.find('+');
			if (posicao == std::string::npos) {
				posicao = linha.find('-');
			}
			i = posicao;
			i++;
			while (linha.at(i) == ' ') {
				i++;
			}
			if (linha.at(posicao) != ' ') {
				linha.erase(std::remove_if(linha.begin() + posicao, linha.end(), isspace), linha.end());
				i = posicao;
				i--;
			}
			while (linha.at(i) == ' ') {
				i--;
			}
			if (linha.at(posicao) != ' ') {
				linha.erase(std::remove_if(linha.begin() + i, linha.end(), isspace), linha.end());
				i = posicao;
				i--;
			}
		}*/
	//}
	int contadorLinha = 1;
	std::vector<Tokenizador::TokensDaLinha> listTokensDaLinha;
	int contadorPosicao = 0;
	setContadorPosicao(contadorPosicao);
	codeLines.erase(remove(codeLines.begin(), codeLines.end(), ""), codeLines.end());
	for (auto &codeLine : codeLines) {
			Tokenizador::TokensDaLinha tokensDaLinha = parseLinha(codeLine, contadorLinha);
			listTokensDaLinha.push_back(tokensDaLinha);
			contadorLinha++;
	}


	//DEBUG
	for (auto &i : listTokensDaLinha) {
		std::cout << "Label: " << i.label << std::endl;
		std::cout << "Operacao: " << i.operacao << std::endl;
		for (auto it = i.operando.begin(); it != i.operando.end(); ++it) {
			std::cout << "Operando: " << *it << std::endl;
		}
		std::cout << "Numero da Linha: " << i.numeroDaLinha << std::endl << std::endl;
	}
	return listTokensDaLinha;
}

