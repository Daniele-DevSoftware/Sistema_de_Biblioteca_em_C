#include "BancoDeDados.h"
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

#define CAMINHO_BANCO "biblioteca.db"

static sqlite3 *conexao = NULL;
static int fechamentoRegistrado = 0;

static int arquivoExiste(const char *caminho) {
    FILE *arquivo = fopen(caminho, "rb");

    if (arquivo == NULL) {
        return 0;
    }

    fclose(arquivo);
    return 1;
}

static int executarSql(const char *sql) {
    char *mensagemErro = NULL;
    int resultado = sqlite3_exec(conexao, sql, NULL, NULL, &mensagemErro);

    if (resultado != SQLITE_OK) {
        fprintf(stderr,
                "Erro no banco de dados: %s\n",
                mensagemErro != NULL ? mensagemErro : sqlite3_errmsg(conexao));

        sqlite3_free(mensagemErro);
        return 0;
    }

    return 1;
}

static int criarTabelas(void) {
    /* O SQL legado tambem tem DROP e consultas de teste; aqui ficam so os CREATE seguros. */
    const char *sql =
        "PRAGMA foreign_keys = ON;"

        "CREATE TABLE IF NOT EXISTS livros ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "titulo TEXT NOT NULL,"
        "autor TEXT NOT NULL,"
        "genero TEXT NOT NULL,"
        "ano INTEGER,"
        "paginas INTEGER,"
        "ja_lido INTEGER,"
        "UNIQUE(titulo, autor)"
        ");"

        "CREATE TABLE IF NOT EXISTS usuarios ("
        "id_usuario INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nome TEXT NOT NULL UNIQUE,"
        "livros_lidos INTEGER DEFAULT 0"
        ");"

        "CREATE TABLE IF NOT EXISTS categorias ("
        "id_categoria INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nome TEXT NOT NULL UNIQUE"
        ");"

        "CREATE TABLE IF NOT EXISTS leituras ("
        "id_leitura INTEGER PRIMARY KEY AUTOINCREMENT,"
        "id_usuario INTEGER,"
        "id_livro INTEGER,"
        "data_leitura DATE DEFAULT CURRENT_DATE,"
        "FOREIGN KEY(id_usuario) REFERENCES usuarios(id_usuario),"
        "FOREIGN KEY(id_livro) REFERENCES livros(id)"
        ");";

    return executarSql(sql);
}

static int carregarDadosIniciais(void) {
    const char *sql =
        "INSERT OR IGNORE INTO categorias (nome) "
        "VALUES "
        "('Ficcao'),"
        "('Fantasia'),"
        "('Romance'),"
        "('Misterio/Suspense'),"
        "('Terror/Horror'),"
        "('Distopia'),"
        "('Ficcao Cientifica');"

        "INSERT OR IGNORE INTO livros "
        "(titulo, autor, genero, ano, paginas, ja_lido) "
        "VALUES "

        "('1984',"
        "'George Orwell',"
        "'Distopia',"
        "1949,"
        "328,"
        "1),"

        "('Duna',"
        "'Frank Herbert',"
        "'Ficcao Cientifica',"
        "1965,"
        "412,"
        "0),"

        "('Fahrenheit 451',"
        "'Ray Bradbury',"
        "'Distopia',"
        "1953,"
        "194,"
        "1),"

        "('Admiravel Mundo Novo',"
        "'Aldous Huxley',"
        "'Distopia',"
        "1932,"
        "311,"
        "0),"

        "('O Guia do Mochileiro das Galaxias',"
        "'Douglas Adams',"
        "'Ficcao Cientifica',"
        "1979,"
        "224,"
        "1),"

        "('Eu, Robo',"
        "'Isaac Asimov',"
        "'Ficcao Cientifica',"
        "1950,"
        "253,"
        "0),"

        "('2001: Uma Odisseia no Espaco',"
        "'Arthur C. Clarke',"
        "'Ficcao Cientifica',"
        "1968,"
        "297,"
        "1),"

        "('Fundacao',"
        "'Isaac Asimov',"
        "'Ficcao Cientifica',"
        "1951,"
        "255,"
        "0),"

        "('Frankenstein',"
        "'Mary Shelley',"
        "'Ficcao Cientifica',"
        "1818,"
        "280,"
        "1),"

        "('Neuromancer',"
        "'William Gibson',"
        "'Ficcao Cientifica',"
        "1984,"
        "271,"
        "1),"

        "('It: A Coisa',"
        "'Stephen King',"
        "'Terror/Horror',"
        "1986,"
        "1104,"
        "0),"

        "('O Iluminado',"
        "'Stephen King',"
        "'Terror/Horror',"
        "1977,"
        "447,"
        "0),"

        "('O Chamado de Cthulhu',"
        "'Howard Phillips Lovecraft',"
        "'Terror/Horror',"
        "1926,"
        "160,"
        "0),"

        "('A Estrada da Noite',"
        "'Joe Hill',"
        "'Terror/Horror',"
        "2007,"
        "400,"
        "0),"

        "('Fragmentos do Horror',"
        "'Junji Ito',"
        "'Terror/Horror',"
        "2020,"
        "224,"
        "0),"

        "('A Volta do Parafuso',"
        "'Henry James',"
        "'Terror/Horror',"
        "1898,"
        "160,"
        "0),"

        "('O Medico e o Monstro',"
        "'Robert Louis Stevenson',"
        "'Terror/Horror',"
        "1886,"
        "144,"
        "0),"

        "('Dracula',"
        "'Bram Stoker',"
        "'Terror/Horror',"
        "1897,"
        "576,"
        "0),"

        "('Carmilla',"
        "'Joseph Sheridan Le Fanu',"
        "'Terror/Horror',"
        "1872,"
        "108,"
        "0),"

        "('Os Evangelhos Escarlates',"
        "'Clive Barker',"
        "'Terror/Horror',"
        "2015,"
        "352,"
        "0),"

        "('O Rei de Amarelo',"
        "'Robert William Chambers',"
        "'Terror/Horror',"
        "1895,"
        "272,"
        "0),"

        "('O Senhor dos Aneis: A Sociedade do Anel',"
        "'J.R.R. Tolkien',"
        "'Fantasia',"
        "1954,"
        "576,"
        "0),"

        "('O Hobbit',"
        "'J.R.R. Tolkien',"
        "'Fantasia',"
        "1937,"
        "336,"
        "0),"

        "('Harry Potter e a Pedra Filosofal',"
        "'J.K. Rowling',"
        "'Fantasia',"
        "1997,"
        "264,"
        "0),"

        "('As Cronicas de Narnia',"
        "'C.S. Lewis',"
        "'Fantasia',"
        "1956,"
        "768,"
        "0),"

        "('Eragon',"
        "'Christopher Paolini',"
        "'Fantasia',"
        "2002,"
        "544,"
        "0),"

        "('O Nome do Vento',"
        "'Patrick Rothfuss',"
        "'Fantasia',"
        "2007,"
        "656,"
        "0),"

        "('Mistborn: O Imperio Final',"
        "'Brandon Sanderson',"
        "'Fantasia',"
        "2006,"
        "672,"
        "0),"

        "('A Guerra dos Tronos',"
        "'George R.R. Martin',"
        "'Fantasia',"
        "1996,"
        "592,"
        "0),"

        "('Percy Jackson e o Ladrao de Raios',"
        "'Rick Riordan',"
        "'Fantasia',"
        "2005,"
        "400,"
        "0),"

        "('A Roda do Tempo: O Olho do Mundo',"
        "'Robert Jordan',"
        "'Fantasia',"
        "1990,"
        "832,"
        "0),"

        "('Orgulho e Preconceito',"
        "'Jane Austen',"
        "'Romance',"
        "1813,"
        "424,"
        "0),"

        "('Como Eu Era Antes de Voce',"
        "'Jojo Moyes',"
        "'Romance',"
        "2012,"
        "320,"
        "0),"

        "('Romeu e Julieta',"
        "'William Shakespeare',"
        "'Romance',"
        "1597,"
        "288,"
        "0),"

        "('A Culpa e das Estrelas',"
        "'John Green',"
        "'Romance',"
        "2012,"
        "288,"
        "0),"

        "('Diario de uma Paixao',"
        "'Nicholas Sparks',"
        "'Romance',"
        "1996,"
        "256,"
        "0),"

        "('E Assim que Acaba',"
        "'Colleen Hoover',"
        "'Romance',"
        "2016,"
        "368,"
        "0),"

        "('Anna Karenina',"
        "'Leo Tolstoy',"
        "'Romance',"
        "1878,"
        "864,"
        "0),"

        "('O Morro dos Ventos Uivantes',"
        "'Emily Bronte',"
        "'Romance',"
        "1847,"
        "416,"
        "0),"

        "('Jane Eyre',"
        "'Charlotte Bronte',"
        "'Romance',"
        "1847,"
        "532,"
        "0),"

        "('Razao e Sensibilidade',"
        "'Jane Austen',"
        "'Romance',"
        "1811,"
        "368,"
        "0);";

    return executarSql(sql);
}

static int garantirConexao(void) {
    if (conexao != NULL) {
        return 1;
    }

    return inicializarBancoDeDados();
}

int inicializarBancoDeDados(void) {
    if (conexao != NULL) {
        return 1;
    }

    int bancoCriadoAgora = !arquivoExiste(CAMINHO_BANCO);

    if (sqlite3_open(CAMINHO_BANCO, &conexao) != SQLITE_OK) {
        fprintf(stderr,
                "Erro ao abrir banco de dados: %s\n",
                conexao != NULL ? sqlite3_errmsg(conexao) : "erro desconhecido");

        fecharBancoDeDados();
        return 0;
    }

    if (!fechamentoRegistrado) {
        atexit(fecharBancoDeDados);
        fechamentoRegistrado = 1;
    }

    if (!criarTabelas()) {
        fecharBancoDeDados();
        return 0;
    }

    if (bancoCriadoAgora && !carregarDadosIniciais()) {
        fecharBancoDeDados();
        return 0;
    }

    return 1;
}

void fecharBancoDeDados(void) {
    if (conexao != NULL) {
        sqlite3_close(conexao);
        conexao = NULL;
    }
}

Biblioteca* criarBibliotecaComBanco(int capacidadeInicial) {
    Biblioteca *bib = criarBiblioteca(capacidadeInicial);

    if (bib != NULL && !carregarLivrosDoBanco(bib)) {
        printf("Aviso: nao foi possivel carregar os livros do banco de dados.\n");
    }

    return bib;
}

int carregarLivrosDoBanco(Biblioteca *bib) {
    sqlite3_stmt *consulta = NULL;
    const char *sql =
        "SELECT id, titulo, autor, genero, ano, paginas, ja_lido "
        "FROM livros "
        "ORDER BY id;";

    if (bib == NULL || !garantirConexao()) {
        return 0;
    }

    if (sqlite3_prepare_v2(conexao, sql, -1, &consulta, NULL) != SQLITE_OK) {
        fprintf(stderr,
                "Erro ao preparar leitura dos livros: %s\n",
                sqlite3_errmsg(conexao));
        return 0;
    }

    int resultado;

    while ((resultado = sqlite3_step(consulta)) == SQLITE_ROW) {
        int id = sqlite3_column_int(consulta, 0);
        const unsigned char *titulo = sqlite3_column_text(consulta, 1);
        const unsigned char *autor = sqlite3_column_text(consulta, 2);
        const unsigned char *genero = sqlite3_column_text(consulta, 3);
        int ano = sqlite3_column_int(consulta, 4);
        int paginas = sqlite3_column_int(consulta, 5);
        int jaLido = sqlite3_column_int(consulta, 6);

        if (bib->quantidade == bib->capacidade) {
            expandirBiblioteca(bib);
        }

        bib->livros[bib->quantidade] = criarLivro(
            id,
            ano,
            paginas,
            jaLido,
            (char*) (titulo != NULL ? titulo : (const unsigned char*) ""),
            (char*) (autor != NULL ? autor : (const unsigned char*) ""),
            (char*) (genero != NULL ? genero : (const unsigned char*) "")
        );

        bib->quantidade++;
    }

    if (resultado != SQLITE_DONE) {
        fprintf(stderr,
                "Erro ao carregar livros: %s\n",
                sqlite3_errmsg(conexao));
        sqlite3_finalize(consulta);
        return 0;
    }

    sqlite3_finalize(consulta);
    return 1;
}

int inserirLivroNoBanco(const Livro *livro) {
    sqlite3_stmt *comando = NULL;
    const char *sql =
        "INSERT INTO livros "
        "(id, titulo, autor, genero, ano, paginas, ja_lido) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    if (livro == NULL || !garantirConexao()) {
        return 0;
    }

    if (sqlite3_prepare_v2(conexao, sql, -1, &comando, NULL) != SQLITE_OK) {
        fprintf(stderr,
                "Erro ao preparar cadastro do livro: %s\n",
                sqlite3_errmsg(conexao));
        return 0;
    }

    sqlite3_bind_int(comando, 1, livro->id);
    sqlite3_bind_text(comando, 2, livro->titulo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(comando, 3, livro->autor, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(comando, 4, livro->genero, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(comando, 5, livro->ano_de_publicacao);
    sqlite3_bind_int(comando, 6, livro->quantidade_de_paginas);
    sqlite3_bind_int(comando, 7, livro->ja_lido ? 1 : 0);

    int resultado = sqlite3_step(comando);

    if (resultado != SQLITE_DONE) {
        fprintf(stderr,
                "Erro ao inserir livro no banco de dados: %s\n",
                sqlite3_errmsg(conexao));
        sqlite3_finalize(comando);
        return 0;
    }

    sqlite3_finalize(comando);
    return 1;
}

int removerLivroDoBanco(int id) {
    sqlite3_stmt *comando = NULL;
    const char *sql = "DELETE FROM livros WHERE id = ?;";

    if (!garantirConexao()) {
        return 0;
    }

    if (sqlite3_prepare_v2(conexao, sql, -1, &comando, NULL) != SQLITE_OK) {
        fprintf(stderr,
                "Erro ao preparar remocao do livro: %s\n",
                sqlite3_errmsg(conexao));
        return 0;
    }

    sqlite3_bind_int(comando, 1, id);

    if (sqlite3_step(comando) != SQLITE_DONE) {
        fprintf(stderr,
                "Erro ao remover livro do banco de dados: %s\n",
                sqlite3_errmsg(conexao));
        sqlite3_finalize(comando);
        return 0;
    }

    int linhasAfetadas = sqlite3_changes(conexao);
    sqlite3_finalize(comando);

    return linhasAfetadas > 0;
}

int marcarLivroComoLidoNoBanco(int id) {
    sqlite3_stmt *comando = NULL;
    const char *sql = "UPDATE livros SET ja_lido = 1 WHERE id = ?;";

    if (!garantirConexao()) {
        return 0;
    }

    if (sqlite3_prepare_v2(conexao, sql, -1, &comando, NULL) != SQLITE_OK) {
        fprintf(stderr,
                "Erro ao preparar atualizacao do livro: %s\n",
                sqlite3_errmsg(conexao));
        return 0;
    }

    sqlite3_bind_int(comando, 1, id);

    if (sqlite3_step(comando) != SQLITE_DONE) {
        fprintf(stderr,
                "Erro ao marcar livro como lido no banco de dados: %s\n",
                sqlite3_errmsg(conexao));
        sqlite3_finalize(comando);
        return 0;
    }

    int linhasAfetadas = sqlite3_changes(conexao);
    sqlite3_finalize(comando);

    return linhasAfetadas > 0;
}
