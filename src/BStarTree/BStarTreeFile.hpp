#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <string>

#include "../Common.hpp"
#include "../File.hpp"
#include "../Item.hpp"

namespace Algorithm::BStarTree {
class BStarTreeFile {
public:
    /**
     * @brief Metadados gravados no cabeçalho do arquivo da árvore B* em disco.
     */
    struct Metadata {
        std::filesystem::file_time_type lastModification;
        uint64_t size;
        int64_t rootIndex;
    };

    /**
     * @brief Entrada armazenada nos nós folha (Data) da árvore B*.
     *
     * Contém a chave inteira de busca e o índice da página no arquivo de dados
     * onde o item reside.
     */
    struct Entry {
        int key = 0;
        uint64_t pageIndex = 0;
    };

    /**
     * @brief Estrutura de um nó da árvore B* armazenado em disco.
     *
     * A árvore B* diferencia nós internos (Index), que contêm apenas chaves e
     * apontadores para nós filhos, de nós folhas (Data), que contêm as entradas
     * reais com chave e pageIndex, além de um apontador para a próxima página
     * folha.
     */
    struct Node {
        enum Type : uint8_t {
            Index,
            Data,
        };

        struct Index {
            std::array<int, PAGE_SIZE> indexes;
            std::array<int64_t, PAGE_SIZE + 1> nodePos;
        };

        struct Data {
            std::array<Entry, PAGE_SIZE> entries;
            int64_t nextData;
        };

        Type type;
        union {
            struct Index index;
            struct Data data;
        };
        uint64_t size = 0;

        /**
         * @brief Verifica se o nó é um nó folha (tipo Data).
         *
         * @return true se o tipo for Data; false caso contrário.
         */
        [[nodiscard]] bool isLeaf() const;

        /**
         * @brief Construtor padrão do nó. Inicializa como nó de dados vazio.
         */
        Node();

        /**
         * @brief Construtor com especificação do tipo do nó.
         *
         * @param type Tipo do nó (Index ou Data).
         */
        explicit Node(Type type);
    };

    /**
     * @brief Construtor da classe BStarTreeFile.
     *
     * Obtém o caminho do arquivo de índice através de `GetFilePath` e tenta
     * carregar um arquivo existente via `TryLoadExistingFile`. Se não existir
     * ou for inválido, constrói a árvore chamando `BuildFile`.
     *
     * @param input Referência para o arquivo binário de dados de entrada.
     */
    explicit BStarTreeFile(File& input);

    /**
     * @brief Destrutor da classe BStarTreeFile.
     *
     * Fecha o fluxo de leitura do arquivo em disco (`file_`).
     */
    ~BStarTreeFile();

    /**
     * @brief Realiza a pesquisa de uma chave na árvore B* em disco.
     *
     * Percorre a árvore B* a partir do nó raiz navegando pelos nós de índice
     * até encontrar o nó folha (Data) onde a chave reside ou constatar que ela
     * não existe.
     *
     * @param key Chave numérica a ser procurada.
     * @return std::optional<Node> O nó folha contendo a chave, ou std::nullopt.
     */
    std::optional<Node> Search(int key);

private:
    std::ifstream file_;
    int64_t rootIndex_ = 0;

    /**
     * @brief Gera o caminho do arquivo binário da árvore B* a partir do arquivo
     * de entrada.
     *
     * @param input Arquivo de dados de entrada.
     * @return std::string Caminho completo com sufixo ".bstartree".
     */
    static std::string GetFilePath(const File& input);

    /**
     * @brief Calcula a posição em bytes (offset) de um nó no arquivo da árvore
     * B*.
     *
     * @param nodeIndex Índice numérico do nó.
     * @return std::streamoff Deslocamento em bytes em relação ao início do
     * arquivo.
     */
    static std::streamoff GetNodeOffset(uint64_t nodeIndex);

    /**
     * @brief Tenta carregar e validar um arquivo de árvore B* pré-existente.
     *
     * @param path Caminho do arquivo da árvore B*.
     * @param input Arquivo de entrada original para validação de integridade.
     * @return true se o arquivo existe e é válido; false caso contrário.
     */
    bool TryLoadExistingFile(const std::string& path, const File& input);

    /**
     * @brief Valida os metadados do arquivo da árvore B* comparando com o
     * arquivo original.
     *
     * @param input Arquivo de entrada original.
     * @return true se os metadados forem compatíveis; false caso contrário.
     */
    bool ValidateFile(const File& input);

    /**
     * @brief Constrói o arquivo da árvore B* em disco a partir do arquivo de
     * dados.
     *
     * @param input Arquivo de dados de entrada.
     * @param path Caminho do arquivo a ser criado.
     */
    void BuildFile(File& input, const std::string& path);

    /**
     * @brief Grava um nó na posição especificada do arquivo em disco.
     *
     * @param file Fluxo de arquivo aberto em modo de escrita binária.
     * @param nodeIndex Índice do nó a ser gravado.
     * @param node Estrutura do nó a ser persistida.
     */
    static void WriteNode(std::fstream& file, uint64_t nodeIndex,
                          const Node& node);

    /**
     * @brief Lê um nó do arquivo em disco a partir de seu índice.
     *
     * @param file Fluxo de entrada aberto em modo binário.
     * @param nodeIndex Índice do nó a ser lido.
     * @param node Referência para armazenar o nó lido.
     * @return true se a leitura foi bem-sucedida; false caso contrário.
     */
    static bool ReadNode(std::istream& file, uint64_t nodeIndex, Node& node);

    /**
     * @brief Adiciona um novo nó ao final do arquivo e atualiza o contador de
     * nós.
     *
     * @param file Fluxo aberto em modo de escrita.
     * @param node Nó a ser gravado.
     * @param lastNodeIndex Referência para o índice do último nó gravado.
     * @return uint64_t Novo índice atribuído ao nó.
     */
    static uint64_t AppendNode(std::fstream& file, const Node& node,
                               uint64_t& lastNodeIndex);

    /**
     * @brief Grava a estrutura de metadados no início do arquivo de árvore B*.
     *
     * @param file Fluxo aberto para escrita.
     * @param input Arquivo original de dados.
     * @param rootIndex Índice do nó raiz no momento da gravação.
     */
    static void WriteMetadata(std::fstream& file, const File& input,
                              int64_t rootIndex);

    /**
     * @brief Localiza a posição de busca ou inserção de uma chave dentro de um
     * nó ordenado.
     *
     * @param node Nó da árvore B* (Index ou Data).
     * @param key Chave buscada.
     * @param found Saída indicando se a chave existe exatamente no nó.
     * @return size_t Para nós Data, índice da entrada ou de inserção. Para nós
     *         Index, índice do ponteiro do filho onde a chave deve residir.
     */
    static size_t FindKeyIndex(const Node& node, int key, bool& found);

    /**
     * @brief Insere uma entrada em um nó folha (Data) que não está cheio.
     *
     * @param node Nó de dados com espaço disponível (size < PAGE_SIZE).
     * @param entry Entrada a ser inserida.
     */
    static void InsertIntoNonFullDataNode(Node& node, const Entry& entry);

    /**
     * @brief Insere uma chave e seu ponteiro direito em um nó de índice que
     * não está cheio.
     *
     * @param node Nó de índice com espaço disponível (size < PAGE_SIZE).
     * @param key Chave a ser inserida.
     * @param rightChild Ponteiro para o filho à direita associado à chave.
     */
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    static void InsertIntoNonFullIndexNode(Node& node, int key,
                                           int64_t rightChild);

    /**
     * @brief Realiza a divisão (split) de um nó folha (Data) cheio.
     *
     * @param file Fluxo de arquivo para persistência dos nós.
     * @param node Nó folha atual que atingiu a capacidade máxima.
     * @param nodeIndex Índice do nó atual no arquivo.
     * @param entry Entrada que causou o transbordamento.
     * @param promotedEntry Saída contendo a entrada promovida com a chave
     * divisora.
     * @param createdSiblingIndex Saída contendo o índice do novo nó folha
     * irmão criado.
     * @param lastNodeIndex Referência para o controle do índice do último nó no
     * arquivo.
     */
    static void SplitDataNode(std::fstream& file, Node& node,
                              uint64_t nodeIndex, const Entry& entry,
                              Entry& promotedEntry,
                              int64_t& createdSiblingIndex,
                              uint64_t& lastNodeIndex);

    /**
     * @brief Realiza a divisão (split) de um nó interno (Index) cheio.
     *
     * @param file Fluxo de arquivo para persistência dos nós.
     * @param node Nó de índice atual que atingiu a capacidade máxima.
     * @param nodeIndex Índice do nó atual no arquivo.
     * @param key Chave promovida do filho que causou o transbordamento.
     * @param rightChildIndex Ponteiro direito associado à nova chave.
     * @param promotedEntry Saída contendo a entrada promovida para o pai.
     * @param createdSiblingIndex Saída contendo o índice do novo nó irmão
     * criado.
     * @param lastNodeIndex Referência para o controle do índice do último nó no
     * arquivo.
     */
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    static void SplitIndexNode(std::fstream& file, Node& node,
                               uint64_t nodeIndex, int key,
                               int64_t rightChildIndex, Entry& promotedEntry,
                               int64_t& createdSiblingIndex,
                               uint64_t& lastNodeIndex);

    /**
     * @brief Função auxiliar recursiva para inserção descendente na árvore B*.
     *
     * @param file Fluxo de arquivo da árvore B*.
     * @param currentNodeIndex Índice do nó atual na recursão.
     * @param entryToInsert Entrada a ser inserida.
     * @param promotedEntry Saída para entrada promovida se houver divisão.
     * @param newChildIndex Saída para índice do novo nó criado na divisão.
     * @param lastNodeIndex Referência para o controle de nós no arquivo.
     * @return true se o nó atual foi dividido e requer inserção no pai; false
     * caso contrário.
     */
    static bool InsertInternal(std::fstream& file, int64_t currentNodeIndex,
                               const Entry& entryToInsert, Entry& promotedEntry,
                               int64_t& newChildIndex, uint64_t& lastNodeIndex);

    /**
     * @brief Insere um item individual na árvore B*.
     *
     * Inicia a inserção a partir da raiz e, se a raiz for dividida, cria uma
     * nova raiz (Index), aumentando a altura da árvore.
     *
     * @param file Fluxo do arquivo em disco.
     * @param item Item a ser inserido.
     * @param pageIndex Índice da página do arquivo de dados onde o item se
     * encontra.
     * @param rootIndex Referência para o índice da raiz atual.
     * @param lastNodeIndex Referência para o controle de nós no arquivo.
     */
    static void InsertItem(std::fstream& file, const Item& item,
                           uint64_t pageIndex, int64_t& rootIndex,
                           uint64_t& lastNodeIndex);

    /**
     * @brief Insere os itens de uma página de dados na árvore B*.
     *
     * @param file Fluxo do arquivo em disco.
     * @param page Array contendo os itens da página.
     * @param pageIndex Índice da página de dados.
     * @param itemCount Quantidade de itens válidos na página.
     * @param rootIndex Referência para o índice da raiz.
     * @param lastNodeIndex Referência para o controle de nós no arquivo.
     */
    static void InsertPage(
        std::fstream& file, const std::array<Item, PAGE_SIZE>& page,
        uint64_t pageIndex,  // NOLINT(bugprone-easily-swappable-parameters)
        size_t itemCount, int64_t& rootIndex, uint64_t& lastNodeIndex);

    /**
     * @brief Popula a árvore B* lendo sequencialmente as páginas do arquivo de
     * dados.
     *
     * @param file Fluxo do arquivo da árvore B*.
     * @param input Arquivo de dados de entrada.
     * @param rootIndex Referência para o índice da raiz.
     * @param lastNodeIndex Referência para o controle de nós no arquivo.
     */
    static void PopulateTree(std::fstream& file, File& input,
                             int64_t& rootIndex, uint64_t& lastNodeIndex);
};
}  // namespace Algorithm::BStarTree
