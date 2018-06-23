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
{ "begin",{ 0,0, BEGIN, bool(false) } },
{ "end",{ 0,0, END, bool(false) } },
{ "public",{ 1,0, PUBLIC, bool(false)} },
{ "extern", {1,0, EXTERN, bool(false)} }
};

std::map<std::string, InfoDeSimbolo> TabelaLib::TabelaDeSimbolos;
std::map<std::string, InfoDeDefinicao> TabelaLib::TabelaDeDefinicoes;
std::map<std::string, InfoDeUso> TabelaLib::TabelaDeUso;


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
InfoDeSimbolo::InfoDeSimbolo(int endereco, int espaco, bool isConstante, int valorConstante, bool isExtern) : endereco(endereco),
espaco(espaco),
isConstante(isConstante),
valorConstante(valorConstante),
isExtern(isExtern) {}

InfoDeDefinicao::InfoDeDefinicao(int valor) : valor(valor) {}

void TabelaLib::insereSimboloNaTabelaDeSimbolos(std::string key, InfoDeSimbolo infoDeSimbolo) {
	TabelaLib::TabelaDeSimbolos.insert(std::make_pair(key, infoDeSimbolo));
}

void TabelaLib::modificaSimboloNaTabelaDeSimbolos(std::string key, InfoDeSimbolo infoDeSimbolo) {
	std::map<std::string, InfoDeSimbolo>::iterator it = TabelaLib::TabelaDeSimbolos.find(key);
	if (it != TabelaLib::TabelaDeSimbolos.end()) {
		it->second = infoDeSimbolo;
	}
}

bool TabelaLib::rotuloJaExistenteNaTabelaDeSimbolos(std::string rotulo) {
	return !(TabelaLib::TabelaDeSimbolos.find(rotulo) == TabelaLib::TabelaDeSimbolos.end());
}

void TabelaLib::insereSimboloNaTabelaDeDefinicoes(std::string key, InfoDeDefinicao infoDeDefinicao)
{
	TabelaLib::TabelaDeDefinicoes.insert(std::make_pair(key, infoDeDefinicao));
}

void TabelaLib::montarTabelaDeDefinicoes()
{
	std::map<std::string, InfoDeSimbolo> tabelaCopy = getTabelaDeSimbolos();
	for (auto &tabelaDeSimbolo : tabelaCopy) {
		if (!tabelaDeSimbolo.second.isExtern) { // Se a variável não for EXTERN, ela será adicionada à TD
			insereSimboloNaTabelaDeDefinicoes(tabelaDeSimbolo.first, InfoDeDefinicao(tabelaDeSimbolo.second.endereco));
		}
	}
}

const std::map<std::string, InfoDeDefinicao> &TabelaLib::getTabelaDeDefinicoes() const {
	return TabelaDeDefinicoes;
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

void TabelaLib::esvaziarTabelaDeSimbolos() {
	TabelaLib::TabelaDeSimbolos.clear();
}