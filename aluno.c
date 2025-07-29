#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Necessário para a função time
#include <limits.h> // Para INT_MAX
#include "aluno.h"
#include "disciplina.h"

// Adiciona a declaração externa de qtdDisciplinas, assumindo que está definida em disciplina.c
extern int qtdDisciplinas;
extern Disciplina disciplinas[]; // Adiciona a declaração externa do vetor de disciplinas
#define MAX_ALUNOS 1000000

Aluno alunos[MAX_ALUNOS];
int qtdAlunos = 0;

void registrarLog(char *mensagem) {
    FILE *logFile = fopen("buscas.log", "a"); // Abre o arquivo log em modo de adição
    if (logFile != NULL) {
        fprintf(logFile, "%s\n", mensagem); // Grava a mensagem no arquivo
        fclose(logFile); // Fecha o arquivo
    } else {
        printf("Erro ao abrir o arquivo de log.\n");
    }
}
void cadastrarAluno(FILE *arq) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return;
    }

    Aluno aluno;
    printf("\nCadastrar Aluno\n");
    printf("Nome: ");
    scanf("%s", aluno.nome);
    printf("Matricula: ");
    scanf("%d", &aluno.matricula);
    printf("Disciplina: ");
    scanf("%s", aluno.disciplina);
    printf("Email: ");
    scanf("%s", aluno.email);
    aluno.qtdDisciplinas = 0; // Inicializa o número de disciplinas do aluno

    // Grava o aluno no arquivo de forma binária
    fwrite(&aluno, sizeof(Aluno), 1, arq);

    printf("Aluno cadastrado com sucesso!\n");
}



void listarAlunos(FILE *arq) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return;
    }

    Aluno aluno;
    printf("\nListar Alunos\n");
    
    // Lê cada aluno do arquivo até o final
    while (fread(&aluno, sizeof(Aluno), 1, arq)) {
        printf("Nome: %s | Matricula: %d | E-mail: %s\n", aluno.nome, aluno.matricula, aluno.email);
        printf("Disciplinas matriculadas: ");
        
        // Exibe as disciplinas do aluno
        for (int j = 0; j < aluno.qtdDisciplinas; j++) {
            printf("%d", aluno.disciplinas[j]);
            if (j < aluno.qtdDisciplinas - 1) printf(", ");
        }
        printf("\n");
    }
}



void salvarAlunos(FILE *arq) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return;
    }

    // Grava todos os alunos no arquivo de forma binária
    fwrite(&alunos, sizeof(Aluno), qtdAlunos, arq);

    printf("Dados dos alunos salvos com sucesso!\n");
}


void carregarAlunos(FILE *arq) {
    if (arq != NULL) {
        // Lê os dados dos alunos no arquivo e armazena na variável alunos
        qtdAlunos = fread(&alunos, sizeof(Aluno), 100, arq);  // Lê até 100 alunos do arquivo
        printf("Dados dos alunos carregados com sucesso!\n");
    } else {
        printf("Erro ao abrir o arquivo de alunos.\n");
    }
}

int buscaBinariaAluno(FILE *arq, int matricula) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return -1;
    }

    clock_t comeco, final;
    double tempoGasto;
    int comparacoes = 0; // Contador de comparações
    comeco = clock();

    int inicio = 0, fim;
    
    // Obtém o número total de alunos no arquivo para definir o valor de "fim"
    fseek(arq, 0, SEEK_END); // Move o ponteiro do arquivo para o final
    long tamanhoArquivo = ftell(arq);  // Obtém o tamanho do arquivo
    fim = tamanhoArquivo / sizeof(Aluno) - 1; // Define "fim" como o índice do último aluno

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;

        // Posiciona o ponteiro do arquivo na posição do "meio" para ler o aluno
        fseek(arq, meio * sizeof(Aluno), SEEK_SET);
        Aluno aluno;
        fread(&aluno, sizeof(Aluno), 1, arq);  // Lê o aluno do arquivo

        comparacoes++; // Incrementa a contagem de comparações

        if (aluno.matricula == matricula) {
            final = clock();
            tempoGasto = (double)(final - comeco) / CLOCKS_PER_SEC;
            char logMsg[256];
            snprintf(logMsg, sizeof(logMsg), "Busca Binária: %d comparações, tempo: %.2f ms", comparacoes, tempoGasto * 1000);
            registrarLog(logMsg); // Registra no arquivo log
            return meio;  // Retorna o índice do aluno encontrado
        } else if (aluno.matricula < matricula) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    final = clock();
    tempoGasto = (double)(final - comeco) / CLOCKS_PER_SEC;
    char logMsg[256];
    snprintf(logMsg, sizeof(logMsg), "Busca Binária: %d comparações, tempo: %.2f ms (não encontrado)", comparacoes, tempoGasto * 1000);
    registrarLog(logMsg); // Registra no arquivo log

    return -1;  // Se não encontrar o aluno, retorna -1
}



int buscaSequencialAluno(FILE *arq, int matricula) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return -1;
    }

    clock_t comeco, final;
    double tempoGasto;
    int comparacoes = 0;
    comeco = clock();

    Aluno aluno;
    int i = 0;

    // Lê o arquivo e percorre os alunos sequencialmente
    while (fread(&aluno, sizeof(Aluno), 1, arq)) {
        comparacoes++;  // Incrementa a contagem de comparações
        if (aluno.matricula == matricula) {
            final = clock();
            tempoGasto = (double)(final - comeco) / CLOCKS_PER_SEC;
            char logMsg[256];
            snprintf(logMsg, sizeof(logMsg), "Busca Sequencial: %d comparações, tempo: %.2f ms", comparacoes, tempoGasto * 1000);
            registrarLog(logMsg);  // Registra no arquivo log
            return i;  // Retorna o índice do aluno encontrado
        }
        i++;  // Incrementa o índice do aluno
    }

    final = clock();
    tempoGasto = (double)(final - comeco) / CLOCKS_PER_SEC;
    char logMsg[256];
    snprintf(logMsg, sizeof(logMsg), "Busca Sequencial: %d comparações, tempo: %.2f ms (não encontrado)", comparacoes, tempoGasto * 1000);
    registrarLog(logMsg);  // Registra no arquivo log

    return -1;  // Se não encontrar o aluno, retorna -1
}



// Função de ordenação Bubble Sort para alunos
void bubbleSortAlunos(FILE *arq) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return;
    }

    // Calcula o número total de alunos no arquivo
    fseek(arq, 0, SEEK_END);  // Move o ponteiro para o final do arquivo
    long tamanhoArquivo = ftell(arq);  // Obtém o tamanho do arquivo
    int qtdAlunos = tamanhoArquivo / sizeof(Aluno);  // Calcula o número de alunos no arquivo
    fseek(arq, 0, SEEK_SET);  // Volta o ponteiro para o início do arquivo

    Aluno aluno;
    Aluno alunos[qtdAlunos];  // Cria um array para armazenar os alunos do arquivo

    // Lê os alunos do arquivo
    for (int i = 0; i < qtdAlunos; i++) {
        fread(&alunos[i], sizeof(Aluno), 1, arq);
    }

    // Ordenação Bubble Sort dos alunos pela matrícula
    for (int i = 0; i < qtdAlunos - 1; i++) {
        for (int j = 0; j < qtdAlunos - 1 - i; j++) {
            if (alunos[j].matricula > alunos[j + 1].matricula) {
                // Troca os alunos de posição no array
                Aluno temp = alunos[j];
                alunos[j] = alunos[j + 1];
                alunos[j + 1] = temp;
            }
        }
    }

    // Volta o ponteiro para o início do arquivo e grava os alunos ordenados de volta no arquivo
    fseek(arq, 0, SEEK_SET);
    for (int i = 0; i < qtdAlunos; i++) {
        fwrite(&alunos[i], sizeof(Aluno), 1, arq);
    }

    printf("\nBase de dados de alunos ordenada com sucesso!\n");
}



void matricularAlunoEmDisciplina(FILE *arq, int matriculaAluno, int *codigosDisciplinas, int qtdDisciplinas) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return;
    }

    Aluno aluno;
    int encontrado = 0;

    // Busca o aluno no arquivo
    while (fread(&aluno, sizeof(Aluno), 1, arq)) {
        if (aluno.matricula == matriculaAluno) {
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Aluno não encontrado.\n");
        return;
    }

    // Matricular o aluno em cada disciplina
    for (int i = 0; i < qtdDisciplinas; i++) {
        // Verifica se o aluno já está matriculado nesta disciplina
        int jaMatriculado = 0;
        for (int j = 0; j < aluno.qtdDisciplinas; j++) {
            if (aluno.disciplinas[j] == codigosDisciplinas[i]) {
                jaMatriculado = 1;  // O aluno já está matriculado nesta disciplina
                break;
            }
        }

        // Se não estiver matriculado, adiciona a disciplina
        if (!jaMatriculado && aluno.qtdDisciplinas < MAX_DISCIPLINAS) {
            aluno.disciplinas[aluno.qtdDisciplinas] = codigosDisciplinas[i];
            aluno.qtdDisciplinas++;
            printf("Aluno matriculado na disciplina %d com sucesso.\n", codigosDisciplinas[i]);
        } else if (jaMatriculado) {
            printf("Aluno já está matriculado na disciplina %d.\n", codigosDisciplinas[i]);
        } else {
            printf("Aluno não pode ser matriculado em mais de %d disciplinas.\n", MAX_DISCIPLINAS);
            break;
        }
    }

    // Volta o ponteiro do arquivo para a posição do aluno encontrado e grava as alterações
    // Reposiciona o ponteiro no arquivo para a posição correta do aluno
    fseek(arq, -(long)(sizeof(Aluno)), SEEK_CUR);
    fwrite(&aluno, sizeof(Aluno), 1, arq);  // Grava os dados do aluno atualizados de volta no arquivo

    printf("Matricula realizada com sucesso!\n");
}




void editarAluno(FILE *arq, int matricula) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return;
    }

    Aluno aluno;
    int encontrado = 0;

    // Busca o aluno no arquivo
    while (fread(&aluno, sizeof(Aluno), 1, arq)) {
        if (aluno.matricula == matricula) {
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Aluno não encontrado.\n");
        return;
    }

    // Solicita a nova informação para o aluno
    printf("Novo Nome: ");
    scanf("%s", aluno.nome);
    printf("Nova Disciplina: ");
    scanf("%s", aluno.disciplina);
    printf("Novo E-mail: ");
    scanf("%s", aluno.email);

    // Volta o ponteiro do arquivo para a posição do aluno encontrado e grava as alterações
    // Reposiciona o ponteiro de volta para a posição do aluno no arquivo para a gravação dos dados atualizados
    long posicao = ftell(arq) - sizeof(Aluno);  // Obtém a posição onde o aluno foi lido
    fseek(arq, posicao, SEEK_SET);  // Volta para a posição do aluno

    fwrite(&aluno, sizeof(Aluno), 1, arq);  // Grava os dados do aluno atualizados de volta no arquivo

    printf("Dados do aluno atualizados com sucesso!\n");
}



// ...código existente...

void cancelarMatriculaAluno(FILE *arq, int matricula) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return;
    }

    // Lê todos os alunos do arquivo para o array em memória
    fseek(arq, 0, SEEK_SET);
    int total = fread(alunos, sizeof(Aluno), 1000000, arq);

    // Procura o aluno e remove do array
    int encontrado = 0;
    for (int i = 0; i < total; i++) {
        if (alunos[i].matricula == matricula) {
            for (int j = i; j < total - 1; j++) {
                alunos[j] = alunos[j + 1];
            }
            total--;
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Aluno não encontrado.\n");
        return;
    }

    // Sobrescreve o arquivo com os alunos restantes
    freopen(NULL, "wb", arq); // Reabre o arquivo no mesmo ponteiro para sobrescrever
    fwrite(alunos, sizeof(Aluno), total, arq);

    // Atualiza a variável global
    qtdAlunos = total;

    printf("Aluno de matrícula %d removido com sucesso.\n", matricula);
}

// Função para gerar partições usando seleção natural
void gerarParticoesSelecaoNatural(FILE *arq, int tamanhoMemoria) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de alunos.\n");
        return;
    }

    // Cria buffer para armazenar alunos em memória
    Aluno *buffer = (Aluno*)malloc(tamanhoMemoria * sizeof(Aluno));
    if (buffer == NULL) {
        printf("Erro ao alocar memória para o buffer.\n");
        return;
    }

    int numParticao = 0;
    int alunosLidos = 0;
    int totalAlunos = 0;

    // Calcula o número total de alunos no arquivo
    fseek(arq, 0, SEEK_END);
    long tamanhoArquivo = ftell(arq);
    totalAlunos = tamanhoArquivo / sizeof(Aluno);
    fseek(arq, 0, SEEK_SET);

    printf("\nIniciando geração de partições com seleção natural...\n");
    printf("Total de alunos: %d\n", totalAlunos);
    printf("Tamanho da memória: %d alunos\n", tamanhoMemoria);

    while ((alunosLidos = fread(buffer, sizeof(Aluno), tamanhoMemoria, arq)) > 0) {
        // Ordena o buffer atual usando bubble sort
        for (int i = 0; i < alunosLidos - 1; i++) {
            for (int j = 0; j < alunosLidos - 1 - i; j++) {
                if (buffer[j].matricula > buffer[j + 1].matricula) {
                    Aluno temp = buffer[j];
                    buffer[j] = buffer[j + 1];
                    buffer[j + 1] = temp;
                }
            }
        }

        // Aplica seleção natural para criar sequências ordenadas maiores
        int *marcados = (int*)calloc(alunosLidos, sizeof(int));
        int sequenciaAtual = 0;
        
        // Cria nome do arquivo da partição
        char nomeArquivo[256];
        snprintf(nomeArquivo, sizeof(nomeArquivo), "particoes/particao_%d.dat", numParticao);
        
        FILE *arquivoParticao = fopen(nomeArquivo, "wb");
        if (arquivoParticao == NULL) {
            printf("Erro ao criar arquivo de partição %s\n", nomeArquivo);
            free(buffer);
            free(marcados);
            return;
        }

        // Implementa seleção natural
        int ultimoEscrito = -1;
        int alunosEscritos = 0;
        
        while (alunosEscritos < alunosLidos) {
            int menorIndice = -1;
            int menorMatricula = INT_MAX;
            
            // Encontra o próximo aluno válido na sequência
            for (int i = 0; i < alunosLidos; i++) {
                if (!marcados[i] && buffer[i].matricula >= ultimoEscrito && buffer[i].matricula < menorMatricula) {
                    menorMatricula = buffer[i].matricula;
                    menorIndice = i;
                }
            }
            
            // Se não encontrou nenhum válido, reinicia a busca (nova sequência)
            if (menorIndice == -1) {
                ultimoEscrito = -1;
                for (int i = 0; i < alunosLidos; i++) {
                    if (!marcados[i] && buffer[i].matricula < menorMatricula) {
                        menorMatricula = buffer[i].matricula;
                        menorIndice = i;
                    }
                }
            }
            
            if (menorIndice != -1) {
                // Escreve o aluno no arquivo da partição
                fwrite(&buffer[menorIndice], sizeof(Aluno), 1, arquivoParticao);
                marcados[menorIndice] = 1;
                ultimoEscrito = buffer[menorIndice].matricula;
                alunosEscritos++;
            } else {
                break; // Não deveria acontecer, mas previne loop infinito
            }
        }

        fclose(arquivoParticao);
        free(marcados);
        
        printf("Partição %d criada com %d alunos (arquivo: %s)\n", 
               numParticao, alunosLidos, nomeArquivo);
        numParticao++;
    }

    free(buffer);
    
    printf("\nParticionamento concluído!\n");
    printf("Total de partições criadas: %d\n", numParticao);
    printf("Partições salvas na pasta: particoes/\n");
}

// Função para verificar o conteúdo das partições
void verificarParticoes() {
    printf("\nVerificando conteúdo das partições:\n");
    printf("=====================================\n");
    
    int numParticao = 0;
    char nomeArquivo[256];
    
    while (1) {
        snprintf(nomeArquivo, sizeof(nomeArquivo), "particoes/particao_%d.dat", numParticao);
        
        FILE *arquivoParticao = fopen(nomeArquivo, "rb");
        if (arquivoParticao == NULL) {
            break; // Não há mais partições
        }
        
        printf("\nPartição %d (%s):\n", numParticao, nomeArquivo);
        
        Aluno aluno;
        int contador = 0;
        int ultimaMatricula = -1;
        int ordenada = 1;
        
        while (fread(&aluno, sizeof(Aluno), 1, arquivoParticao)) {
            printf("  Aluno %d: %s (Matrícula: %d)\n", contador + 1, aluno.nome, aluno.matricula);
            
            // Verifica se está ordenada
            if (ultimaMatricula != -1 && aluno.matricula < ultimaMatricula) {
                ordenada = 0;
            }
            ultimaMatricula = aluno.matricula;
            contador++;
        }
        
        printf("  Total de alunos: %d\n", contador);
        printf("  Partição ordenada: %s\n", ordenada ? "SIM" : "NÃO");
        
        fclose(arquivoParticao);
        numParticao++;
    }
    
    if (numParticao == 0) {
        printf("Nenhuma partição encontrada. Execute primeiro a geração de partições.\n");
    } else {
        printf("\nTotal de partições verificadas: %d\n", numParticao);
    }
}