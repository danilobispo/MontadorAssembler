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

//TODO: Adicionar if e equ
std::map<std::string, InfoDeDiretivas> TabelaLib::TabelaDeDiretivas{
{ "section",{ 1,0, SECTION, bool(false) } },
{ "space",{ -1,-1, SPACE, bool(false) } },
{ "const" ,{ 1,1, CONST, bool(false) } },
{ "public",{ 1,0, PUBLIC, bool(false)} },
{ "extern", {0,0, EXTERN, bool(false)} },
{ "begin",{ 0,0, BEGIN, bool(false) } },
{ "end",{ 0,0, END, bool(false) } },
{ "if", {1, 0, IF, bool(true) } },
{ "equ", {1, 0, EQU, bool(true) } } 

};

std::map<std::string, InfoDeSimbolo> TabelaLib::TabelaDeSimbolos;
std::map<std::string, InfoDeDefinicao> TabelaLib::TabelaDeDefinicoes;
std::map<std::string, InfoDeUso> TabelaLib::TabelaDeUso;


bool TabelaLib::isDiretiva(std::string operacao) {
	// Obtém os valores da lista de diretiva e verifica pelo valor da key se é uma diretiva
	for (auto i : TabelaLib::TabelaDeDiretivas) {
		if (i.first == operacao) {
			return true;
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

bool TabelaLib::rotuloJaExistenteNaTabelaDeDefinicoes(std::string rotulo) {
	return !(TabelaLib::TabelaDeDefinicoes.find(rotulo) == TabelaLib::TabelaDeDefinicoes.end());
}


InfoDeDefinicao TabelaLib::obtemSimboloNaTabelaDeDefinicoes(std::string id) {
	return TabelaLib::TabelaDeDefinicoes.at(id);
}

void TabelaLib::insereSimboloNaTabelaDeUso(std::string key, InfoDeUso infoDeUso) {
	if (rotuloJaExistenteNaTabelaDeUso(key)) { // Se o símbolo já existe, só adicionamos mais um valor à lista
		if (infoDeUso.valorList.size() == 2) {
			TabelaLib::TabelaDeUso.at(key).valorList.push_back(infoDeUso.valorList[0]);
			TabelaLib::TabelaDeUso.at(key).valorList.push_back(infoDeUso.valorList[1]);
		}
		else {
			TabelaLib::TabelaDeUso.at(key).valorList.push_back(infoDeUso.valorList[0]);
		}
		
	}
	else {
		TabelaLib::TabelaDeUso.insert(std::make_pair(key, infoDeUso));
	}
}

InfoDeUso TabelaLib::obtemSimboloNaTabelaDeUso(std::string id){
	return TabelaLib::TabelaDeUso.at(id);
}

bool TabelaLib::rotuloJaExistenteNaTabelaDeUso(std::string id) {
	return !(TabelaLib::TabelaDeUso.find(id) == TabelaLib::TabelaDeUso.end());
}


const std::map<std::string, InfoDeDefinicao> &TabelaLib::getTabelaDeDefinicoes() const {
	return TabelaDeDefinicoes;
}

const std::map<std::string, InfoDeSimbolo> &TabelaLib::getTabelaDeSimbolos() const {
	return TabelaDeSimbolos;
}

const std::map<std::string, InfoDeUso>& TabelaLib::getTabelaDeUso() const
{
	return TabelaDeUso;
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

void TabelaLib::esvaziarTabelas() {
	TabelaLib::TabelaDeSimbolos.clear();
	TabelaLib::TabelaDeDefinicoes.clear();
	TabelaLib::TabelaDeUso.clear();

}

InfoDeUso::InfoDeUso(int valor)
{
	valorList.push_back(valor);
}
