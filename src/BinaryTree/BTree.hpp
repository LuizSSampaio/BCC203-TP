#pragma once

#include "../File.hpp"
#include "../Item.hpp"
#include "BTreeFile.hpp"

namespace Algorithm::BinaryTree {
class BTree {
public:
    /**
     * @brief Construtor da classe BTree.
     *
     * Inicializa a árvore binária de pesquisa em disco a partir do arquivo
     * de dados fornecido. Delega a criação ou o carregamento e validação
     * da estrutura em disco para o membro `file_` (BTreeFile).
     *
     * @param input Referência para o arquivo binário de dados de entrada.
     */
    explicit BTree(File& input);

    /**
     * @brief Destrutor padrão da classe BTree.
     *
     * Libera os recursos associados à árvore binária, delegando o fechamento
     * do arquivo de árvore ao destrutor de `file_`.
     */
    ~BTree() = default;

    /**
     * @brief Realiza a busca de um item pela chave informada.
     *
     * Delega a pesquisa diretamente para o método `Search` do arquivo de árvore
     * binária (`file_`). Percorre os nós na árvore em disco até localizar
     * o item com a chave correspondente ou esgotar os nós.
     *
     * @param key Chave numérica inteira a ser buscada.
     * @return std::optional<Item> Contém o item encontrado se a chave existir;
     *         caso contrário, retorna std::nullopt (vazio).
     */
    std::optional<Item> Search(int key);

private:
    BTreeFile file_;
};
}  // namespace Algorithm::BinaryTree
