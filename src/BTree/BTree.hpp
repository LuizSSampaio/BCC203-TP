#pragma once

#include <array>
#include <optional>

#include "../Common.hpp"
#include "../File.hpp"
#include "../Item.hpp"
#include "BTreeFile.hpp"

namespace Algorithm::BTree {
class BTree {
public:
    /**
     * @brief Construtor da classe BTree.
     *
     * Inicializa a árvore B em disco a partir do arquivo binário de dados
     * fornecido. Delega o carregamento/validação ou construção da árvore em
     * disco para a classe BTreeFile.
     *
     * @param input Referência para o arquivo binário de dados de entrada.
     */
    explicit BTree(File& input);

    /**
     * @brief Destrutor padrão da classe BTree.
     */
    ~BTree() = default;

    /**
     * @brief Realiza a busca de um item pela chave informada na árvore B.
     *
     * Consulta primeiramente o arquivo da árvore B em disco através de
     * `file_.Search(key)` para determinar em qual página do arquivo de dados a
     * chave reside:
     * - Se a chave não for encontrada na árvore B, encerra a busca retornando
     * vazio.
     * - Se for encontrada, recupera o `pageIndex` contido na entrada do nó e
     *   carrega essa página específica com `input_.GetPageAt(...)`.
     * - Realiza a busca na página carregada em memória principal.
     *
     * @param key Chave numérica inteira a ser buscada.
     * @return std::optional<Item> Contém o item encontrado se a chave existir;
     *         caso contrário, retorna std::nullopt.
     */
    std::optional<Item> Search(int key);

private:
    File& input_;
    BTreeFile file_;

    /**
     * @brief Localiza uma entrada com a chave especificada dentro de um nó da
     * árvore B.
     *
     * @param node Nó recuperado do arquivo da árvore B.
     * @param key Chave procurada.
     * @return std::optional<BTreeFile::Entry> A entrada contendo a chave e
     * pageIndex, ou std::nullopt caso não esteja presente no nó.
     */
    static std::optional<BTreeFile::Entry> FindEntryInNode(
        const BTreeFile::Node& node, int key);

    /**
     * @brief Realiza a busca pelo item desejado dentro de uma página carregada
     * em memória.
     *
     * @param page Array contendo os itens da página carregada.
     * @param key Chave numérica procurada.
     * @return std::optional<Item> O item encontrado ou std::nullopt.
     */
    static std::optional<Item> SearchInPage(
        const std::array<Item, PAGE_SIZE>& page, int key);
};
}  // namespace Algorithm::BTree
