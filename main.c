#include "dstlib_include.h"

// Protótipos das funções
void tokenizar(char *expr, Fila *fila);
void infixaParaPosfixa(Fila *infixa, Fila *posfixa);
int avaliarPosfixa(Fila *posfixa);

// Função principal
int main()
{
    // Declarações das variáveis
    Fila filaTokens, filaPosfixa, copiaPosfixa;
    char expressao[MAX];
    
    // Arquivos
    FILE *entrada, *saida;
    
    // Inicialização das filas
    initFila(&filaTokens);
    initFila(&filaPosfixa);
    initFila(&copiaPosfixa);
    
    // Abertura dos arquivos
    entrada = fopen("in.txt", "r");
    if (entrada == NULL) {
        entrada = fopen("in.txt", "w");
        if (entrada == NULL) {
            printf("Erro ao tentar abrir o arquivo de texto!\n");
            return 1;
        }
        printf("Arquivo 'in.txt' criado.\n");
        return 1;
    }

    saida = fopen("out.txt", "w");
    if (saida == NULL) {
        printf("Erro ao abrir o arquivo de saída!\n");
        fclose(entrada);
        return 1;
    }

    int contador = 1; //Contador para mostrar as linhas no arquivo
    
    // Processamento de cada linha (expressão)
    while (fgets(expressao, MAX, entrada)) {
        // Inicializa filas a cada iteração
        initFila(&filaTokens);
        initFila(&filaPosfixa);
        initFila(&copiaPosfixa);

        // Tokenizar expressão
        tokenizar(expressao, &filaTokens);

        // Converter infixa para posfixa
        infixaParaPosfixa(&filaTokens, &filaPosfixa);

        // Mostrar posfixa (opcional)
        printf("\nExpressão Posfixa:\n");
        while (!vaziaFila(&filaPosfixa)) {
            Token tk = frente(&filaPosfixa);
            printf("%c ", tk.value);
            inserir(&copiaPosfixa, tk);
            remover(&filaPosfixa);
        }
        printf("\n");

        // Avaliar a expressão posfixa
        int resultado = avaliarPosfixa(&copiaPosfixa);

        // Gravar resultado no arquivo
        if (resultado != INT_MAX) {
            fprintf(saida, "%d - Resultado: %d\n", contador, resultado);
        } else {
            fprintf(saida, "%d - Expressão inválida.\n", contador);
        }
        contador++;
    }

    // Fechar arquivos
    fclose(entrada);
    fclose(saida);

    system("PAUSE");
    return 0;
}

// Função para tokenizar expressão
void tokenizar(char *expr, Fila *fila) {
    int i = 0;
    Token tk;

    while (expr[i] != '\0' && expr[i] != '\n') {
        if (isdigit(expr[i])) {
            tk.value = expr[i];
            tk.type = NUMERO;
            tk.peso = 0;
            inserir(fila, tk);
        }
        else if (expr[i] == '+' || expr[i] == '-') {
            tk.value = expr[i];
            tk.type = OPERADOR;
            tk.peso = 1;
            inserir(fila, tk);
        }
        else if (expr[i] == '*' || expr[i] == '/') {
            tk.value = expr[i];
            tk.type = OPERADOR;
            tk.peso = 2;
            inserir(fila, tk);
        }
        
        else if (expr[i] == '^'){
             tk.value = expr[i];
             tk.type = OPERADOR;
             tk.peso = 3;
             inserir(fila, tk);   
        }
        //
        
        else if (expr[i] == '(' || expr[i] == ')') {
            tk.value = expr[i];
            tk.type = PARENTESES;
            tk.peso = 0;
            inserir(fila, tk);
        }
        else if (expr[i] != ' ') {
            printf("Caractere inválido: %c\n", expr[i]);
        }
        i++;
    }
}

// Função para converter de infixa para posfixa
void infixaParaPosfixa(Fila *infixa, Fila *posfixa) {
    Pilha operadores;
    Token tk, aux;
    initPilha(&operadores);

    while (!vaziaFila(infixa)) {
        tk = frente(infixa);

        if (tk.type == NUMERO) {
            inserir(posfixa, tk);
        }
        else if (tk.type == OPERADOR) {
            while (!vaziaPilha(&operadores) && top(&operadores).type == OPERADOR &&
                   top(&operadores).peso >= tk.peso) {
                aux = top(&operadores);
                pop(&operadores);
                inserir(posfixa, aux);
            }
            push(&operadores, tk);
        }
        else if (tk.value == '(') {
            push(&operadores, tk);
        }
        else if (tk.value == ')') {
            while (!vaziaPilha(&operadores) && top(&operadores).value != '(') {
                aux = top(&operadores);
                pop(&operadores);
                inserir(posfixa, aux);
            }
            if (!vaziaPilha(&operadores) && top(&operadores).value == '(') {
                pop(&operadores);
            } else {
                printf("Erro: Parênteses desbalanceados.\n");
                return;
            }
        }
        remover(infixa);
    }

    while (!vaziaPilha(&operadores)) {
        aux = top(&operadores);
        pop(&operadores);
        inserir(posfixa, aux);
    }
}

// Função para avaliar expressão posfixa
int avaliarPosfixa(Fila *posfixa) {
    Pilha_int valores;
    Token tk;
    int a, b, resultado;

    initPilhaInt(&valores);

    while (!vaziaFila(posfixa)) {
        tk = frente(posfixa);

        if (tk.type == NUMERO) {
            push_int(&valores, tk.value - '0');
        }
        else if (tk.type == OPERADOR) {
            if (!vaziaPilhaInt(&valores)) {
                b = top_int(&valores);
                pop_int(&valores);
            } else {
                printf("Erro: Expressão mal formada.\n");
                return INT_MAX;
            }

            if (!vaziaPilhaInt(&valores)) {
                a = top_int(&valores);
                pop_int(&valores);
            } else {
                printf("Erro: Expressão mal formada.\n");
                return INT_MAX;
            }

            switch (tk.value) {
                case '+': resultado = a + b; break;
                case '-': resultado = a - b; break;
                case '*': resultado = a * b; break;
                case '^': resultado = pow(a, b); break;
                case '/':
                    if (b == 0) {
                        printf("Erro: Divisão por zero.\n");
                        return INT_MAX;
                    }
                    resultado = a / b;
                    break;
                default:
                    printf("Operador desconhecido: %c\n", tk.value);
                    return INT_MAX;
            }

            push_int(&valores, resultado);
        }

        remover(posfixa);
    }

    if (!vaziaPilhaInt(&valores)) {
        resultado = top_int(&valores);
        pop_int(&valores);
    } else {
        printf("Erro: Pilha vazia no final.\n");
        return INT_MAX;
    }

    if (!vaziaPilhaInt(&valores)) {
        printf("Erro: Elementos sobrando na pilha no final.\n");
        return INT_MAX;
    }

    return resultado;
}
