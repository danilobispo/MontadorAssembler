#include "TabelaLib.h"
#include <algorithm>
#include <map>
#include <iostream>

//void TabelaLib::montarTabelaDeDiretivas(std::vector tabelaDeSimbolos) {
//
//}

std::map <std::string, InfoDeInstrucoes> TabelaLib::TabelaDeInstrucoes{
	{ "add",{ 1,2, opcodes::ADD } },
{ "sub",{ 1,2, opcodes::SUB } },
{ "mult",{ 1,2, opcodes::MULT } },
{ "div",{ 1,2, opcodes::DIV } },
{ "jmp",{ 1,2, opcodes::JMP } },
{ "jmpn",{ 1,2,opcodes::JMPN } },
{ "jmpp",{ 1,2, opcodes::JMPP } },
{ "jmpz",{ 1,2, opcodes::JMPZ } },
{ "copy",{ 2,3, opcodes::COPY } },
{ "load",{ 1,2, opcodes::LOAD } },
{ "store",{ 1,2,opcodes::STORE } },
{ "input",{ 1,2, opcodes::INPUT } },
{ "output",{ 1,2,opcodes::OUTPUT } },
{ "stop",{ 0,1, opcodes::STOP } }
};

std::map<std::string, InfoDeDiretivas> TabelaLib::TabelaDeDiretivas{
	{ "section",{ 1,1, SECTION, bool(false) } },
{ "space",{ -1,-1, SPACE, bool(false) } },
{ "const" ,{ 1,1, CONST, bool(false) } },
{ "equ",{ 1,0,EQU, bool(true) } },
{ "if",{ 1,0, IF, bool(true) } },
{ "macro",{ -1,0, MACRO, bool(false) } },
{ "endmacro",{ 0,0, ENDMACRO, bool(false) } }
};

std::map<std::string, InfoDeSimbolo> TabelaLib::TabelaDeSimbolos;
std::map<std::string, InfoMacroDef> TabelaLib::MacroDefinitionTable;
std::map<std::string, InfoMacroName> TabelaLib::MacroNameTable;


bool TabelaLib::isDiretiva(std::string operacao) {
	// Obtém os valores da lista de diretiva e verifica pelo valor da key se é uma diretiva
	for (auto i : TabelaLib::TabelaDeDiretivas) {
		if (i.first == operacao) {
			return i.first == operacao;
		};
	}
	return false;
}

InfoDeDiretivas TabelaLib::getDiretiva(std::string operacao) {
	return TabelaLib::TabelaDeDiretivas.at(operacao);
}

InfoDeInstrucoes TabelaLib::getInstrucao(std::string operacao) {
	return TabelaLib::TabelaDeInstrucoes.at(operacao);
}

bool TabelaLib::isInstrucao(std::string operacao) {
	for (auto i : TabelaLib::TabelaDeInstrucoes) {
		if (operacao == i.first) {
			return i.first == operacao;
		}
	}
	return false;
}
InfoDeSimbolo::InfoDeSimbolo(int endereco, int espaco, bool isConstante, int valorConstante) : endereco(endereco),
espaco(espaco),
isConstante(isConstante),
valorConstante(
	valorConstante) {}

void TabelaLib::insereSimboloNaTabelaDeSimbolos(std::string key, InfoDeSimbolo infoDeSimbolo) {
	TabelaLib::TabelaDeSimbolos.insert(std::make_pair(key, infoDeSimbolo));
}

bool TabelaLib::rotuloJaExistenteNaTabelaDeSimbolos(std::string rotulo) {
	return !(TabelaLib::TabelaDeSimbolos.find(rotulo) == TabelaLib::TabelaDeSimbolos.end());
}

const std::map<std::string, InfoDeSimbolo> &TabelaLib::getTabelaDeSimbolos() const {
	return TabelaDeSimbolos;
}

void TabelaLib::setTabelaDeSimbolos(const std::map<std::string, InfoDeSimbolo> &TabelaDeSimbolos) {
	TabelaLib::TabelaDeSimbolos = TabelaDeSimbolos;
}

InfoDeSimbolo TabelaLib::obtemSimboloNaTabelaDeSimbolos(std::string id) {
	return TabelaLib::TabelaDeSimbolos.at(id);
}

void TabelaLib::insereMacroNaTabelaDeDefinicoes(std::string nomeMacro, InfoMacroDef infoMacroDef) {
	TabelaLib::MacroDefinitionTable.insert(std::make_pair(nomeMacro, infoMacroDef));

}

InfoMacroDef TabelaLib::obtemInfoMacroDefNaTabelaDeDefinicoes(std::string id) {
	return TabelaLib::MacroDefinitionTable.at(id);
}

void TabelaLib::insereNomeDaMacroNaTabelaDeNomes(std::string nomeMacro, InfoMacroName infoMacroName) {
	TabelaLib::MacroNameTable.insert(std::make_pair(nomeMacro, infoMacroName));

}

InfoMacroName TabelaLib::obtemInfoMacroNameNaTabelaDeNomes(std::string id) {
	return TabelaLib::MacroNameTable.at(id);
}

bool TabelaLib::macroJaExistenteNaTabelaDeNomes(std::string id) {
	return !(TabelaLib::MacroNameTable.find(id) == TabelaLib::MacroNameTable.end());
}

void TabelaLib::esvaziarTabelaDeSimbolosEDeMacros() {
	TabelaLib::TabelaDeSimbolos.clear();
	TabelaLib::MacroNameTable.clear();
	TabelaLib::MacroDefinitionTable.clear();
}


InfoMacroDef::InfoMacroDef(int linha, const std::vector<Montador::TokensDaLinha> &tokensDaLinha) : linha(linha),
tokensDaLinha(
	tokensDaLinha) {}

InfoMacroName::InfoMacroName(int numeroDeArgumentos, int linha) : numeroDeArgumentos(numeroDeArgumentos),
linha(linha) {}
