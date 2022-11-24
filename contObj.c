#include <stdio.h>
#include <stdlib.h>

typedef struct simg {
  char id[3];
  int col, lin, max;
  unsigned char rf, gf, bf;
  char **mat;
  int **matVis;
} image;

typedef struct {
  int x, y;
  struct coord *prox;
} coord;

typedef struct {
  coord *topo;
  int tam;
} pilha;

void initPilha(pilha *pilha);
void push(pilha *pilha, int x, int y);
void pop(pilha *pilha, int *x, int *y);
void percorreObjeto(image *img, pilha *pilCoord);
coord catchLayer(pilha *pilCoord, int layer);

int main() {
  char arq[] = "./img/02.ppm";  // Caminho relativo
  int init;                     // Posição inicial da imagem no arquivo
  unsigned char r, g, b;        // Receber a leitura de RGB da chunk
  int obj = 0;

  pilha coords;  // Pilha de coordenadas
  initPilha(&coords);

  FILE *fp;

  image img;

  fp = fopen(arq, "rb");
  fscanf(fp, "%s", img.id);
  fscanf(fp, "%d", &img.col);
  fscanf(fp, "%d", &img.lin);
  fscanf(fp, "%d", &img.max);
  fscanf(fp, "%c", &r);  // retira o \n

  init = ftell(fp);                                 // Posição de onde começa a imagem
  fscanf(fp, "%c%c%c", &img.rf, &img.gf, &img.bf);  // Analisa o primeiro char pra saber a cor do fundo
  fseek(fp, init, SEEK_SET);

  if (fp == NULL) {  // Erro no arquivo
    printf("Erro na abertura do arquivo <%s>", arq);
    exit(EXIT_FAILURE);
  }

  img.mat = (char **)malloc(img.lin * sizeof(char *));  // cria o ponteiro pra matriz
  for (int i = 0; i < img.lin; i++) {                   // atribui a matriz ao ponteiro
    img.mat[i] = (char *)malloc(img.col * sizeof(char));
  }

  img.matVis = (int **)malloc(img.lin * sizeof(int *));  // cria o ponteiro pra matriz
  for (int i = 0; i < img.lin; i++) {                    // atribui a matriz ao ponteiro
    img.matVis[i] = (int *)malloc(img.col * sizeof(int));
  }

  // Percorre imagem e atribui na img.mat
  for (int i = 0; i < img.lin; i++) {
    for (int j = 0; j < img.col; j++) {
      fscanf(fp, "%c%c%c", &r, &g, &b);
      if ((r != img.rf) || (g != img.gf) || (b != img.bf)) {  // Checa pixels diferentes do fundo
        img.mat[i][j] = 'X';
      } else {
        img.mat[i][j] = '.';
      }
    }
  }
  fclose(fp);

  // Contador de Objetos
  for (int i = 0; i < img.lin; i++) {
    for (int j = 0; j < img.col; j++) {
      if (img.mat[i][j] == 'X') {
        obj++;
        push(&coords, i, j);
        img.matVis[i][j] = 1;
        percorreObjeto(&img, &coords);
      }
    }
  }

  // Printa a imagem
  for (int i = 0; i < img.lin; i++) {
    for (int j = 0; j < img.col; j++) {
      printf("%c", img.mat[i][j]);
    }
    printf("\n");
  }
  printf("\nTem %d objetos", obj);
}

void initPilha(pilha *pilha) {
  pilha->topo = NULL;  // Não ha nada no topo
  pilha->tam = 0;      // Não nenhuma coordenada
}

void push(pilha *pilha, int x, int y) {
  coord *novo = (coord *)malloc(sizeof(coord));  // Cria novo no de coordenada
  novo->x = x;
  novo->y = y;

  if (pilha->topo == NULL) {  // Pilha vazia ?
    novo->prox = NULL;        // No na base nao tem proximo
    pilha->topo = novo;       // novo no vai para o topo da pilha
  } else {                    // Pilha nao esta vazia
    novo->prox = pilha->topo;
    pilha->topo = novo;
  }

  pilha->tam++;  // Aumenta em um no o tamanho da pilha
};

void pop(pilha *pilha, int *x, int *y) {
  if (pilha->topo != NULL) {
    coord *temp = pilha->topo;
    pilha->topo = pilha->topo->prox;

    *x = temp->x;
    *y = temp->y;
    free(temp);    // Libera a memoria ocupada pelo topo anterior, o no que esta sendo removido
    pilha->tam--;  // Diminui em um no o tamanho da pilha
  }
};

void percorreObjeto(image *img, pilha *pilCoord) {
  int x, y;
  int region;
  while (pilCoord->tam != 0) {  // NAO SAI ATE A PILHA ZERAR
    x = pilCoord->topo->x;
    y = pilCoord->topo->y;

    region = 0;
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        if (img->mat[x + i][y + j] == 'X' && !img->matVis[x + i][y + j]) {  // Checa se na região ao redor tem objeto
          push(pilCoord, x + i, y + j);                                     // Coloca na pilha a nova coordenada
          img->matVis[x + i][y + j] = 1;                                    // Marca como visitado
        } else {
          region++;
        }
      }
    }

    if (region == 9) {
      pop(pilCoord, x, y);
    }
  }
}
