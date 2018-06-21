#include "Montador.h"

Montador::TokensDaLinha::TokensDaLinha(
	const std::string &label,
	const std::string &operacao,
	const std::vector<std::string> &operando,
	int numeroDaLinha) : label(label),
	operacao(operacao),
	operando(operando),
	numeroDaLinha(numeroDaLinha) {}

void Montador::adicionarTokenDaLinha(Montador::TokensDaLinha linha) {
	Montador::listaDeTokensDoArquivo.push_back(linha);
}
