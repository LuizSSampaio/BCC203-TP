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
     * Consulta primeiramente a árvore binária em disco através de
     * `file_.Search(key)` para localizar em qual página do arquivo de dados a
     * chave reside:
     * - Se a chave não for encontrada na árvore, encerra a busca retornando
     * vazio.
     * - Se for encontrada, recupera o `pageIndex` contido no nó e carrega essa
     * página específica do arquivo de dados para a memória principal com
     * `input_.GetPageAt(...)`.
     * - Realiza a busca linear nos itens contidos na página em memória.
     *
     * @param key Chave numérica inteira a ser buscada.
     * @return std::optional<Item> Contém o item encontrado se a chave existir;
     *         caso contrário, retorna std::nullopt (vazio).
     */
    std::optional<Item> Search(int key);

private:
    File& input_;
    BTreeFile file_;
};
}  // namespace Algorithm::BinaryTree
