#include "Tokenizador.h"

Tokenizador::TokensDaLinha::TokensDaLinha(
	const std::string &label,
	const std::string &operacao,
	const std::vector<std::string> &operando,
	int numeroDaLinha) : label(label),
	operacao(operacao),
	operando(operando),
	numeroDaLinha(numeroDaLinha) {}

void Tokenizador::adicionarTokenDaLinha(Tokenizador::TokensDaLinha linha) {
	Tokenizador::listaDeTokensDoArquivo.push_back(linha);
}
