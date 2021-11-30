/*
* Implementação de um Compilador para a Linguagem L
* Disciplina: Compiladores
* Professor: Alexei Manso Correa Machado
*
* Pontifícia Universidade Católica de Minas Gerais
* Coração Eucarístico
*
* Grupo(G11):
* Arthur Henrique(602718)
* Pedro Rodrigues(594451)
*
*/


#include <string>
#include <bits/stdc++.h>
#include <iostream>
using namespace std;

//Palavras reservadas da linguagem e seus respectivos lexemas

//Alfabeto                           Lexema
  #define CONST 1                    // const
  #define INT 2                      // int
  #define CHAR 3                     // char
  #define STRING 4                   // string
  #define WHILE 5                    // while
  #define IF 6                       // if
  #define FLOAT 7                    // float
  #define ELSE 8                     // else
  #define AND 9                      // &&
  #define OR 10                      // ||
  #define EXCLAMACAO 11              // !
  #define MENOR_MENOS 12             // <-
  #define IGUAL 13                   // =
  #define ABRE_PARENTESES 14         // (
  #define FECHA_PARENTESES 15        // )
  #define MENOR 16                   // <
  #define MAIOR 17                   // >
  #define DIFERENTE 18               // !=
  #define MAIOR_IGUAL 19             // >=
  #define MENOR_IGUAL 20             // <=
  #define VIRGULA 21                 // ,
  #define MAIS 22                    // + 
  #define MENOS 23                   // -
  #define ASTERISCO 24               // *
  #define BARRA 25                   // /
  #define PONTO_VIRGULA 26           // ;
  #define ABRE_CHAVE 27              // {
  #define FECHA_CHAVE 28             // }
  #define READLN 29                  // readln
  #define DIV 30                     // div
  #define WRITE 31                   // write
  #define WRITELN 32                 // writeln
  #define MOD 33                     // mod
  #define ABRE_COLCHETE 34           // [
  #define FECHA_COLCHETE 35          // ]
  #define ID 36                      //
  #define VALOR 37                   //
  #define SUBLINHADO 38              // _
  #define END 39                     // EOF

//Classes e tipos da linguagem

  #define CLASSE_VAR 40
  #define CLASSE_CONST 41

  #define TIPO_INT 42
  #define TIPO_FLOAT 43
  #define TIPO_CHAR 44
  #define TIPO_STR 45
  #define TIPO_VAZIO 46
  #define TIPO_BOOL 47

  int linha = 1;            // Variável Global para contagem de linhas do programa fonte
  int token_lido;           // Variável Global que armazena o token
  int linha_last_token;     // Variável Global que armazena a linha do último token encontrado

  ofstream arqDados;                // Declarando variável global para fluxo de saída
  ofstream arqComandos;             // Declarando variável global para fluxo de saída
  ifstream arqLeituraComandos;      // Declarando variável global para fluxo de entrada

  int contadorTemp = 0; // Variável Global para contagem de novos temporários
  int contadorDados = 0; // Variável Global para indexação na memória de dados do assembly
  int contadorRot = 0; // Variável Global para geração de novos temporários na geração de código

/*
* Tabela de Simbolos
*/
typedef struct Simbolo {
    int nToken;
    string lex;
    int tipo;
    int classe;
    int tam;
    int endereco;
} Simbolo; //struct Simbolo

typedef list<Simbolo> Lista;

/*
*   Struct Hash para auxiliar no gerenciamento da Tabela de Símbolos
*/
struct Hash {
    
    int bucket = 1399;   // Número de buckets  
    Lista table[1399];    

    public:
        
        int hashF(string lexema);
        Simbolo* inserirSimbolo(string lexema, int token, int tipo, int tamanho); 
        Simbolo* atualizarTS(string lexema, int tipo, int classe, int tamanho);
        int resgataTipo(string lexema);
        int resgataTamanho(string lexema);
        int resgataClasse(string lexema);
        int resgataEndereco(string lexema);
        void inserirPalavraReservada(string lexema, int token); 
        void preencherPR();
        Simbolo* pesquisar(string lexema);

} Hash; //struct Hash

/*
* Método para calcular uma saída para uma key(string) recebida
*/
int Hash::hashF(string lexema) {
    int i, aux = 0;
    for (i = 0; i < lexema.length(); i++) {
        aux += lexema.at(i);
    }
    return (aux % bucket);
} //hashF()

/*
*   Método para inserir um novo Símbolo na Tabela e retornar seu endereço de inserção
*/
Simbolo* Hash::inserirSimbolo(string lexema, int token, int tipo, int tamanho) {
    
    Simbolo simbol;
    simbol.lex = lexema;
    simbol.nToken = token;
    simbol.tipo = tipo;
    simbol.tam = tamanho;

    int index = hashF(lexema);
    table[index].insert(table[index].end(), simbol);
    
    return &table[index].back();

} //inserirSimbolo()

/*
*   Método para atualizar a Tabela de Simbolos com as novas informações de um ID
*/
Simbolo* Hash::atualizarTS(string lexema, int tipo, int classe, int tamanho) {
    Simbolo* simbol;
    simbol = pesquisar(lexema);
    simbol->classe = classe;
    simbol->tipo = tipo;
    simbol->tam = tamanho;
    simbol->endereco = contadorDados;//Endereço da memória no assembly
    return simbol;
}//fim atualizarTS()

/*
*   Método para resgatar o tipo de um identificador na Tabela de Simbolos
*/
int Hash::resgataTipo(string lexema) {
    Simbolo* simbol;
    simbol = pesquisar(lexema);
    return simbol->tipo;
} //fim resgataClasse()

/*
*   Método para resgatar o tamanho de um identificador na Tabela de Simbolos
*/
int Hash::resgataTamanho(string lexema) {
    Simbolo* simbol;
    simbol = pesquisar(lexema);
    return simbol->tam;
} //fim resgataClasse()

/*
*   Método para resgatar o endereco de um identificador na memória 
*/
int Hash::resgataEndereco(string lexema) {
    Simbolo* simbol;
    simbol = pesquisar(lexema);
    return simbol->endereco;
} //fim resgataEndereco()

/*
*   Método para resgatar a classe de um identificador na Tabela de Simbolos
*/
int Hash::resgataClasse(string lexema) {
    Simbolo* simbol;
    simbol = pesquisar(lexema);
    return simbol->classe;
} //fim resgataClasse()

/*
*   Método para inserir uma palavra reservada e o token correspondente na Tabela de Simbolos
*/
void Hash::inserirPalavraReservada(string lexema, int token) {
    
    Simbolo simbol;
    simbol.lex = lexema;
    simbol.nToken = token;

    int index = hashF(lexema);
    table[index].insert(table[index].end(), simbol);

} //inserirPalavraReservada()

/*
*   Método para preencher a Tabela de Símbolos com as palavras reservadas e seus respectivos tokens
*/
void Hash::preencherPR() {

    inserirPalavraReservada("const", CONST);
    inserirPalavraReservada("int", INT);
    inserirPalavraReservada("char", CHAR);
    inserirPalavraReservada("string", STRING);
    inserirPalavraReservada("while", WHILE);
    inserirPalavraReservada("if", IF);
    inserirPalavraReservada("float", FLOAT);   
    inserirPalavraReservada("else", ELSE);
    inserirPalavraReservada("&&", AND);
    inserirPalavraReservada("||", OR);
    inserirPalavraReservada("!", EXCLAMACAO);   
    inserirPalavraReservada("<-", MENOR_MENOS);
    inserirPalavraReservada("=", IGUAL);
    inserirPalavraReservada("(", ABRE_PARENTESES);
    inserirPalavraReservada(")", FECHA_PARENTESES);
    inserirPalavraReservada("<", MENOR);
    inserirPalavraReservada(">", MAIOR);
    inserirPalavraReservada("!=", DIFERENTE);
    inserirPalavraReservada(">=", MAIOR_IGUAL);
    inserirPalavraReservada("<=", MENOR_IGUAL);
    inserirPalavraReservada(",", VIRGULA);
    inserirPalavraReservada("+", MAIS);
    inserirPalavraReservada("-", MENOS);
    inserirPalavraReservada("*", ASTERISCO);
    inserirPalavraReservada("/", BARRA);
    inserirPalavraReservada(";", PONTO_VIRGULA);    
    inserirPalavraReservada("{", ABRE_CHAVE);
    inserirPalavraReservada("}", FECHA_CHAVE);
    inserirPalavraReservada("readln", READLN);
    inserirPalavraReservada("div", DIV);
    inserirPalavraReservada("write", WRITE);
    inserirPalavraReservada("writeln", WRITELN);
    inserirPalavraReservada("mod", MOD);    
    inserirPalavraReservada("[", ABRE_COLCHETE);
    inserirPalavraReservada("]", FECHA_COLCHETE);
    inserirPalavraReservada("_", SUBLINHADO);

} //fim preencherPR()

/*
*   Método para pesquisar na Tabela um Símbolo por um lexema 
*   Se encontrar retorna o endereço de inserção , se não, retorna NULL
*/    
Simbolo* Hash::pesquisar(string lexema) {
    
    Simbolo* retorno = NULL;    
    int index = hashF(lexema);
  
    for (auto i = table[index].begin(); i != table[index].end(); i++) {
        if (i->lex == lexema) {
            retorno = &*i;
        }
    }
    return retorno;
} //pesquisar()

/*
* Método que retorna um endereço para um novo temporário
*/
int novoTemp(int tipo) {
    
    int temp = contadorTemp;

    if (tipo == TIPO_STR) {
        contadorTemp += 32;
    } else if(tipo == TIPO_CHAR) {
        contadorTemp += 1;
    } else {//TIPO_INT || TIPO_FLOAT
        contadorTemp += 4;
    }
    return temp;
}//fim novoTemp()

/*
* Método que retorna um novo rótulo para demarcar pontos de jump na geração de código
*/
string novoRot() {
    return "Rot"+std::to_string(contadorRot++);
}//fim novoRot()


/*
*   Registro Léxico
*   Para armazenar informações sobre o token encontrado pelo Analisador Léxico
*/
struct REGLEX {
    int token;
    int tipo;
    int tam;
    string lexema;
    Simbolo* pos;

    void reset() {
        token = -1;
        tipo = -1;
        tam = 0;
        lexema = "";
        pos =  NULL;
    }

}reg_lex; //struct REGLEX

/*
* Método que verifica se o caractere lido é válido para a linguagem L
*/
bool invalido(char c){

    bool invalido = true;
    
    // Caracteres aceitos na linguagem L
    if (((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) || (c >= '0' && c <= '9') || c == ' ' ||
        c == '_' || c == '.' || c == ',' || c == ';' || c == ':' || c == '(' || c == ')' || c == '[' ||
        c == ']' || c == '{' || c == '}' || c == '+' || c == '-' || c == '\"' || c == '\'' || c == '/' ||
        c == '|' || c == '\\' || c == '&' || c == '%' || c == '!' || c == '?' || c == '>' || c == '<' || c == '=' ||
        c == '*' || c == '\n' || c == '\t' || c == '\r' ) {

            invalido = false;   // Não é inválido
    }

    return invalido;

} //invalido()

/*************************************
* Analisador Léxico
**************************************/
int analisadorLexico() {
    
    reg_lex.reset();    // Registro léxico é reiniciado para reter as informações do próximo token encontrado
    int S = 0;          // Estado inicial 
    char c;             // Caractere de leitura
    string lex;         // String que conterá o lexema do token encontrado

    while(S != 3)  {            // Repita até o estado final
        
        if (c = cin.get()) {    // Se houver conteúdo no programa fonte, leia
            
            if (c == EOF) {     // Verificação de fim de arquivo
                
                if (S == 0 && reg_lex.token != END) {   // Caso esteja no estado inicial, verificamos se possuimos no registro léxico o EOF
                    linha = linha_last_token;   // linha recebe a linha armazenada do ultimo token lido para evitar uma soma de linha equivocada
                    linha++;    // linha do EOF é contabilizada
                    reg_lex.token = END;    // token recebe o fim de arquivo
                    return reg_lex.token;
                }
                reg_lex.token = END;  // Registro léxico recebe o token de fim de arquivo 

            } else if(invalido(c)) {    // Verificação de caractere inválido
                throw(1);   // ERRO: Caractere inválido
            }
        }

        switch (S) {
            
            case 0:

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                } else if(c == '\n') {
                    linha++;
                } else if(c == '=') {
                    reg_lex.token = IGUAL;
                    lex += c;
                    S = 3;
                } else if(c == '(') {
                    reg_lex.token = ABRE_PARENTESES;
                    lex += c;
                    S = 3;
                } else if(c == ')') {
                    reg_lex.token = FECHA_PARENTESES;
                    lex += c;
                    S = 3;
                } else if(c == ',') {
                    reg_lex.token = VIRGULA;
                    lex += c;
                    S = 3;
                } else if(c == '+') {
                    reg_lex.token = MAIS;
                    lex += c;
                    S = 3;
                } else if(c == '-') {
                    reg_lex.token = MENOS;
                    lex += c;
                    S = 3;
                } else if(c == '*') {
                    reg_lex.token = ASTERISCO;
                    lex += c;
                    S = 3;
                } else if(c == ';') {
                    reg_lex.token = PONTO_VIRGULA;
                    lex += c;
                    S = 3;
                } else if(c == '{') {
                    reg_lex.token = ABRE_CHAVE;
                    lex += c;
                    S = 3;
                } else if(c == '}') {
                    reg_lex.token = FECHA_CHAVE;
                    lex += c;
                    S = 3;
                } else if(c == '[') {
                    reg_lex.token = ABRE_COLCHETE;
                    lex += c;
                    S = 3;
                } else if(c == ']') {
                    reg_lex.token = FECHA_COLCHETE;
                    lex += c;
                    S = 3;
                } else if(c == '\'') {
                    lex += c;
                    S = 1;
                } else if(c == '0') {
                    lex += c;
                    S = 4;
                } else if(c >= '1' && c <= '9') {
                    lex += c;
                    S = 7;
                } else if(((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) || c == '_') {
                    lex += c;
                    S = 8;
                } else if(c == '\"') {
                    lex += c;
                    S = 9;
                } else if(c == '&') {
                    lex += c;
                    S = 10;
                } else if(c == '|') {
                    lex += c;
                    S = 11;
                } else if(c == '!') {
                    lex += c;
                    S = 12;
                } else if(c == '<') {
                    lex += c;
                    S = 13;
                } else if(c == '>') {
                    lex += c;
                    S = 14;
                } else if(c == '/') {
                    lex += c;
                    S = 15;
                } else if(c == '.') {
                    lex += c;
                    S = 18;
                } else {    // ERRO: Lexema não identificado
                    if (c != '\n' && c != '\t' && c != '\r' && c != ' ' ) { // Para identificar erros com caracteres permitidos
                                                                            // mas que não formam Tokens
                        lex += c;
                        reg_lex.lexema = lex;
                        throw(0);
                    }//fim if
                }//fim else
                break;
            
            case 1:     // CHARS (LETRAS OU DÍGITOS)

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
                    lex += c;
                    S = 2;
                } else {    // ERRO: Lexema não identificado
                    reg_lex.lexema = lex;
                    throw(0);
                }//fim else
                break;

            case 2:

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c == '\'') {
                    lex += c;
                    S = 3;
                    reg_lex.token = VALOR;
                    reg_lex.tipo = TIPO_CHAR;
                    reg_lex.tam = lex.size();
                } else {    // ERRO: Lexema não identificado
                    reg_lex.lexema = lex;
                    throw(0);
                }//fim else
                break;

            case 4:     // CONSTANTES NÚMERICAS (FLOATS, INTS, HEXADECIMAIS)

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c >= '0' && c <= '9') {
                    lex += c;
                    S = 7;
                } else if(c == 'x') {
                    lex += c;
                    S = 5;
                } else if(c == '.') {
                    lex += c;
                    S = 20;
                } else {
                    S = 3;
                    reg_lex.token = VALOR;
                    reg_lex.tipo = TIPO_INT;
                    reg_lex.tam = lex.size();
                    cin.unget();
                }//fim else
                break;

            case 5:     // HEXADECIMAIS

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
                    lex += c;
                    S = 6;
                } else {    // ERRO: Lexema não identificado
                    reg_lex.lexema = lex;
                    throw(0);
                }//fim else
                break;

            case 6:     // HEXADECIMAIS

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
                    lex += c;
                    reg_lex.token = VALOR;
                    reg_lex.tipo = TIPO_CHAR;
                    reg_lex.tam = lex.size();
                    S = 3;
                } else {    // ERRO: Lexema não identificado
                    reg_lex.lexema = lex;
                    throw(0);
                }//fim else
                break;

            case 7:     // FLOATS, INTS

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c == '.') {
                    lex += c;
                    S = 20;
                } else if(c >= '0' && c <= '9') {
                    lex += c;
                    
                } else {
                    S = 3;
                    reg_lex.token = VALOR;
                    reg_lex.tipo = TIPO_INT;
                    reg_lex.tam = lex.size();
                    cin.unget();
                }//fim else
                break;

            case 8:     // IDENTIFICADORES

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '.')  {
                    lex += c;
                } else {
                   reg_lex.token = ID;
                   reg_lex.tipo = TIPO_VAZIO;
                   reg_lex.tam = lex.size();
                   cin.unget();
                   S = 3;
                }//fim else
                break;

            case 9:     // STRINGS

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c != '\n' && c != '\"') {
                    lex += c;
                } else if(c == '\"') {
                    lex += c;
                    reg_lex.token = VALOR;
                    reg_lex.tipo = TIPO_STR;
                    reg_lex.tam = lex.size();
                    S = 3;
                } else {    // ERRO: Lexema não identificado
                    reg_lex.lexema = lex;
                    throw(0);
                }//fim else
                break;

            case 10:        // &&

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c == '&') {
                    lex += c;
                    reg_lex.token = AND;
                    S = 3;
                } else {    // ERRO: Lexema não identificado
                    reg_lex.lexema = lex;
                    throw(0);
                }//fim else
                break;

            case 11:        // ||

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c == '|') {
                    lex += c;
                    reg_lex.token = OR;
                    S = 3;
                } else {    // ERRO: Lexema não identificado
                    reg_lex.lexema = lex;
                    throw(0);
                }//fim else
                break;

            case 12:        // ! ou !=

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c == '=') {
                    lex += c;
                    reg_lex.token = DIFERENTE;
                    S = 3;
                } else if (c != '=') {
                    reg_lex.token = EXCLAMACAO;
                    cin.unget();
                    S = 3;
                }//fim if
                break;

            case 13:        // < ou <=

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c == '-') {
                    lex += c;
                    reg_lex.token = MENOR_MENOS;
                    S = 3;
                } else if(c == '=') {
                    lex += c;
                    reg_lex.token = MENOR_IGUAL;
                    S = 3;
                } else {
                    cin.unget();
                    reg_lex.token = MENOR;
                    S = 3;
                }//fim else
                break;

            case 14:        // > ou >=

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c == '=') {
                    lex += c;
                    reg_lex.token = MAIOR_IGUAL;
                    S = 3;
                } else {
                    reg_lex.token = MAIOR;
                    cin.unget();
                    S = 3;
                }//fim else
                break;

            case 15:        //    / ou COMENTÁRIOS

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c != '*') {
                    reg_lex.token = BARRA;
                    cin.unget();
                    S = 3;
                } else {
                    S = 16;
                    lex = "";
                }//fim else
                break;

            case 16:        // COMENTÁRIOS

                if(c == '\n') {
                    linha++;
                }//fim if

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if                

                if(c == '*') {
                    S = 17;
                }//fim if 
                break;

            case 17:        // COMENTÁRIOS

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c == '/') {
                    S = 0;
                } else if(c == '\n') {
                    linha ++;
                } else if(c != '/' && c != '*') {
                    S = 16;
                }//fim if
                break;

            case 18:        // FLOATS QUE COMEÇAM COM .

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c >= '0' && c <= '9') {
                    lex += c;
                    S = 19;
                } else {    // ERRO: Lexema não identificado
                    reg_lex.lexema = lex;
                    throw(0);
                }//fim else
                break;

            case 19:        // FLOATS QUE COMEÇAM COM .

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(!(c >= '0' && c <= '9')) {
                    reg_lex.token = VALOR;
                    reg_lex.tipo = TIPO_FLOAT;
                    reg_lex.tam = lex.size();
                    S = 3;
                    cin.unget();
                } else {
                    lex += c;
                }//fim else
                break;

            case 20:        // FLOATS

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(c >= '0' && c <= '9') {
                    lex += c;
                    S = 21;
                } else {    // ERRO: Lexema não identificado
                    reg_lex.lexema = lex;
                    throw(0);
                }//fim else
                break;

            case 21:        // FLOATS

                if(c == EOF) {  // ERRO: Fim de arquivo não esperado
                    throw(2);
                }//fim if

                if(!(c >= '0' && c <= '9')) {
                    reg_lex.token = VALOR;
                    reg_lex.tipo = TIPO_FLOAT;
                    reg_lex.tam = lex.size();
                    S = 3;
                    cin.unget();
                } else {
                    lex += c;
                }//fim else
                break;

        } //fim switch

    } //fim while  

    reg_lex.lexema = lex;       // Entregar lexema para o registro léxico    
    Simbolo* simbol;            // Novo Simbolo criado para reter as informações do token
    
    /*
    * Caso token for ID haverá uma consulta à Tabela de Simbolos para sua inserção, caso ainda não inserido, 
    * ou seu retorno, caso contrário
    */
    if (reg_lex.token == ID) {
        simbol = Hash.pesquisar(reg_lex.lexema);//Pesquisa, pelo lexema, à tabela de simbolos
        
        if (simbol != NULL) {//Se Simbolo != NULL, o lexema já existe, então o registro léxico recebe seu conteúdo
            reg_lex.token = simbol->nToken; 
            reg_lex.tipo = simbol->tipo;
            reg_lex.lexema = simbol->lex;
            reg_lex.pos = simbol;        
        } else {//Se Simbolo == NULL, posição do registro léxico recebe seu endereço de inserção na Tabela de Simbolos
            reg_lex.pos = Hash.inserirSimbolo(reg_lex.lexema, ID, reg_lex.tipo, reg_lex.tam);
        } 
    }//fim if

    linha_last_token = linha;//linha_last_token armazena a linha do último token lido
    return reg_lex.token;
     
} //fim analisadorLexico()

/*
* Método, chamado pelo Analisador Sintático, que verifica a correspondência de tokens e, caso haja tal
* correspondência, chamará o Analisador Léxico para ler o próximo token
*/ 
void CasaToken(int token_esperado) {
    if(token_lido == token_esperado)
        token_lido = analisadorLexico();
    else if (token_lido == END) {
            linha--;
            throw(2);//ERRO: Fim de Arquivo não esperado
        } else
            throw(3);//ERRO: Token não esperado
} //CasaToken()

/*
*   Método que verificará a compatibilidade de tipos na declaração e atribuição de identificadores
*/
void verificaCompatibilidadeAtr_Dec(int tipo1, int tipo2) {
    switch(tipo1) {
        case TIPO_INT:   if(tipo2 == TIPO_INT) 
                             break;
                         else 
                             throw(7);
            
        case TIPO_CHAR:  if(tipo2 == TIPO_CHAR)
                             break;
                         else 
                             throw(7);

        case TIPO_STR:   if(tipo2 == TIPO_STR)
                             break;
                         else        
                             throw(7);

        case TIPO_FLOAT: if(tipo2 == TIPO_FLOAT || tipo2 == TIPO_INT)
                             break;
                         else       
                             throw(7); 
    }//fim switch
}//fim verificaCompatibilidadeAtr_Dec()

/*
*   Método que verifica a compatibilidade de tipos das expressões
*/
void verificaTipos(int tipo1, int tipo2, int operador) {
    switch(operador) {
        case ASTERISCO: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else
                            throw(7);

        case AND: if(tipo1 == TIPO_BOOL && tipo2 == TIPO_BOOL)
                        break;
                  else  
                        throw(7);

        case BARRA: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else
                            throw(7);

        case DIV: if(tipo1 == TIPO_INT && tipo2 == TIPO_INT) 
                        break;
                    else    
                        throw(7);
            

        case MOD: if(tipo1 == TIPO_INT && tipo2 == TIPO_INT) 
                        break;
                    else    
                        throw(7);


        case MAIS: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else
                            throw(7);
            

        case MENOS: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else
                            throw(7);
            

        case OR: if(tipo1 == TIPO_BOOL && tipo2 == TIPO_BOOL)
                        break;
                  else  
                        throw(7);
            

        case IGUAL: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else if(tipo1 == TIPO_STR && tipo2 == TIPO_STR) {
                                    break;
                        } else if(tipo1 == TIPO_CHAR && tipo2 == TIPO_CHAR) {
                                    break;
                        } else if(tipo1 == TIPO_BOOL && tipo2 == TIPO_BOOL)
                                    break;
                                
                        
                    throw(7);

                                        
        case DIFERENTE: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else if(tipo1 == TIPO_BOOL && tipo2 == TIPO_BOOL){
                                break;
                        } else if(tipo1 == TIPO_CHAR && tipo2 == TIPO_CHAR) {
                                break;
                        } else
                            throw(7);
           

        case MENOR: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else if(tipo1 == TIPO_CHAR && tipo2 == TIPO_CHAR) {
                                break;
                        } else
                            throw(7);
           

        case MAIOR: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else if(tipo1 == TIPO_CHAR && tipo2 == TIPO_CHAR) {
                                break;
                        } else
                            throw(7);
            

        case MENOR_IGUAL: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else if(tipo1 == TIPO_CHAR && tipo2 == TIPO_CHAR) {
                                break;
                        } else
                            throw(7);
            

        case MAIOR_IGUAL: if(tipo1 == TIPO_INT) {
                            if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                break;
                            else    
                                throw(7);
                        } else if(tipo1 == TIPO_FLOAT) {
                                    if(tipo2 == TIPO_INT || tipo2 == TIPO_FLOAT)
                                        break;
                                    else
                                        throw(7);
                        } else if(tipo1 == TIPO_CHAR && tipo2 == TIPO_CHAR) {
                                break;
                        } else
                            throw(7);
            
        
    } //fim switch
}//fim verificaTipos()


/*
*   Método que retorna o tipo da expressão
*/
int calculaExp_tipo(int tipo1, int tipo2) {
    int resp;

    if(tipo1 == TIPO_INT && tipo2 == TIPO_FLOAT)
        resp = TIPO_FLOAT;
    else if(tipo1 == TIPO_FLOAT && tipo2 == TIPO_INT)
        resp = TIPO_FLOAT;
    else if(tipo1 == TIPO_FLOAT && tipo2 == TIPO_FLOAT)
        resp = TIPO_FLOAT;
    else if(tipo1 == TIPO_INT && tipo2 == TIPO_INT)
        resp = TIPO_INT;
        
    return resp;
}//fim calculaExp_Tipo()


/*************************************
* Analisador Sintático e Semântico
**************************************/
class Sintatico {
public:
/* 
F        -> !F1(1) | "("Exp(2)")" | ("int"(6) | "float"(7))"("Exp(8)")" | const(3) | id(4)["["Exp(5)"]"]
(1) {se (f1_tipo != TIPO_BOOL) entao ERRO senao f_tipo = f1_tipo; f_tam = f1_tam; f_valor = f1_valor}
(2) {f_tipo = exp_tipo; f_tam = exp_tam; f_valor = exp_valor}
(3) {f_tipo = const_tipo; f_tam = const_tam; f_valor = const_valor}
(4) {se (id_tipo == TIPO_VAZIO) entao ERRO senao f_tipo = id_tipo; f_tam = id_tam}
(5) {se (exp_tipo != TIPO_INT || id_tipo != TIPO_STR) entao ERRO senao f_tipo = TIPO_CHAR; f_tam = exp_tam}
(6) {f_tipo = TIPO_INT}
(7) {f_tipo = TIPO_FLOAT}
(8) {se (exp_tipo != INT && exp_tipo != FLOAT) entao ERRO senao f_tam = exp_tam; f_valor = exp_valor}
*/
    void F(int &f_tipo, int &f_tam, int &f_end) {
        int f1_tipo, f1_tam, f1_end;
        int exp_tipo, exp_tam, exp_end;
        int const_tipo, const_tam;
        int id_tipo, id_tam, id_end;
        
        if(token_lido == EXCLAMACAO) {

            CasaToken(EXCLAMACAO);
            F(f1_tipo, f1_tam, f1_end);

            if(f1_tipo != TIPO_BOOL)
                throw(7);

            f_end = novoTemp(f1_tipo);
            f_tipo = f1_tipo;
            f_tam = f1_tam;

            // Simulação do 'não' lógico - !F1
            arqComandos << "\tmov eax,[M+" << f1_end << "]" << endl;
            arqComandos << "\tneg eax" << endl;
            arqComandos << "\tadd eax,1" << endl;
            arqComandos << "\tmov[M+" << f_end << "],eax" << endl;
            
            
        } else if(token_lido == ABRE_PARENTESES) {
            CasaToken(ABRE_PARENTESES);

            Exp(exp_tipo, exp_tam, exp_end);

            f_end = exp_end;
            f_tipo = exp_tipo;
            f_tam = exp_tam;

            CasaToken(FECHA_PARENTESES);

        } else if(token_lido == INT || token_lido == FLOAT) {
            if(token_lido == INT) {
                
                CasaToken(INT);
                f_tipo = TIPO_INT;

                CasaToken(ABRE_PARENTESES);

                Exp(exp_tipo, exp_tam, exp_end);

            } else {

                CasaToken(FLOAT);
                f_tipo = TIPO_FLOAT;

                CasaToken(ABRE_PARENTESES);
                
                Exp(exp_tipo, exp_tam, exp_end);
                
            }

            if(exp_tipo != TIPO_FLOAT && exp_tipo != TIPO_INT)
                throw(7);

            f_tam = exp_tam;
            f_end = exp_end;

                                                                                                                                                                                                                                                                                                                     
            // ( "int" | "float" ) "(" Exp ")"
            if(f_tipo == TIPO_INT) {

                arqComandos << ";CAST PARA INT" << endl;

                if(exp_tipo == TIPO_INT) { // Se exp_tipo for inteiro, carregamos direto (redundância)

                    f_end = novoTemp(TIPO_INT);
                    arqComandos << "\tmov eax,[M+" << exp_end << "]" << endl;
                    arqComandos << "\tmov [M+" << f_end << "],eax" << endl;

                } else if (exp_tipo == TIPO_FLOAT) { // Se exp for float, fazemos a conversão

                    f_end = novoTemp(TIPO_INT);
                    arqComandos << "\tmovss xmm0,[M+" << exp_end << "]" << endl;//Carregamos valor da memoria
                    arqComandos << "\tcvtss2si rax,xmm0" << endl;//Convertemos para inteiro, rax
                    arqComandos << "\tmov [M+" << f_end << "],eax" << endl;//Movemos parte baixa de rax -> eax
                    
                }

                arqComandos << ";FIM CAST PARA INT" << endl;

            } else if (f_tipo == TIPO_FLOAT) {

                arqComandos << ";CAST PARA FLOAT" << endl;

                if(exp_tipo == TIPO_INT) { // Se exp_tipo for inteiro, fazemos a conversão

                    f_end = novoTemp(TIPO_FLOAT);
                    arqComandos << "\tmov eax,[M+" << exp_end << "]" << endl;//Carregamos valor da memoria
                    arqComandos << "\tcdqe" << endl;//Expansão de sinal
                    arqComandos << "\tcvtsi2ss xmm0, rax" << endl;//Conversão para float
                    arqComandos << "\tmovss[M+" << f_end << "], xmm0" << endl;//Movemos para memória

                } else if (exp_tipo == TIPO_FLOAT) { // Se exp_tipo for float, carregamos direto (redundância)
                    
                    f_end = novoTemp(TIPO_FLOAT);
                    arqComandos << "\tmovss xmm0,[M+" << exp_end << "]" << endl;
                    arqComandos << "\tmovss [M+" << f_end << "],xmm0" << endl;
                }

                arqComandos << ";FIM CAST PARA FLOAT" << endl;

            }                   
            
            CasaToken(FECHA_PARENTESES);

        // const
        } else if(token_lido == VALOR) {
            const_tipo = reg_lex.tipo;
            const_tam = reg_lex.tam; 
            f_tipo = const_tipo;
            f_tam = const_tam;

            if(const_tipo == TIPO_FLOAT || const_tipo == TIPO_STR) {
                f_end = contadorDados;
                if(const_tipo == TIPO_FLOAT) {
                    contadorDados += 4;
                    arqDados << "\tdd " << reg_lex.lexema <<"\t\t\t; Float carregado em "<< f_end << endl;       //Declara na sessão de dados um float
                } else {
                    contadorDados += const_tam -1; //-1 de (-2 das aspas +1 do '\0')
                    arqDados << "\tdb " << reg_lex.lexema <<",0"<<"\t; String carregado em "<< f_end << endl;    //Declara na sessão de dados uma string
                }

            } else {    // Se for um char ou inteiro, carregamos o valor direto
                f_end = novoTemp(const_tipo);
                string reg, imed;

                if(const_tipo == TIPO_CHAR) 
                    reg = "al";     // Char registrador de 8
                else 
                    reg = "eax";    // Int registrador de 32
                
                imed = reg_lex.lexema;

                arqComandos << "\tmov " << reg << ", " << imed << endl;//Move imediato para o registrador correspondente
                arqComandos << "\tmov [M+" << f_end << "], " << reg << endl;//Carrega na memória o registrador no próximo endereço disponível
            }
            
            CasaToken(VALOR);

        } else {
            
            id_tipo = Hash.resgataTipo(reg_lex.lexema);
            id_tam = Hash.resgataTamanho(reg_lex.lexema);
            id_end = Hash.resgataEndereco(reg_lex.lexema);

            if(id_tipo == TIPO_VAZIO)
                throw(4);


            f_tipo = id_tipo;
            f_tam = id_tam;
            f_end = id_end;
            
            CasaToken(ID);
            
            if(token_lido == ABRE_COLCHETE) {//Se for elemento de string(caractere)
                CasaToken(ABRE_COLCHETE);
                
                exp_tam = reg_lex.tam;

                Exp(exp_tipo, exp_tam, exp_end);               

                if (exp_tipo != TIPO_INT || id_tipo != TIPO_STR) 
                    throw(7);

                f_tipo = TIPO_CHAR;
                f_end = novoTemp(f_tipo);
                f_tam = exp_tam;   

                // id "[" Exp "]"   
                /* 
                arqComandos << "\tmov eax, [M+" << exp_end << "]" << endl;  //Carrega no registrador o que está na memória [M+exp_end]
                arqComandos << "\tadd eax," << id_end << endl;  // Calcula deslocamento
                arqComandos << "\tmov ebx,[M+eax]" << endl; // ebx recebe caractere calculado pelo deslocamento
                arqComandos << "\tmov [M+" << f_end << "],ebx" << endl; // Transfere o conteúdo de ebx para o novo temporário
                */
                arqComandos << "\tmov rax," << id_end  << endl;  //Carrega no registrador o que está na memória [M+exp_end]
                arqComandos << "\tadd rax,[M+" << exp_end << "]" << endl;  // Calcula deslocamento
                arqComandos << "\tmov ebx,[M+rax]" << endl; // ebx recebe caractere calculado pelo deslocamento
                arqComandos << "\tmov [M+" << f_end << "],ebx" << endl; // Transfere o conteúdo de ebx para o novo temporário
                
                CasaToken(FECHA_COLCHETE);
            }
        }
    } //fim F()


/* 
T        -> F1(1) {(*(2) | &&(3) | /(4) | "div"(5) | "mod"(6) ) F2(8)(7)}
(1) {t_tipo = f1_tipo; t_tam = f1_tam}
(2) {t_operador = ASTERISCO}
(3) {t_operador = AND; t_tipo = TIPO_BOOL}
(4) {t_operador = BARRA}
(5) {t_operador = DIV}
(6) {t_operador = MOD}
(7) {verificaTipos(f1_tipo, f2_tipo, t_operador)}
(8) {se (t_operador == BARRA) entao t_tipo = TIPO_FLOAT senao se (t_operador == ASTERISCO) entao t_tipo = calculaExp_Tipo(f1_tipo, f2_tipo)}
*/   
    void T(int &t_tipo, int &t_tam, int &t_end) {
        int f1_tipo, f1_tam, f1_end;
        int f2_tipo, f2_tam, f2_end;
        int t_operador;

        F(f1_tipo, f1_tam, f1_end);
        t_tipo = f1_tipo;
        t_tam = f1_tam;
        t_end = f1_end;

        while (token_lido == ASTERISCO || token_lido == AND || token_lido == BARRA || token_lido == DIV || token_lido == MOD) {
            
            if (token_lido == ASTERISCO) { 
                CasaToken(ASTERISCO);
                t_operador = ASTERISCO;
            }else if (token_lido == AND) {                
                CasaToken(AND);
                t_operador = AND;
                t_tipo = TIPO_BOOL;
            }else if (token_lido == BARRA) { 
                CasaToken(BARRA);
                t_operador = BARRA;           
            }else if (token_lido == DIV) {
                CasaToken(DIV);
                t_operador = DIV;
            } else {
                CasaToken(MOD);
                t_operador = MOD;
            }
                
            F(f2_tipo, f2_tam, f2_end);

            if(t_operador == BARRA)//Divisão real, t_tipo recebe tipo float direto
                t_tipo = TIPO_FLOAT;
            else if(t_operador == ASTERISCO)//Senão, calcula exp_tipo
                t_tipo = calculaExp_tipo(f1_tipo, f2_tipo);

            // Carregando corretamente os registradores de acordo com o t_tipo
            if(t_tipo == TIPO_INT) {                                
                
                arqComandos << "\tmov eax,[M+" << f1_end << "]" << endl;    // Carregar o conteúdo do T_endereço para o registrador EAX
                arqComandos << "\tmov ebx,[M+" << f2_end << "]" << endl;    // Carregar o conteúdo do T_endereço para o registrador EBX
                
            } else if(t_tipo == TIPO_FLOAT) {

                if(f1_tipo == TIPO_INT) { // Se tipo for int fazemos a conversão
                    arqComandos << "\tmov eax,[M+" << f1_end << "]" << endl;    // Carregar o conteúdo do T_endereço para o registrador EAX
                    arqComandos << "\tcdqe" << endl;    // Estende o sinal do registrador EAX
                    arqComandos << "\tcvtsi2ss xmm0,rax" << endl;    // Converte conteúdo do registrador RAX para float e guarda no registrador XMM0

                } else { // senão carregamos o valor direto 
                    arqComandos << "\tmovss xmm0,[M+" << f1_end << "]" << endl;    // Carregar o conteúdo do T_endereço para o registrador XMM0
                }

                if (f2_tipo == TIPO_INT) {//Se tipo for int fazemos a conversão
                    arqComandos << "\tmov eax,[M+" << f2_end << "]" << endl;    // Carregar o conteúdo do T_endereço para o registrador EAX
                    arqComandos << "\tcdqe" << endl;    // Estende o sinal do registrador EAX
                    arqComandos << "\tcvtsi2ss xmm1,rax" << endl;    // Converte conteúdo do registrador RAX para float e guarda no registrador XMM1
                } else { // senão carregamos o valor direto 
                    arqComandos << "\tmovss xmm1,[M+" << f2_end << "]" << endl;    // Carregar o conteúdo do T_endereço para o registrador XMM1
                }            
                
            } else if(t_tipo == TIPO_BOOL) {
                
                arqComandos << "\tmov eax,[M+" << f1_end << "]" << endl;    // Carregar o conteúdo do T_endereço para o registrador EAX
                arqComandos << "\tmov ebx,[M+" << f2_end << "]" << endl;    // Carregar o conteúdo do T_endereço para o registrador EBX
                    
            }

            verificaTipos(f1_tipo, f2_tipo, t_operador);

            t_end = novoTemp(t_tipo);

            //Valores carregados nos devidos registradores, agora só realizar a operação
            if(t_operador == BARRA) {
                arqComandos << "\tdivss xmm0,xmm1" << endl;
                arqComandos << "\tmovss [M+" << t_end << "], xmm0" << endl; // Armazenando o resultado (DIVISÃO REAL) na memória 
            } else if(t_operador == AND) {
                //AND = eax * ebx
                arqComandos << "\timul ebx" << endl;
                arqComandos << "\tmov [M+" << t_end << "], eax" << endl; // Armazenando o resultado (AND) na memória

            } else if(t_operador == ASTERISCO) {
                if (f1_tipo == TIPO_INT && f2_tipo == TIPO_INT) {
                    arqComandos << "\timul ebx" << endl;
                    arqComandos << "\tmov [M+" << t_end << "], eax" << endl; // Armazenando o resultado (MULTIPLICAÇÃO) na memória
                } else {
                    arqComandos << "\tmulss xmm0,xmm1" << endl;
                    arqComandos << "\tmovss [M+" << t_end << "], xmm0" << endl;
                }
            } else if(t_operador == DIV || t_operador == MOD) { // Expansão de sinal para DIV e MOD
                
                arqComandos << "\tcdq" << endl; // Expansão de sinal
                arqComandos << "\tidiv ebx" << endl; // Divisão entre inteiros

                if(t_operador == DIV)
                    arqComandos << "\tmov [M+" << t_end << "], eax" << endl; // Quociente da divisão
                else 
                    arqComandos << "\tmov [M+" << t_end << "], edx" << endl; // Resto da divisão

            } 
                 
        }
    } //fim T()

/* 
ExpS     -> [+ | -] T1(1)(6) {(+(2) | -(3) | "||"(4)) T2(7)(5) }
(1) {exps_tipo = t1_tipo; exps_tam = t1_tam}
(2) {exps_operador = MAIS; ehNumero = true}
(3) {exps_operador = MENOS; ehNumero = true}
(4) {exps_operador = OR; exps_tipo = TIPO_BOOL}
(5) {verificaTipos(t1_tipo, t2_tipo, exps_operador)}
(6) {se ((exps_tipo != TIPO_INT && exps_tipo != TIPO_FLOAT) && ehNumero) entao ERRO}
(7) {se (exps_operador == MAIS || exps_operador = MENOS) entao exps_tipo = calculaExp_tipo(t1_tipo, t2_tipo)}
*/
    void ExpS(int &exps_tipo, int &exps_tam, int &exps_end) {
        
        int t1_tipo, t1_tam, t1_end;
        int t2_tipo, t2_tam, t2_end;
        int exps_operador;
        bool ehNumero = false, ehNeg = false;
        
        if (token_lido == MAIS) {        
            CasaToken(MAIS);
            ehNumero = true;//Flag de é int ou float
        } else if (token_lido == MENOS) {
            CasaToken(MENOS);
            ehNumero = true;//Flag de é int ou float
            ehNeg = true;//Flag de sinal negativo
        } 
        
        T(t1_tipo, t1_tam, t1_end);
        exps_tipo = t1_tipo;
        exps_tam = t1_tam;
        exps_end = t1_end;

        if((exps_tipo != TIPO_INT && exps_tipo != TIPO_FLOAT) && ehNumero)
            throw(7);

        
        if (ehNeg) {//Se número possui sinal negativo

            exps_end = novoTemp(exps_tipo);

            if (t1_tipo == TIPO_INT) {
                //Negação Inteira
                arqComandos << "\tmov eax,[M+" << exps_end << "]" << endl;//Carregamos valor inteiro da memória
                arqComandos << "\tneg eax" << endl;//Valor é negado
                arqComandos << "\tmov [M+" << exps_end << "],eax" << endl;//Retornamos o novo valor para a memória
            } else {
                //Simulação de negação aritmética real
                arqComandos << "\tmovss xmm0,[M+" << exps_end << "]" << endl;//Carregamos o valor real da memória
                arqComandos << "\tadd eax,-1" << endl;//eax recebe -1 para a negação
                arqComandos << "\tcdqe" << endl;//Expandimos sinal de eax para rax
                arqComandos << "\tcvtsi2ss xmm1,rax" << endl;//-1 convertido para float
                arqComandos << "\tmulss xmm0,xmm1" << endl;//Multiplicamos valor original por -1
                
                arqComandos << "\tmovss [M+" << exps_end << "],xmm0" << endl;//Retornamos o novo valor para a memória
            }
        }
        
        while(token_lido == MAIS || token_lido == MENOS || token_lido == OR) {
            
            if (token_lido == MAIS) {
                CasaToken(MAIS);
                exps_operador = MAIS;
            }else if (token_lido == MENOS) {
                CasaToken(MENOS);
                exps_operador = MENOS;
            }else {
                CasaToken(OR);
                exps_operador = OR;
                exps_tipo = TIPO_BOOL;
            }

            T(t2_tipo, t2_tam, t2_end);   

            if(exps_operador == MAIS || exps_operador == MENOS)
                exps_tipo = calculaExp_tipo(t1_tipo, t2_tipo);


            // Carregando corretamente os registradores de acordo com o exps_tipo
            if(exps_tipo == TIPO_INT) {                                
                
                arqComandos << "\tmov eax,[M+" << t1_end << "]" << endl;    // Carregar o conteúdo do exps_endereço para o registrador EAX
                arqComandos << "\tmov ebx,[M+" << t2_end << "]" << endl;    // Carregar o conteúdo do exps_endereço para o registrador EBX
                
            } else if(exps_tipo == TIPO_FLOAT) {

                if(t1_tipo == TIPO_INT) { // Se tipo for int fazemos a conversão
                    arqComandos << "\tmov eax,[M+" << t1_end << "]" << endl;    // Carregar o conteúdo do exps_endereço para o registrador EAX
                    arqComandos << "\tcdqe" << endl;    // Carregar o conteúdo do exps_endereço para o registrador RAX
                    arqComandos << "\tcvtsi2ss xmm0,rax" << endl;    // Converte conteúdo do registrador RAX para float e guarda no registrador XMM0

                } else { // senão carregamos o valor direto 
                    arqComandos << "\tmovss xmm0,[M+" << t1_end << "]" << endl;    // Carregar o conteúdo do exps_endereço para o registrador XMM0
                }

                if (t2_tipo == TIPO_INT) {//Se tipo for int fazemos a conversão
                    arqComandos << "\tmov eax,[M+" << t2_end << "]" << endl;    // Carregar o conteúdo do exps_endereço para o registrador RAX
                    arqComandos << "\tcdqe" << endl;    // Carregar o conteúdo do exps_endereço para o registrador RAX
                    arqComandos << "\tcvtsi2ss xmm1,rax" << endl;    // Converte conteúdo do registrador RAX para float e guarda no registrador XMM1
                } else { // senão carregamos o valor direto 
                    arqComandos << "\tmovss xmm1,[M+" << t2_end << "]" << endl;    // Carregar o conteúdo do exps_endereço para o registrador XMM1
                }            
                
            } else if(exps_tipo == TIPO_BOOL) {
                
                arqComandos << "\tmov eax,[M+" << t1_end << "]" << endl;    // Carregar o conteúdo do exps_endereço para o registrador EAX
                arqComandos << "\tmov ebx,[M+" << t2_end << "]" << endl;    // Carregar o conteúdo do exps_endereço para o registrador EBX
                    
            }

            verificaTipos(t1_tipo, t2_tipo, exps_operador);

            exps_end = novoTemp(exps_tipo);

            //Valores carregados nos devidos registradores, agora só realizar a operação
            if(exps_operador == MAIS) {
                if (exps_tipo == TIPO_FLOAT) {
                    arqComandos << "\taddss xmm0,xmm1" << endl;
                    arqComandos << "\tmovss [M+" << exps_end << "], xmm0" << endl; // Armazenando o resultado (SOMA REAL) na memória 
                } else {
                    arqComandos << "\tadd eax,ebx" << endl;
                    arqComandos << "\tmov [M+" << exps_end << "], eax" << endl; // Armazenando o resultado (SOMA INTEIRA) na memória
                }
            } else if(exps_operador == MENOS) {
                if (exps_tipo == TIPO_FLOAT) {
                    
                    arqComandos << "\tsubss xmm0,xmm1" << endl;
                    arqComandos << "\tmovss [M+" << exps_end << "], xmm0" << endl; // Armazenando o resultado (SUBTRAÇÃO REAL) na memória 
                } else {
                                        
                    arqComandos << "\tsub eax,ebx" << endl;
                    arqComandos << "\tmov [M+" << exps_end << "], eax" << endl; // Armazenando o resultado (SUBTRAÇÃO INTEIRA) na memória
                }
            } else if(exps_operador == OR) {
                
                //OR = eax + ebx - (eax * ebx)
                arqComandos << "\tmov ecx,eax" << endl;// Valor de eax é salvo no auxiliar ecx
                arqComandos << "\timul ebx" << endl;// eax <- eax * ebx
                arqComandos << "\tadd ebx,ecx" << endl;// ebx <- ebx + ecx(eax)
                arqComandos << "\tsub ebx,eax" << endl;// ebx <- ebx(eax + ebx) - eax(eax * ebx)             
                arqComandos << "\tmov [M+" << exps_end << "], ebx" << endl; // Armazenando o resultado (OU) na memória
            
            }

        }
    } //fim ExpS()


/* 
Exp      -> ExpS1(1)[(=(2) | !=(3) | <(4) | >(5) | <=(6) | >=(7)) ExpS2(8)]
(1) {exp_tipo = exps1_tipo; exp_tam = exps1_tam}
(2) {exp_operador = IGUAL; exp_tipo = TIPO_BOOL}
(3) {exp_operador = DIFERENTE; exp_tipo = TIPO_BOOL}
(4) {exp_operador = MENOR; exp_tipo = TIPO_BOOL}
(5) {exp_operador = MAIOR; exp_tipo = TIPO_BOOL}
(6) {exp_operador = MENOR_IGUAL; exp_tipo = TIPO_BOOL}
(7) {exp_operador = MAIOR_IGUAL; exp_tipo = TIPO_BOOL}
(8) {verificaTipos(exps1_tipo, exps2_tipo, exp_operador)}
*/
    void Exp(int &exp_tipo, int &exp_tam, int &exp_end) {
        int exps1_tipo, exps1_tam, exps1_end;
        int exps2_tipo, exps2_tam, exps2_end;
        int exp_operador;
        int tipo_comp;
        
        ExpS(exps1_tipo, exps1_tam, exps1_end);
        exp_tipo = exps1_tipo;
        exp_tam = exps1_tam;
        exp_end = exps1_end;

        if(token_lido == IGUAL || token_lido == DIFERENTE || token_lido == MENOR || token_lido == MAIOR || 
           token_lido == MENOR_IGUAL || token_lido == MAIOR_IGUAL) {
            if(token_lido == IGUAL) {
                CasaToken(IGUAL);
                exp_operador = IGUAL;
            }else if(token_lido == DIFERENTE) {
                  CasaToken(DIFERENTE);
                  exp_operador = DIFERENTE;
            }else if(token_lido == MENOR) {
                  CasaToken(MENOR);
                  exp_operador = MENOR;
            }else if(token_lido == MAIOR) {
                  CasaToken(MAIOR);
                  exp_operador = MAIOR;
            }else if(token_lido == MENOR_IGUAL) {
                  CasaToken(MENOR_IGUAL);
                  exp_operador = MENOR_IGUAL;
            }else {
                  CasaToken(MAIOR_IGUAL);
                  exp_operador = MAIOR_IGUAL;
            }                            
            
            ExpS(exps2_tipo, exps2_tam, exps2_end);
            verificaTipos(exps1_tipo, exps2_tipo, exp_operador);

            // Carregar registradores e converter, se necessário
            if(exps1_tipo == TIPO_INT && exps2_tipo == TIPO_INT) { // Se os dois tipo forem inteiros, carrega direto

                arqComandos << "\tmov eax,[M+" << exps1_end << "]" << endl;
                arqComandos << "\tmov ebx,[M+" << exps2_end << "]" << endl;
                tipo_comp = 1;

            } else if(exps1_tipo == TIPO_INT){   // Se exp1 for int, exp2 é float Converte exp1

                arqComandos << "\tmov eax,[M+" << exps1_end << "]" << endl; // Carrega exp1
                arqComandos << "\tcdqe" << endl;    // Estendeu para rax
                arqComandos << "\tcvtsi2ss xmm0,rax" << endl;   // Convertido para float
                arqComandos << "\tmovss xmm1,[M+" << exps2_end << "]" << endl; // Carrega exp2 direto
                tipo_comp = 2;
                
            } else if(exps2_tipo == TIPO_INT) {  // Se exp2 for int, exp1 é float Converte exp2

                arqComandos << "\tmovss xmm0,[M+" << exps1_end << "]" << endl; // Carrega exp1 direto
                arqComandos << "\tmov ebx,[M+" << exps2_end << "]" << endl; // Carrega exp2
                arqComandos << "\tcdqe" << endl;    // Estendeu para rbx
                arqComandos << "\tcvtsi2ss xmm1,rbx" << endl;   // Convertido para float
                tipo_comp = 2;        
                
            } else if(exps1_tipo == TIPO_FLOAT && exps2_tipo == TIPO_FLOAT){ // Se os dois tipo forem float, carrega direto

                arqComandos << "\tmovss xmm0,[M+" << exps1_end << "]" << endl; // Carrega exp1 direto
                arqComandos << "\tmovss xmm1,[M+" << exps2_end << "]" << endl; // Carrega exp2 direto
                tipo_comp = 2;
                
            } else if (exps1_tipo == TIPO_CHAR && exps2_tipo == TIPO_CHAR) { // Char

                arqComandos << "\tmov al,[M+" << exps1_end << "]" << endl; // Carrega exp1 num registrador 8 bits
                arqComandos << "\tmov bl,[M+" << exps2_end << "]" << endl; // Carrega exp1 num registrador 8 bits
                tipo_comp = 3;
                
            } else { // String

                arqComandos << "\tmov al,[M+" << exps1_end << "]" << endl;  //Carregamos o primeiro caractere de exps1
                arqComandos << "\tmov bl,[M+" << exps2_end << "]" << endl;  //Carregamos o primeiro caractere de exps2
                tipo_comp = 4;

            }

            string rotInicio;
            string rotVerdadeiro;
            string rotFim;

            switch (tipo_comp) {    // Codigo gerado de acordo com os tipos resultantes
                
                case 1: // Comparação de inteiros
                    rotVerdadeiro = novoRot();
                    rotFim = novoRot();                  

                    arqComandos << "\tcmp eax,ebx" << endl;
                    
                    if (exp_operador == IGUAL) {
                        arqComandos << "\tje " << rotVerdadeiro << endl;
                    } else if (exp_operador == DIFERENTE) {
                        arqComandos << "\tjne " << rotVerdadeiro << endl;
                    } else if (exp_operador == MENOR) {
                        arqComandos << "\tjl " << rotVerdadeiro << endl;
                    } else if (exp_operador == MAIOR) {
                        arqComandos << "\tjg " << rotVerdadeiro << endl;
                    } else if (exp_operador == MENOR_IGUAL) {
                        arqComandos << "\tjle " << rotVerdadeiro << endl;
                    } else if (exp_operador == MAIOR_IGUAL) {
                        arqComandos << "\tjge " << rotVerdadeiro << endl;
                    }

                    arqComandos << "\tmov ecx,0" << endl;   // Boolean recebe falso(0) se não atender nenhum dos testes
                    arqComandos << "\tjmp " << rotFim << endl;
                    arqComandos << rotVerdadeiro << ":" << endl;
                    arqComandos << "\tmov ecx,1" << endl;   // Boolean recebe verdadeiro(1) se atender a algum dos testes
                    arqComandos << rotFim << ":" <<endl;                    

                    break;
                
                case 2: // Comparação de reais
                    rotVerdadeiro = novoRot();
                    rotFim = novoRot();
                     
                    arqComandos << "\tcomiss xmm0,xmm1" << endl;
                    
                    if (exp_operador == IGUAL) {
                        arqComandos << "\tje " << rotVerdadeiro << endl;
                    } else if (exp_operador == DIFERENTE) {
                        arqComandos << "\tjne " << rotVerdadeiro << endl;
                    } else if (exp_operador == MENOR) {
                        arqComandos << "\tjb " << rotVerdadeiro << endl;
                    } else if (exp_operador == MAIOR) {
                        arqComandos << "\tja " << rotVerdadeiro << endl;
                    } else if (exp_operador == MENOR_IGUAL) {
                        arqComandos << "\tjbe " << rotVerdadeiro << endl;
                    } else if (exp_operador == MAIOR_IGUAL) {
                        arqComandos << "\tjae " << rotVerdadeiro << endl;
                    }

                    arqComandos << "\tmov ecx,0" << endl;   // Boolean recebe falso(0) se não atender nenhum dos testes
                    arqComandos << "\tjmp " << rotFim << endl;
                    arqComandos << rotVerdadeiro << ":" << endl;
                    arqComandos << "\tmov ecx,1" << endl;   // Boolean recebe verdadeiro(1) se atender a algum dos testes
                    arqComandos << rotFim << ":" << endl;

                    break;

                case 3: // Comparação de caracteres
                    rotVerdadeiro = novoRot();
                    rotFim = novoRot();

                    arqComandos << "\tcmp al,bl" << endl;
                    
                    if (exp_operador == IGUAL) {
                        arqComandos << "\tje " << rotVerdadeiro << endl;
                    } else if (exp_operador == DIFERENTE) {
                        arqComandos << "\tjne " << rotVerdadeiro << endl;
                    } else if (exp_operador == MENOR) {
                        arqComandos << "\tjl " << rotVerdadeiro << endl;
                    } else if (exp_operador == MAIOR) {
                        arqComandos << "\tjg " << rotVerdadeiro << endl;
                    } else if (exp_operador == MENOR_IGUAL) {
                        arqComandos << "\tjle " << rotVerdadeiro << endl;
                    } else if (exp_operador == MAIOR_IGUAL) {
                        arqComandos << "\tjge " << rotVerdadeiro << endl;
                    }

                    arqComandos << "\tmov ecx,0" << endl;   // Boolean recebe falso(0) se não atender nenhum dos testes
                    arqComandos << "\tjmp " << rotFim << endl;
                    arqComandos << rotVerdadeiro << ":" << endl;
                    arqComandos << "\tmov ecx,1" << endl;   // Boolean recebe verdadeiro(1) se atender a algum dos testes
                    arqComandos << rotFim << ":" << endl;
                    
                    break;
                
                case 4: // Comparação de strings(realizada caractere a caractere)
                    
                    string rotInicio_Cmp, rotIguais, rotFim_Cmp;
                    
                    rotInicio_Cmp = novoRot();
                    rotIguais = novoRot();
                    rotFim_Cmp = novoRot();
                                        
                    arqComandos << "\tmov rdx,0" << endl; // Contador da posição da string
                    arqComandos << "\tmov ecx,1" << endl; // Boolean que contém o estado da comparação recebe true, inicialmente

                    arqComandos << rotInicio_Cmp << ":" << endl; // Início loop comparação
                    arqComandos << "\tcmp al,bl" << endl;    // Compara caracteres correntes        
                    arqComandos << "\tje " << rotIguais << endl;// Se iguais, vá para rotIguais
                    arqComandos << "\tmov ecx,0" << endl; // Boolean recebe false, pois falhou na igualdade
                    arqComandos << "\tjmp " << rotFim_Cmp << endl;// Se diferentes, termina comparação
                    
                    arqComandos << rotIguais << ":" << endl;                    
                    arqComandos << "\tcmp al, 0h" << endl; // Verificando se não é fim da string 1 ('\0')
                    arqComandos << "\tje " << rotFim_Cmp << endl; // Se fim da string1, termina o loop
                    
                    arqComandos << "\tcmp bl, 0h" << endl; // Verificando se não é fim da string 2 ('\0')
                    arqComandos << "\tje " << rotFim_Cmp << endl; // Se fim da string2, termina o loop
                    
                    arqComandos << "\tadd rdx, 1" << endl; // Um caractere ocupa 8bits, andamos uma posição
                    
                    arqComandos << "\tmov rax, M+" << exps1_end << endl; // Carregando a posição inicial da string 1
                    arqComandos << "\tadd rax,rdx" << endl; // Incrementar a posição no index inicial da string 1
                    arqComandos << "\tmov al,[rax]" << endl; // Carrega a próxima posição da string 1 para comparação                   
                    
                    arqComandos << "\tmov rbx, M+" << exps2_end << endl; // Carregando a posição inicial da string 2
                    arqComandos << "\tadd rbx,rdx" << endl; //  Incrementar a posição no index inicial da string 2 
                    arqComandos << "\tmov bl,[rbx]" << endl; // Carrega a próxima posição da string 2 para comparação 

                    arqComandos << "\tjmp " << rotInicio_Cmp << endl;//Volta ao início para continuar a comparação
                    arqComandos << rotFim_Cmp << ":" << endl;
                    
                    break;
                    
            }

            exp_tipo = TIPO_BOOL;
            exp_end = novoTemp(exp_tipo);
            arqComandos << "\tmov [M+" << exp_end << "],ecx" << endl; // Move o resultado da comparação (0/1) para a memória

        }
    } //fim Exp()


/*
CmdNull  -> ;
*/
    void CmdNull() {
        arqComandos << ";Comando NULL" << endl;
        CasaToken(PONTO_VIRGULA);
    } //fim CmdNull()


/* 
CmdAtr   -> id(1)(2)[ "["Exp1(3)"]" ] "<-" Exp2(4);
(1) {se (id_tipo == TIPO_VAZIO) entao ERRO senao cmdatr_tipo = id_tipo}
(2) {se (id_classe == CLASSE_CONST) entao ERRO}
(3) {se (exp1_tipo != TIPO_INT || id_tipo != TIPO_STR) entao ERRO senao ehVetor = true}
(4) {se (ehVetor && exp2_tipo != TIPO_CHAR) entao ERRO senao se !ehVetor entao verificaCompatibilidadeAtr_Dec(id_tipo, exp2_tipo)}
*/
    void CmdAtr() {
        arqComandos << ";Comando ATRIBUIÇÃO" << endl;

        int id_tipo, id_classe, id_end;
        int exp1_tipo, exp1_tam, exp1_end;
        int exp2_tipo, exp2_tam, exp2_end;
        int cmdatr_tipo;
        bool ehVetor = false;

        id_tipo = Hash.resgataTipo(reg_lex.lexema);
        id_classe = Hash.resgataClasse(reg_lex.lexema);
        id_end = Hash.resgataEndereco(reg_lex.lexema);

        //Verificação de Unicidade e Classe de identificadores
        if(id_tipo == TIPO_VAZIO)
            throw(4);
        else if(id_classe == CLASSE_CONST)// Não podemos atribuir valor a uma constante
            throw(6);
        else
            cmdatr_tipo = id_tipo;

        CasaToken(ID);

        if (token_lido == ABRE_COLCHETE) {
            CasaToken(ABRE_COLCHETE);

            Exp(exp1_tipo, exp1_tam, exp1_end);
            contadorTemp = 0; // Contador de temporários é reinicializado depois de um comando chamar uma expressão

            if(id_tipo != TIPO_STR || exp1_tipo != TIPO_INT) 
                throw(7);    
            else 
                ehVetor = true;//Flag para indicação de atribuição por elementos de string(caracteres)

            CasaToken(FECHA_COLCHETE);
        }

        CasaToken(MENOR_MENOS);

        Exp(exp2_tipo, exp2_tam, exp2_end);
        contadorTemp = 0; // Contador de temporários é reinicializado depois de um comando chamar uma expressão


        if(ehVetor && exp2_tipo != TIPO_CHAR) 
            throw(7);
        else if (!ehVetor)//Se não for elemento de string, verifica compatibilidade de tipos       
            verificaCompatibilidadeAtr_Dec(id_tipo, exp2_tipo);


        if(exp2_tipo == TIPO_INT) {//Tipo retornado de exp
            
            if(id_tipo == TIPO_INT) { // Se id_tipo for inteiro, carrega direto

                arqComandos << "\tmov eax,[M+" << exp2_end << "]" << endl; // Carrega inteiro da memória
                arqComandos << "\tmov [M+" << id_end << "],eax" << endl; // Substituição do valor de id  

            } else { // Se id_tipo for float, carregamos o inteiro retornado por exp e convertemos para float

                arqComandos << "\tmov eax,[M+" << exp2_end << "]" << endl; // Carrega inteiro da memória
                arqComandos << "\tcdqe" << endl; // Expansão de sinal
                arqComandos << "\tcvtsi2ss xmm0,rax" << endl; // Convertemos inteiro para float
                arqComandos << "\tmovss [M+" << id_end << "],xmm0" << endl; // Armazenamos o float no id_end
                
            }     

        } else if(exp2_tipo == TIPO_FLOAT) {//Tipo retornado de exp

                //Se exp2_tipo for float, id_tipo também é, então carregamos direto
                arqComandos << "\tmovss xmm0,[M+" << exp2_end << "]" << endl; // Carrega float da memória
                arqComandos << "\tmovss [M+" << id_end << "],xmm0" << endl; // Substituição do valor de id   

        } else if(exp2_tipo == TIPO_CHAR) {//Tipo retornado de exp
                
                if(ehVetor) {//Se é vetor

                    arqComandos << "\tmov ecx,[M+" << exp1_end << "]" << endl; // Carrega da memória a posição do vetor
                    arqComandos << "\tmov eax,1" << endl; // Cada posição da string ocupa 8bits, que é o tamanho de um caractere 
                    arqComandos << "\timul ecx" << endl; // Multiplicamos 1 byte pelo valor carregado em eax 
                    arqComandos << "\tmov ecx,[M+" << id_end << "]" << endl; // Carregamos o endereço inicial da string
                    arqComandos << "\tadd eax,ecx" << endl; // Adicionamos o deslocamento da posição desejada no endereço inicial
                    arqComandos << "\tmov bl,[M+" << exp2_end << "]" << endl; // Carrega o caractere da memória
                    arqComandos << "\tmov [eax],bl" << endl; // Faz a substituição do caractere desejado

                } else { // Se não é um vetor

                    arqComandos << "\tmov al,[M+" << exp2_end << "]" << endl; // Carrega o caractere da memória
                    arqComandos << "\tmov [M+" << id_end << "],al" << endl;   // Substitui o caractere de id   
                           
                }

        } else if(exp2_tipo == TIPO_STR) {//Tipo retornado de exp
            
            string rotInicio_Atr, rotFim_Atr;
                    
            rotInicio_Atr = novoRot();
            rotFim_Atr = novoRot();
                                
            arqComandos << "\tmov rdx,0" << endl; // Contador da posição da string começando na primeira posição
            
            arqComandos << rotInicio_Atr << ":" << endl; // Início loop atribuição string
            arqComandos << "\tmov rax, M+" << id_end << endl; // Carregando a posição inicial do id string (destino)
            arqComandos << "\tadd rax,rdx" << endl; // Incrementar a posição no index inicial do id string (destino)
            arqComandos << "\tmov rbx, M+" << exp2_end << endl; // Carregando a posição inicial da string origem
            arqComandos << "\tadd rbx,rdx" << endl; // Incrementar a posição no index inicial da string origem
            arqComandos << "\tmov bl,[rbx]" << endl; // Carrega o caractere da string origem na posição desejada

            arqComandos << "\tmov [rax],bl" << endl; // Move o caractere para o endereço de memória na posição desejada da string (destino)
            arqComandos << "\tcmp bl, 0h" << endl; // Verificando se não é fim da string('\0')
            arqComandos << "\tje " << rotFim_Atr << endl; // Se fim da string, termina o loop            
            
            arqComandos << "\tadd rdx, 1" << endl; // Um caractere ocupa 8bits, andamos uma posição

            arqComandos << "\tjmp " << rotInicio_Atr << endl; // Volta ao início para continuar a atribuição
            arqComandos << rotFim_Atr << ":" << endl;   // Fim loop atribuição string        
            
        }

        
        CasaToken(PONTO_VIRGULA);  
        arqComandos << ";Fim Comando ATRIBUIÇÃO" << endl;      
    } //fim CmdAtr()


/* 
CmdWrite -> ("write" | "writeln") "(" {Exp(1) [,]}+ ")";
(1) {se (exp_tipo == TIPO_VAZIO) entao ERRO}
*/
    void CmdWrite() {
        arqComandos << ";Comando WRITE" << endl;

        int exp_tipo, exp_tam, exp_end;
        int buffer_end;
        bool ln = false;
        
        if (token_lido == WRITE) {
            CasaToken(WRITE);
            ln = false;
        } else {
            CasaToken(WRITELN);
            ln = true;//Flag para verificar se é necessário um \n ao final da impressão
        }

        CasaToken(ABRE_PARENTESES);

        do {
            Exp(exp_tipo, exp_tam, exp_end);
            contadorTemp = 0; // Contador de temporários é reinicializado depois de um comando chamar uma expressão

            if (exp_tipo == TIPO_VAZIO) // Verificação de Unicidade(caso exp retorne um identificador)
                throw(4);

            buffer_end = novoTemp(TIPO_STR);

            if(exp_tipo == TIPO_INT) {

                string rot0, rot1, rot2;
                rot0 = novoRot();
                rot1 = novoRot();
                rot2 = novoRot();
                
                arqComandos << "\tmov eax, [M+" << exp_end << "]\t\t;inteiro a ser convertido" << endl;
                arqComandos << "\tmov rsi, M+" << buffer_end << "\t\t;end. string ou temp." << endl;
                arqComandos << "\tmov rcx, 0 \t\t;contador pilha" << endl;
                arqComandos << "\tmov rdi, 0 \t\t;tam. string convertido" << endl;

                arqComandos << "\tcmp eax, 0 \t\t;verifica sinal" << endl;
                arqComandos << "\tjge " << rot0 << "\t\t;salta se número positivo" << endl;
                arqComandos << "\tmov bl, \'-\' \t\t;senão, escreve sinal –" << endl;
                arqComandos << "\tmov [rsi], bl" << endl;
                arqComandos << "\tadd rsi, 1 \t\t;incrementa índice" << endl;
                arqComandos << "\tadd rdi, 1 \t\t;incrementa tamanho" << endl;

                arqComandos << "\tneg eax \t\t;toma módulo do número" << endl;
                
                arqComandos << rot0 << ":" << endl;
                arqComandos << "\tmov ebx, 10 \t\t;divisor" << endl;
                
                arqComandos << rot1 << ":" << endl;
                arqComandos << "\tadd rcx, 1 \t\t;incrementa contador" << endl;

                arqComandos << "\tcdq \t\t;estende edx:eax p/ div." << endl;
                arqComandos << "\tidiv ebx \t\t;divide edx;eax por ebx" << endl;
                arqComandos << "\tpush dx \t\t;empilha valor do resto" << endl;
                arqComandos << "\tcmp eax, 0 \t\t;verifica se quoc. é 0" << endl;
                arqComandos << "\tjne " << rot1 << "\t\t;se não é 0, continua" << endl;
                arqComandos << "\tadd rdi,rcx \t\t;atualiza tam. string" << endl;
                arqComandos << ";agora, desemp. os valores e escreve o string" << endl;
                
                arqComandos << rot2 << ":" << endl;
                arqComandos << "\tpop dx \t\t;desempilha valor" << endl;
                arqComandos << "\tadd dl, \'0\' \t\t;transforma em caractere" << endl;
                arqComandos << "\tmov [rsi], dl \t\t;escreve caractere" << endl;
                arqComandos << "\tadd rsi, 1 \t\t;incrementa base" << endl;
                arqComandos << "\tsub rcx, 1 \t\t;decrementa contador" << endl;
                arqComandos << "\tcmp rcx, 0 \t\t;verifica pilha vazia" << endl;
                arqComandos << "\tjne " << rot2 << "\t\t;se não pilha vazia, loop" << endl;
                arqComandos << "\tmov rdx, rdi" << endl;
                arqComandos << "\t\t;executa interrupção de saída" << endl;

            } else if(exp_tipo == TIPO_FLOAT) {

                string rot0, rot1, rot2, rot3, rot4;

                rot0 = novoRot();
                rot1 = novoRot();
                rot2 = novoRot();
                rot3 = novoRot();
                rot4 = novoRot();
                
                arqComandos << "\tmovss xmm0, [M+" << exp_end << "] \t\t;real a ser convertido" << endl;
                arqComandos << "\tmov rsi, M+" << buffer_end << "\t\t;end. temporário" << endl;
                arqComandos << "\tmov rcx, 0 \t\t;contador pilha" << endl;
                arqComandos << "\tmov rdi, 6 \t\t;precisao 6 casas compart "<< endl;
                arqComandos << "\tmov rbx, 10 \t\t;divisor" << endl;
                arqComandos << "\tcvtsi2ss xmm2, rbx \t\t;divisor real" << endl;
                arqComandos << "\tsubss xmm1, xmm1 \t\t;zera registrador" << endl;
                arqComandos << "\tcomiss xmm0, xmm1 \t\t;verifica sinal" << endl;
                arqComandos << "\tjae " << rot0 << "\t\t;salta se número positivo" << endl;
                arqComandos << "\tmov dl, \'-\' \t\t;senão, escreve sinal –" << endl;
                arqComandos << "\tmov [rsi], dl" << endl;
                arqComandos << "\tmov rdx, -1 \t\t;Carrega -1 em RDX" << endl;
                arqComandos << "\tcvtsi2ss xmm1, rdx \t\t;Converte para real" << endl;
                arqComandos << "\tmulss xmm0, xmm1 \t\t;Toma módulo" << endl;
                arqComandos << "\tadd rsi, 1 \t\t;incrementa índice" << endl;
                
                arqComandos << rot0 << ":" << endl;
                arqComandos << "\troundss xmm1, xmm0, 0b0011 \t\t;parte inteira xmm1" << endl;
                arqComandos << "\tsubss xmm0, xmm1 \t\t;parte frac xmm0" << endl;
                arqComandos << "\tcvtss2si rax, xmm1 \t\t;convertido para int" << endl;
                arqComandos << "\t\t;converte parte inteira que está em rax" << endl;
                
                arqComandos << rot1 << ":" << endl;
                arqComandos << "\tadd rcx, 1 \t\t;incrementa contador"<< endl;
                arqComandos << "\tcdq \t\t;estende edx:eax p/ div." << endl;
                arqComandos << "\tidiv ebx \t\t;divide edx;eax por ebx" << endl;
                arqComandos << "\tpush dx \t\t;empilha valor do resto" << endl;
                arqComandos << "\tcmp eax, 0 \t\t;verifica se quoc. é 0" << endl;
                arqComandos << "\tjne " << rot1 << "\t\t;se não é 0, continua" << endl;
                arqComandos << "\tsub rdi, rcx \t\t;decrementa precisao" << endl;
                arqComandos << "\t\t;agora, desemp valores e escreve parte int" << endl;
                
                arqComandos << rot2 << ":" << endl;
                arqComandos << "\tpop dx \t\t;desempilha valor" << endl;
                arqComandos << "\tadd dl, \'0\' \t\t;transforma em caractere" << endl;
                arqComandos << "\tmov [rsi], dl \t\t;escreve caractere" << endl;
                arqComandos << "\tadd rsi, 1 \t\t;incrementa base" << endl;
                arqComandos << "\tsub rcx, 1 \t\t;decrementa contador" << endl;
                arqComandos << "\tcmp rcx, 0 \t\t;verifica pilha vazia" << endl;
                arqComandos << "\tjne " << rot2 << "\t\t;se não pilha vazia, loop" << endl;
                arqComandos << "\tmov dl, \'.\' \t\t;escreve ponto decimal" << endl;
                arqComandos << "\tmov [rsi], dl" << endl;
                arqComandos << "\tadd rsi, 1 \t\t;incrementa base" << endl;
                arqComandos << "\t\t;converte parte fracionaria que está em xmm0" << endl;
                
                arqComandos << rot3 << ":" << endl;
                arqComandos << "\tcmp rdi, 0 \t\t;verifica precisao" << endl;
                arqComandos << "\tjle " << rot4 << "\t\t;terminou precisao ?" << endl;
                arqComandos << "\tmulss xmm0,xmm2 \t\t;desloca para esquerda" << endl;
                arqComandos << "\troundss xmm1,xmm0,0b0011 \t\t;parte inteira xmm1" << endl;
                arqComandos << "\tsubss xmm0,xmm1 \t\t;atualiza xmm0" << endl;
                arqComandos << "\tcvtss2si rdx, xmm1 \t\t;convertido para int" << endl;
                arqComandos << "\tadd dl, \'0\' \t\t;transforma em caractere" << endl;
                arqComandos << "\tmov [rsi], dl \t\t;escreve caractere" << endl;
                arqComandos << "\tadd rsi, 1 \t\t;incrementa base" << endl;
                arqComandos << "\tsub rdi, 1 \t\t;decrementa precisao" << endl;
                arqComandos << "\tjmp " << rot3 << endl;
                arqComandos << "\t\t; impressão" << endl;
                
                arqComandos << rot4 << ":" << endl;
                arqComandos << "\tmov dl, 0 \t\t;fim string, opcional" << endl;
                arqComandos << "\tmov [rsi], dl \t\t;escreve caractere" << endl;
                arqComandos << "\tmov rdx, rsi \t\t;calc tam str convertido" << endl;
                arqComandos << "\tmov rbx, M+" << buffer_end << endl;
                arqComandos << "\tsub rdx, rbx \t\t;tam=rsi-M-buffer.end "<< endl;
                arqComandos << "\tmov rsi,M+" << buffer_end << "\t\t; endereço do buffer" << endl;
                arqComandos << "\t;executa interrupção de saída. rsi e rdx já foram calculados então usar só as instruções para a chamada do kernel." << endl;
                
            } else if(exp_tipo == TIPO_CHAR) {  
                
                arqComandos << "\tmov rsi, M+" << buffer_end << "\t\t;end. string ou temp." << endl;
                arqComandos << "\tmov rax, M+" << exp_end << endl;//Movemos o endereço de exp para rax           
                arqComandos << "\tmov bl, [rax]\t\t;carregando o caractere" << endl;
                arqComandos << "\tmov [rsi], bl\t\t;armazenando o caractere em end. buffer" << endl;
                arqComandos << "\tmov rdx, 1" << endl;//Como é caractere, tamanho do buffer é 1

            } else if(exp_tipo == TIPO_STR) { 
                
                string rotInicio_Read, rotFim_Read;

                rotInicio_Read = novoRot();
                rotFim_Read = novoRot();

                arqComandos << "\tmov rdx, 0" << endl;//Contador do tamanho do buffer inicializado com 0
                arqComandos << "\tmov rsi, M+" << buffer_end << "\t\t;end. string ou temp." << endl;
                arqComandos << "\tmov rax, M+" << exp_end << endl;//Movemos o endereço de exp para rax 

                arqComandos << rotInicio_Read << ":" << endl;           
                arqComandos << "\tmov bl, [rax]\t\t;carregando o caractere" << endl;              
                arqComandos << "\tcmp bl, 0h\t\t;verificando se não é o fim do string" << endl;
                arqComandos << "\tje " << rotFim_Read << "\t\t;se fim de string pula para o fim" << endl;
                arqComandos << "\tmov [rsi], bl\t\t;armazenando o caractere em end. buffer" << endl;
                arqComandos << "\tadd rdx, 1\t\t;incrementado o contador do buffer" << endl;
                arqComandos << "\tadd rsi, 1\t\t;incrementando o índice de posição de inserção do end. buffer" << endl;
                arqComandos << "\tadd rax, 1\t\t;incrementando o índice de posição de leitura do end. exp" << endl;

                arqComandos << "\tjmp " << rotInicio_Read << endl;
                arqComandos << rotFim_Read << ":" << endl;

            }            

            //Impressão na tela
            if(exp_tipo == TIPO_FLOAT) {//Se tipo float, rsi e rdx ja possuem os valores corretos, entao so chamamos o kernel direto

                arqComandos << "\tmov rax, 1 ;chamada para saída" << endl;
                arqComandos << "\tmov rdi, 1 ;saída para tela" << endl;
                arqComandos << "\tsyscall" << endl;                              

            } else {//Tamanho do buffer, rdx, já calculado anteriormente nos respectivos casos de tipo
                
                arqComandos << "\tmov rsi, M+" << buffer_end << "\t\t;ou buffer.end" << endl;
                arqComandos << "\tmov rax, 1 ;chamada para saída" << endl;
                arqComandos << "\tmov rdi, 1 ;saída para tela" << endl;
                arqComandos << "\tsyscall" << endl;

            }

            contadorTemp = 0; // Reiniciamos o contador de temporários para apagar o buffer caso seja necessário escrever outra exp

            if(token_lido == VIRGULA)
                CasaToken(VIRGULA);
        } while(token_lido == EXCLAMACAO || token_lido == ABRE_PARENTESES || token_lido == INT || token_lido == FLOAT || 
                token_lido == VALOR || token_lido == ID); 

        CasaToken(FECHA_PARENTESES);
        CasaToken(PONTO_VIRGULA);

        if (ln) {//Código de inserção de quebra de linha ao final da escrita na tela

            int ln_end = novoTemp(TIPO_CHAR);

            arqComandos << "\tmov rbx, M+" << ln_end << endl;//Carregamos o primeiro índice de ln
            arqComandos << "\tmov cl, 0Ah" << endl;//Carregamos \n em cl
            arqComandos << "\tmov [rbx], cl" << endl;//Inserimos \n na memória
            arqComandos << "\tmov rsi, rbx" << endl;//rsi recebe o endereço onde está o \n
            arqComandos << "\tmov rdx, 1" << endl;//Tamanho do buffer igual 1
            arqComandos << "\tmov rax, 1 ;chamada para saída" << endl;
            arqComandos << "\tmov rdi, 1 ;saída para tela" << endl;
            arqComandos << "\tsyscall" << endl;
            contadorTemp = 0; // Reiniciamos o contador de temporários para apagar o buffer caso seja necessário escrever outra exp

        }        

        arqComandos << ";Fim Comando WRITE" << endl;
    } //fim CmdWrite()    


/* 
CmdRead  -> "readln" "(" id(1)(2) ")";
(1) {se (id_tipo == TIPO_VAZIO) entao ERRO} 
(2) {se (id_classe == CLASSE_CONST) entao ERRO}
*/
    void CmdRead() {
        arqComandos << ";Comando READ" << endl;

        int id_tipo, id_classe, id_end;
        int buffer_end;

        CasaToken(READLN);
        CasaToken(ABRE_PARENTESES);
        
        id_tipo = Hash.resgataTipo(reg_lex.lexema);
        id_classe = Hash.resgataClasse(reg_lex.lexema);
        id_end = Hash.resgataEndereco(reg_lex.lexema);

        // Verificação de Unicidade e Classe de identificadores
        if(id_tipo == TIPO_VAZIO)
            throw(4);
        else if(id_classe == CLASSE_CONST)//Não podemos atribuir um valor para uma constante
            throw(6);
       
        
        CasaToken(ID);        

        CasaToken(FECHA_PARENTESES);
        CasaToken(PONTO_VIRGULA);

        buffer_end = novoTemp(TIPO_STR);

        string rotInicio, rotPositivo, rotSinal, rotFim;
        string rotLoop, rotPonto;
        string rotInicio_Read, rotFim_Read;

        //Libera terminal para escrita
        arqComandos << "\tmov rsi, M+" << buffer_end << endl;
        arqComandos << "\tmov rdx, 32 ; tamanho do buffer" << endl; //100h
        arqComandos << "\tmov rax, 0 ; chamada para leitura" << endl;
        arqComandos << "\tmov rdi, 0 ; leitura do teclado" << endl;
        arqComandos << "\tsyscall" << endl;

        if (id_tipo == TIPO_STR) {

            rotInicio_Read = novoRot();
            rotFim_Read = novoRot();

            arqComandos << "\tmov rcx, M+" << id_end << endl;//Move para rcx o endereço do primeiro índice de id
            
            arqComandos << rotInicio_Read << ":" << endl;            
            arqComandos << "\tmov bl, [rsi] \t\t;carrega caractere" << endl;
            arqComandos << "\tcmp bl, 0Ah\t\t;compara caractere lido com \\n" << endl;
            arqComandos << "\tje " << rotFim_Read << endl;//Se for igual a \n, vá para o fim
            arqComandos << "\tmov [rcx],bl \t\t;move caractere para id_end" << endl;
            arqComandos << "\tadd rsi, 1\t\t;incrementamos a posicao de leitura do buffer" << endl;
            arqComandos << "\tadd rcx, 1\t\t;incrementamos a posicao de insercao do id" << endl;
            arqComandos << "\tjmp " << rotInicio_Read << endl;//Retorna para o inicio

            arqComandos << rotFim_Read << ":" << endl;
            arqComandos << "\tmov bl, 0h" << endl;//Carregamos o \0 em bl
            arqComandos << "\tmov [rcx], bl \t\t;adiciona o caractere \\0 no final da string" << endl;

        } else if (id_tipo == TIPO_CHAR) {

            arqComandos << "\tmov rsi, M+" << buffer_end << "\t\t;end. buffer" << endl;//Carregamos o endereço do primeiro índice do buffer
            arqComandos << "\tmov bl, [rsi] \t\t;carrega caractere" << endl;
            arqComandos << "\tmov [M+" << id_end << "],bl \t\t;move caractere para id_end" << endl;

        } else if (id_tipo == TIPO_INT) {

            rotInicio = novoRot();
            rotPositivo = novoRot();
            rotSinal = novoRot();
            rotFim = novoRot();

            arqComandos << "\tmov eax, 0 \t\t;acumulador" << endl;
            arqComandos << "\tmov ebx, 0 \t\t;caractere" << endl;
            arqComandos << "\tmov ecx, 10 \t\t;base 10" << endl;
            arqComandos << "\tmov dx, 1 \t\t;sinal" << endl;
            arqComandos << "\tmov rsi, M+" << buffer_end << "\t\t;end. buffer" << endl;
            arqComandos << "\tmov bl, [rsi] \t\t;carrega caractere" << endl;
            arqComandos << "\tcmp bl, \'-\' \t\t;sinal - ?" << endl;
            arqComandos << "\tjne "<< rotInicio << "\t\t;se dif -, salta" << endl;
            arqComandos << "\tmov dx, -1 \t\t;senão, armazena -" << endl;
            arqComandos << "\tadd rsi, 1 \t\t;inc. ponteiro string" << endl;
            arqComandos << "\tmov bl, [rsi] \t\t;carrega caractere" << endl;
            
            arqComandos << rotInicio << ":" << endl;
            arqComandos << "\tpush dx \t\t;empilha sinal" << endl;
            arqComandos << "\tmov edx, 0 \t\t;reg. multiplicação" << endl;
            
            arqComandos << rotPositivo << ":" << endl;
            arqComandos << "\tcmp bl, 0Ah \t\t;verifica fim string" << endl;
            arqComandos << "\tje " << rotSinal << "\t\t;salta se fim string" << endl;
            arqComandos << "\timul ecx \t\t;mult. eax por 10" << endl;
            arqComandos << "\tsub bl, \'0\' \t\t;converte caractere" << endl;
            arqComandos << "\tadd eax, ebx \t\t;soma valor caractere" << endl;
            arqComandos << "\tadd rsi, 1 \t\t;incrementa base" << endl;
            arqComandos << "\tmov bl, [rsi] \t\t;carrega caractere" << endl;
            arqComandos << "\tjmp " << rotPositivo << "\t\t;loop" << endl;
            
            arqComandos << rotSinal << ":" << endl;
            arqComandos << "\tpop cx \t\t; desempilha sinal" << endl;
            arqComandos << "\tcmp cx, 0" << endl;
            arqComandos << "\tjg " << rotFim << endl;
            arqComandos << "\tneg eax \t\t;mult. sinal" << endl;            
            
            arqComandos << rotFim << ":" << endl;
            arqComandos << "\tmov [M+" << id_end << "],eax\t\t;move inteiro para id_end" << endl;

        } else if (id_tipo == TIPO_FLOAT) {
            
            rotInicio = novoRot();
            rotLoop = novoRot();
            rotFim = novoRot();
            rotPonto = novoRot();

            arqComandos << "\tmov rax, 0 \t\t;acumul. parte int." << endl;
            arqComandos << "\tsubss xmm0,xmm0 \t\t;acumul. parte frac." << endl;
            arqComandos << "\tmov rbx, 0 \t\t;caractere" << endl;
            arqComandos << "\tmov rcx, 10 \t\t;base 10" << endl;
            arqComandos << "\tcvtsi2ss xmm3,rcx \t\t;base 10" << endl;
            arqComandos << "\tmovss xmm2,xmm3 \t\t;potência de 10" << endl;
            arqComandos << "\tmov rdx, 1 \t\t;sinal" << endl;
            arqComandos << "\tmov rsi, M+" << buffer_end << "\t\t;end. buffer" << endl;
            arqComandos << "\tmov bl, [rsi] \t\t;carrega caractere" << endl;
            arqComandos << "\tcmp bl, \'-\' \t\t;sinal - ?" << endl;
            arqComandos << "\tjne " << rotInicio << "\t\t;se dif -, salta" << endl;
            arqComandos << "\tmov rdx, -1 \t\t;senão, armazena -" << endl;
            arqComandos << "\tadd rsi, 1 \t\t;inc. ponteiro string" << endl;
            arqComandos << "\tmov bl, [rsi] \t\t;carrega caractere" << endl;
            
            arqComandos << rotInicio << ":" << endl;
            arqComandos << "\tpush rdx \t\t;empilha sinal" << endl;
            arqComandos << "\tmov rdx, 0 \t\t;reg. multiplicação" << endl;
            
            arqComandos << rotLoop << ":" << endl;
            arqComandos << "\tcmp bl, 0Ah \t\t;verifica fim string" << endl;
            arqComandos << "\tje " << rotFim << "\t\t;salta se fim string" << endl; 
            arqComandos << "\tcmp bl, \'.\' \t\t;senão verifica ponto" << endl; 
            arqComandos << "\tje " << rotPonto << "\t\t;salta se ponto" << endl; 
            arqComandos << "\timul ecx \t\t;mult. eax por 10" << endl; 
            arqComandos << "\tsub bl, \'0\' \t\t;converte caractere" << endl; 
            arqComandos << "\tadd eax, ebx \t\t;soma valor caractere" << endl; 
            arqComandos << "\tadd rsi, 1 \t\t;incrementa base" << endl; 
            arqComandos << "\tmov bl, [rsi] \t\t;carrega caractere" << endl; 
            arqComandos << "\tjmp " << rotLoop << "\t\t;loop" << endl; 
           
            arqComandos << rotPonto << ":" << endl;
            arqComandos << "\t\t;calcula parte fracionária em xmm0" << endl; 
            arqComandos << "\tadd rsi, 1 \t\t;inc. ponteiro string" << endl; 
            arqComandos << "\tmov bl, [rsi] \t\t;carrega caractere" << endl; 
            arqComandos << "\tcmp bl, 0Ah \t\t;*verifica fim string" << endl; 
            arqComandos << "\tje " << rotFim << "\t\t;salta se fim string" << endl; 
            arqComandos << "\tsub bl, \'0\' \t\t;converte caractere" << endl; 
            arqComandos << "\tcvtsi2ss xmm1,rbx \t\t;conv real" << endl; 
            arqComandos << "\tdivss xmm1,xmm2 \t\t;transf. casa decimal" << endl; 
            arqComandos << "\taddss xmm0,xmm1 \t\t;soma acumul." << endl; 
            arqComandos << "\tmulss xmm2,xmm3 \t\t;atualiza potência" << endl; 
            arqComandos << "\tjmp " << rotPonto << "\t\t;loop" << endl; 
            
            arqComandos << rotFim << ":" << endl; 
            arqComandos << "\tcvtsi2ss xmm1,rax \t\t;conv parte inteira" << endl; 
            arqComandos << "\taddss xmm0,xmm1 \t\t;soma parte frac." << endl; 
            arqComandos << "\tpop rcx \t\t;desempilha sinal" << endl; 
            arqComandos << "\tcvtsi2ss xmm1,rcx \t\t;conv sinal" << endl; 
            arqComandos << "\tmulss xmm0,xmm1 \t\t;mult. sinal" << endl;

            arqComandos << "\tmovss [M+" << id_end << "],xmm0\t\t;move float para id_end" << endl;

        }

        arqComandos << ";Fim Comando READ" << endl;
    } //fim CmdRead()


/* 
CmdIf    -> "if" Exp(1) (Cmd | CmdS) ["else" (Cmd | CmdS)]
(1) {se exp_tipo != TIPO_BOOL entao ERRO}
*/
    void CmdIf() {
        arqComandos << ";Comando IF" << endl;
      
        int exp_tipo, exp_tam, exp_end;
        
        string rotFalso = novoRot(); 
        string rotFim = novoRot();
        
        CasaToken(IF);
        Exp(exp_tipo, exp_tam, exp_end); 
        contadorTemp = 0; // Contador de temporários é reinicializado depois de um comando chamar uma expressão

        if(exp_tipo != TIPO_BOOL)
            throw(7);

        arqComandos << "\tmov eax,[M+" << exp_end << "]" << endl; // Carregando conteúdo(0/1) do endereço exp_end para eax
        arqComandos << "\tcmp eax,0" << endl;   // Comparando com imediato 0 para verificar se é falso ou verdadeiro
        arqComandos << "\tje " << rotFalso << endl; // Se for igual a 0(falso), pular para rotFalso

        if(token_lido == ABRE_CHAVE)
            CmdS();
        else
            Cmd();
        
        arqComandos << "\tjmp " << rotFim << endl; // Se verdadeiro, realiza comandos do IF e vai para o final
        arqComandos << rotFalso << ":" << endl; // Rótulo para executar o else, se ele existir
        
        if(token_lido == ELSE) {
            CasaToken(ELSE);
            if(token_lido == ABRE_CHAVE)
                CmdS();
            else
                Cmd();
        }

        arqComandos << rotFim << ":" << endl; // Fim do teste        
        arqComandos << ";Fim Comando IF" << endl;
    } //fim CmdIf()


/* 
CmdWhile -> "while" Exp(1) (Cmd | CmdS)
(1) {se exp_tipo != TIPO_BOOL entao ERRO}
*/
    void CmdWhile() {
        arqComandos << ";Comando WHILE" << endl;

        int exp_tipo, exp_tam, exp_end;
        
        string rotInicio = novoRot();
        string rotFim = novoRot();
        
        CasaToken(WHILE);

        arqComandos << rotInicio << ":" << endl; // Rótulo para ínicio do loop while
        
        Exp(exp_tipo, exp_tam, exp_end);
        contadorTemp = 0; // Contador de temporários é reinicializado depois de um comando chamar uma expressão
        
        if(exp_tipo != TIPO_BOOL)
            throw(7);

        arqComandos << "\tmov eax,[M+" << exp_end << "]" << endl; // Carregando conteúdo(0/1) do endereço exp_end para eax
        arqComandos << "\tcmp eax,0" << endl; // Comparando com imediato 0 para verificar se é falso ou verdadeiro
        arqComandos << "\tje " << rotFim << endl; // Se for igual a 0(falso), salta para o Fim
            
        if (token_lido == ABRE_CHAVE)
            CmdS();
        else
            Cmd();
        
        arqComandos << "\tjmp " << rotInicio << endl; // Se verdadeiro, realiza comandos e volta para o inicio para continuar a iteração
        arqComandos << rotFim << ":" << endl;   // Fim da iteração
        arqComandos << ";Fim Comando WHILE" << endl;
    } //fim CmdWhile()


/*
* Cmd      -> CmdWhile | CmdIf | CmdRead | CmdWrite | CmdAtr | CmdNull
*/
    void Cmd() {

        if (token_lido == WHILE)
            CmdWhile();
        else if (token_lido == IF)
            CmdIf();
        else if (token_lido == READLN)
            CmdRead();
        else if (token_lido == WRITE || token_lido == WRITELN)
            CmdWrite();
        else if (token_lido == PONTO_VIRGULA)
            CmdNull();
        else
            CmdAtr();

    } //fim Cmd()


/*
* CmdS     -> "{" {Cmd} "}"
*/    
    void CmdS() {

        CasaToken(ABRE_CHAVE);
        while(token_lido == WHILE || token_lido == IF || token_lido == READLN || token_lido == WRITE || 
              token_lido == WRITELN || token_lido == ID || token_lido == PONTO_VIRGULA) {
            Cmd();
        }
        CasaToken(FECHA_CHAVE);

    } //fim CmdS()


/* 
Dec      -> ("int"(1) | "float"(2) | "string"(3) | "char"(4)) {id(5) ["<-"[-](11)const(12)(6)](7)  [,]}+ ; | "const"(8) id(9) = [-](11)const(12)(10)(7);
(1) {dec_tipo = TIPO_INT; dec_classe = CLASSE_VAR}
(2) {dec_tipo = TIPO_FLOAT; dec_classe = CLASSE_VAR}
(3) {dec_tipo = TIPO_STR; dec_classe = CLASSE_VAR}
(4) {dec_tipo = TIPO_CHAR; dec_classe = CLASSE_VAR}
(5) {se(id_tipo != TIPO_VAZIO) entao ERRO senao id_tipo = dec_tipo; id_classe = dec_classe}
(6) {verificaCompatibilidadeAtr_Dec(id_tipo, const_tipo)}
(7) {atualizaTS()}
(8) {dec_classe = CLASSE_CONST}
(9) {se (id_tipo != TIPO_VAZIO) entao ERRO senao id_classe = dec_classe}
(10) {dec_tipo = const_tipo}
(11) {se token_lido == MENOS entao ehNumero = true}
(12) {se (ehNumero && (const_tipo != TIPO_INT && const_tipo != TIPO_FLOAT) entao ERRO }
*/
    void Dec() {
        arqDados << ";DECLARAÇÃO" << endl;
        int dec_tipo, dec_classe, id_tipo, id_classe, id_tam, const_tipo;
        int id_end;
        int tam_reserva;
        string id_lexema;
        bool ehNumero = false, ehNeg = false;

        // Declaração de variável
        if(token_lido == INT || token_lido == FLOAT || token_lido == STRING || token_lido == CHAR) {

            if(token_lido == INT){
                dec_tipo = TIPO_INT;
                dec_classe = CLASSE_VAR; 
                CasaToken(INT);
            }else if(token_lido == FLOAT) {
                dec_tipo = TIPO_FLOAT;
                dec_classe = CLASSE_VAR; 
                CasaToken(FLOAT);
            }else if(token_lido == STRING){
                dec_tipo = TIPO_STR;
                dec_classe = CLASSE_VAR; 
                CasaToken(STRING);
            }else {
                dec_tipo = TIPO_CHAR;
                dec_classe = CLASSE_VAR; 
                CasaToken(CHAR);
            }

            do { 

                id_tipo = reg_lex.tipo;
                id_tam = reg_lex.tam;

                if(id_tipo != TIPO_VAZIO)   // Verificação de Unicidade de identificador
                    throw(5);


                id_tipo = dec_tipo;
                id_classe = dec_classe;                  

                Hash.atualizarTS(reg_lex.lexema, id_tipo, id_classe, id_tam);

                id_end = Hash.resgataEndereco(reg_lex.lexema);
                           
                CasaToken(ID);

                if(token_lido == MENOR_MENOS) {
                    CasaToken(MENOR_MENOS);
                    if(token_lido == MENOS) {//Verificação de sinal
                        CasaToken(MENOS);
                        ehNumero = true;//Flag se é int ou float
                        ehNeg = true;//Flag de sinal negativo
                    }

                    const_tipo = reg_lex.tipo;

                    if(ehNumero && (const_tipo != TIPO_INT && const_tipo != TIPO_FLOAT)) 
                        throw(7);
                   
                    verificaCompatibilidadeAtr_Dec(id_tipo, const_tipo);

                    // Declaração e Inicialização
                    if(dec_tipo == TIPO_STR) {
                        tam_reserva = 32-(reg_lex.tam-1);//tam_reserva recebe a quantidade de bytes restante para completar 32 bytes                
                        arqDados << "\tdb " << reg_lex.lexema << ",0\t; String em " << contadorDados << endl;
                        arqDados << "\tresb " << tam_reserva << endl;
                        contadorDados += 32;
                    } else if (dec_tipo == TIPO_CHAR) {
                        tam_reserva = 1;
                        arqDados << "\tdb " << reg_lex.lexema << "\t\t; Car. em " << contadorDados << endl;
                        contadorDados += tam_reserva;
                    } else if(dec_tipo == TIPO_INT) {
                        tam_reserva = 4;
                        if(ehNeg)
                            arqDados << "\tdd -" << reg_lex.lexema << "\t\t; Int em " << contadorDados << endl;   // Inteiro com sinal
                        else
                            arqDados << "\tdd " << reg_lex.lexema << "\t\t; Int em " << contadorDados << endl;    // Inteiro sem sinal
                        contadorDados += tam_reserva;
                    } else if(dec_tipo == TIPO_FLOAT) {
                        tam_reserva = 4;
                        string float_lex = reg_lex.lexema;
                        
                        if(const_tipo == TIPO_INT) {//Se float recebe inteiro, adicionamos .0 para sua conversão
                            float_lex += ".0";
                        }

                        if(ehNeg)
                            arqDados << "\tdd -" << float_lex << "\t\t; Float em " << contadorDados << endl; // Float com sinal
                        else
                            arqDados << "\tdd " << float_lex << "\t\t; Float em " << contadorDados << endl;   // Float sem sinal

                        
                        contadorDados += tam_reserva;
                    }                    
                   
                    CasaToken(VALOR);
                
                } else { // Declaração sem Inicialização
                    
                    if(dec_tipo == TIPO_STR) {
                        tam_reserva = 32;
                        arqDados << "\tresb 32\t; String em " << contadorDados << endl; //0x100
                        contadorDados += tam_reserva;
                    } else if (dec_tipo == TIPO_CHAR) {
                        tam_reserva = 1;
                        arqDados << "\tresb 1\t\t; Car. em " << contadorDados << endl;
                        contadorDados += tam_reserva;
                    } else if(dec_tipo == TIPO_INT){
                        tam_reserva = 4;  
                        arqDados << "\tresd 1\t\t; Int em " << contadorDados << endl;
                        contadorDados += tam_reserva;
                    } else if(dec_tipo == TIPO_FLOAT) {
                        tam_reserva = 4;
                        arqDados << "\tresd 1\t\t; Float em " << contadorDados << endl;
                        contadorDados += tam_reserva;
                    }
                                       
                }

                if(token_lido == VIRGULA)
                    CasaToken(VIRGULA);

            }while(token_lido == ID);

        } else {   // Declaração de constante

            dec_classe = CLASSE_CONST;
            CasaToken(CONST);
            
            id_tipo = reg_lex.tipo;
            id_lexema = reg_lex.lexema;
            id_tam = reg_lex.tam;
                
            if(id_tipo != TIPO_VAZIO)   // Verificação de Unicidade de identificador
                throw(5);
            else 
                id_classe = dec_classe;
            

            CasaToken(ID);
            CasaToken(IGUAL);   

            if(token_lido == MENOS) {//Verificação de sinal
                CasaToken(MENOS);
                ehNumero = true;//Flag se é int ou float
                ehNeg = true;//Flag de sinal negativo
            }

            const_tipo = reg_lex.tipo;
            dec_tipo = const_tipo;

            if(ehNumero && (const_tipo != TIPO_INT && const_tipo != TIPO_FLOAT)) 
                throw(7);

            Hash.atualizarTS(id_lexema, dec_tipo, id_classe, id_tam);

            if(dec_tipo == TIPO_STR) {
                tam_reserva = reg_lex.tam - 1;  // -1 de (-2 das aspas +1 do '\0')
                arqDados << "\tdb " << reg_lex.lexema << ",0\t; String em " << contadorDados << endl;
                contadorDados += tam_reserva;
            } else if (dec_tipo == TIPO_CHAR) {
                tam_reserva = 1;
                arqDados << "\tdb " << reg_lex.lexema << "\t\t; Car. em " << contadorDados << endl;
                contadorDados += tam_reserva;
            } else if(dec_tipo == TIPO_INT){
                tam_reserva = 4;  
                if(ehNeg)
                    arqDados << "\tdd -" << reg_lex.lexema << "\t\t; Int em " << contadorDados << endl;   // Inteiro com sinal
                else
                    arqDados << "\tdd " << reg_lex.lexema << "\t\t; Int em " << contadorDados << endl;    // Inteiro sem sinal
                contadorDados += tam_reserva;
            } else if(dec_tipo == TIPO_FLOAT) {
                tam_reserva = 4;
                if(ehNeg)
                    arqDados << "\tdd -" << reg_lex.lexema << "\t\t; Float em " << contadorDados << endl; //Float com sinal
                else
                    arqDados << "\tdd " << reg_lex.lexema << "\t\t; Float em " << contadorDados << endl;  // Float sem sinal
                contadorDados += tam_reserva;
            }   

            CasaToken(VALOR);
        }
        CasaToken(PONTO_VIRGULA);
    } //fim Dec()


/*
* S        -> {Dec | (Cmd | CmdS)}
*/
    void S() {

        contadorDados += 0x10000;//10000h

        arqDados << "section .data\t\t; Sessão de dados" << endl;
        arqDados << "M:\t\t\t; Rótulo para demarcar o início da sessão de dados" << endl;       
        arqDados << "resb " << contadorDados << "\t\t; Reserva de temporários" << endl;
        
        arqComandos << "section .text\t\t; Sessão de código" << endl;
        arqComandos << "global _start\t\t; Ponto inicial do programa" << endl;
        arqComandos << "_start:\t\t\t; Início do programa" << endl;
         
        while(token_lido == INT || token_lido == FLOAT || token_lido == STRING || token_lido == CHAR || token_lido == CONST ||
              token_lido == ABRE_CHAVE || token_lido == WHILE || token_lido == IF || token_lido == READLN || token_lido == WRITE ||
              token_lido == WRITELN || token_lido == ID || token_lido == PONTO_VIRGULA) {
            if(token_lido == INT || token_lido == FLOAT || token_lido == STRING || token_lido == CHAR || token_lido == CONST) {
                Dec();
            } else {                

                if(token_lido == ABRE_CHAVE) {
                    CmdS();
                } else {
                    Cmd();                    
                }
            }
        } //fim while

        arqComandos << "; Halt" << endl;
        arqComandos << "mov rax, 60\t\t; Código de saída" << endl;
        arqComandos << "mov rdi, 0\t\t; Código de saída sem erros" << endl;
        arqComandos << "syscall\t\t\t; Chama o kernel";
            
    } //fim S()  
    
}; //fim Sintatico


/*
* Método que concatena o arquivo de dados e de comandos do assembly em um único arquivo .asm
*/
void concatArqs() {
    arqComandos.close();
    
    arqLeituraComandos.open("cmd.txt");
    char ch;
    while(arqLeituraComandos.get(ch))//Enquanto !EOF leia
        arqDados << ch;
} //fim concatArqs

int main() {   
    try {

        //Inicializando arquivo de saida
        arqDados.open("saida.asm");
        arqComandos.open("cmd.txt");

        Hash.preencherPR();//Método para preencher a tabela hash com as palavras reservadas e seus tokens
        token_lido = analisadorLexico();//token_lido recebe o primeiro token do programa fonte
        Sintatico sint;
        sint.S();//Símbolo inicial da gramática é chamado
        CasaToken(END);//Verificação de EOF
        cout << linha << " linhas compiladas.";
        concatArqs();//Método chamado para concatenar os arquivos da geração de código assembly
        arqDados.close();//Fluxo de saida fechado
        
    } catch(int tipo) {//catch com os erros previstos pelo compilador
        if(tipo == 0) {
            cout << linha << "\n" << "lexema nao identificado [" << reg_lex.lexema << "].";
        }
        else if (tipo == 1) {
            cout << linha << "\n" << "caractere invalido.";
        }
        else if (tipo == 2)  {
            cout << linha << "\n" << "fim de arquivo nao esperado.";
        }
        else if (tipo == 3)  {
            cout << linha << "\n" << "token nao esperado [" << reg_lex.lexema << "].";
        }
        else if (tipo == 4)  {
            cout << linha << "\n" << "identificador nao declarado [" << reg_lex.lexema << "].";
        }
        else if (tipo == 5)  {
            cout << linha << "\n" << "identificador ja declarado [" << reg_lex.lexema << "].";
        }
        else if (tipo == 6)  {
            cout << linha << "\n" << "classe de identificador incompativel [" << reg_lex.lexema << "].";
        }
        else if (tipo == 7)  {
            cout << linha << "\n" << "tipos incompativeis.";
        }

    } //fim catch

    return 0;
    
} //main