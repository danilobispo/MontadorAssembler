#pragma once
#include <string>

class ErrorLib {
private:
	int linhaDoErro;
	std::string mensagemDeErro;
	std::string tipoErro; //Léxico, sintático ou semântico
public:
	ErrorLib(int linhaDoErro, const std::string &mensagemDeErro, const std::string &tipoErro);
};
