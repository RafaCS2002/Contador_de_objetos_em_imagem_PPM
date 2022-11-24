#include <stdio.h>
#include <stdlib.h>

typedef struct simg {
  char id[3];
  int col, lin, max;
  unsigned char rf, gf, bf;
  char **mat;
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
void pop(pilha *pilha);
int percorreObjeto(image *img, pilha *coords, FILE **fp, int x, int y);
void proxCoord(coord *topo, int xy[2], int layer);

int main() {
  char arq[] = "./img/02.ppm";  // Caminho relativo
  int init;                     // Posição inicial da imagem no arquivo
  unsigned char r, g, b;        // Receber a leitura de RGB da chunk

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

  if (fp == NULL) {
    printf("Erro na abertura do arquivo <%s>", arq);
    exit(EXIT_FAILURE);
  }

  img.mat = (char **)malloc(img.lin * sizeof(char *));  // cria o ponteiro pra matriz
  for (int i = 0; i < img.lin; i++) {                   // atribui a matriz ao ponteiro
    img.mat[i] = (char *)calloc(img.col, sizeof(char));
  }

  // Percorre imagem
  for (int i = 0; i < img.lin; i++) {
    for (int j = 0; j < img.col; j++) {
      fscanf(fp, "%c%c%c", &r, &g, &b);
      if (img.mat[i][j] != '.' && img.mat[i][j] != 'X') {
        if ((r != img.rf) || (g != img.gf) || (b != img.bf)) {  // Checa pixels diferentes do fundo
          percorreObjeto(&img, &coords, fp, i, j);
        } else {
          img.mat[i][j] = '.';
        }
      }
    }
  }
  fclose(fp);

  for (int i = 0; i < img.lin; i++) {
    for (int j = 0; j < img.col; j++) {
      printf("%c", img.mat[i][j]);
    }
    printf("\n");
  }
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

void pop(pilha *pilha) {  // ***alterar
  if (pilha->topo != NULL) {
    coord *temp = pilha->topo;
    pilha->topo = pilha->topo->prox;

    free(temp);  // Libera a memoria ocupada pelo topo anterior, o no que esta sendo removido

    pilha->tam--;  // Diminui em um no o tamanho da pilha
  }
};

int percorreObjeto(image *img, pilha *coords, FILE **fp, int x, int y) {
  push(coords, x, y);           // Coloca as coordenadas atuais na pilha
  img->mat[x][y] = 'X';         // Add o pixel de objeto na matriz, ou seja, ja foi verificado
  int posiCurrent = ftell(fp);  // Inicializa variável para guardar a posição atual
  int posiNav = posiCurrent;    // Inicializa na posição atual
  unsigned char r, g, b;
  int xy[2];

  printf("%d\n", coords->topo->x);
  printf("%d\n", coords->topo->y);

  // entra na pilha, a partir dela, analisa os arredores
  while (coords->tam != 0) {                 // NAO SAI ATE A PILHA ZERAR -- Soh vai sair dps de por td na matriz
    for (int z = 0; z < coords->tam; z++) {  // Percorre a pilha
      coord *topo = (coords)->topo;          // Coordenada do topo
      proxCoord(topo, xy, z);                // XY passa a ter as coordenadas da posição z da pilha
      // if (img->mat[][j] != '.' && img->mat[i][j] != 'X') {
      // }
      // // Checa em cima
      // (qnt_de_col * tam_de_chunk) - (tam_de_chunk * volta_uma_leitura)
      // posiNav = (img->col * 3) - (3 * 1);
      // fseek(fp, posiNav, SEEK_CUR);
      // fscanf(fp, "%c%c%c", &r, &g, &b);
      // if ((r != img->rf) || (g != img->gf) || (b != img->bf)) {
      //   push(coords, x, y);
      // }
      // posiNav = ((img->lin) * 3) - 1;
      // fseek(fp, posiNav, SEEK_CUR);
    }
    // Tem q descarregar as coord na matriz depois
    break;
  }
  return posiCurrent;
}

void proxCoord(coord *topo, int xy[2], int layer) {
  // pilha - para pegar as coordenadas
  // xy - para receber as coordenadas respectivas da camada da pilha
  // layer - posição da camada requisitada da pilha
  coord *temp = topo;
  if (layer != 0) {  // não é topo
    // Percorre a pilha e faz temp receber
    for (int foo = 0; foo < layer; foo++) {  // qnts vezes precisa pegar o prox
      temp = temp->prox;
    }
    xy[0] = temp->x;
    xy[1] = temp->y;
  } else {  // eh topo
    xy[0] = temp->x;
    xy[1] = temp->y;
  }
  printf("\nx = %d\ny = %d", xy[0], xy[1]);
}
