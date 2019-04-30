#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node{
  char c;
  int freq;
  struct node *left, *right;
} node;

node *nodes[256], *HuffmanTree;
int fileLen, freq[256], numNode;
int bits[20];
unsigned power2[32], code[256], codeLen[256];

void init(void){
  int i;

  power2[0] = 1;
  for (i=1; i<32; i++)
    power2[i] = 2*power2[i-1];
}

void Bits(unsigned bits){
  int i, count;

  for (i=0; i<32; i++){
    count = bits & power2[31-i];
    printf("%1d", count ? 1 : 0);
  }
}

void ipfile(void){
  int i;
  FILE *fp;
  char line[1024];

  for (i=0; i<256; i++)
    freq[i] = 0;
  fp = fopen("Image-1.png", "r");
  fileLen = 0;
  while (fgets(line, 1024, fp) != NULL){
    for (i=0; i<strlen(line); i++){
      freq[line[i]]++;
      fileLen++;
    }
  }
  fclose(fp);
  printf("file length %d\n", fileLen);
}

void minHeap(void){
  node *new;
  int i, j, min, count;

  for (i=0, numNode=0; i<256; i++){
    if (freq[i]){
      nodes[numNode] = (node*)malloc(sizeof(node));
      nodes[numNode]->c = i;
      nodes[numNode]->freq = freq[i];
      nodes[numNode]->left = nodes[numNode]->right = NULL;
      numNode++;
    }
  }

  count = numNode - 1;
  for (i=0; i<count; i++){
    new = (node*)malloc(sizeof(node));
    min = 0;
    for (j=1; j<numNode; j++)
      if (nodes[j]->freq < nodes[min]->freq)
	min = j;
    new->right = nodes[min];
    nodes[min] = nodes[--numNode];
    min = 0;
    for (j=1; j<numNode; j++)
      if (nodes[j]->freq < nodes[min]->freq)
	min = j;
    new->left = nodes[min];
    nodes[min] = nodes[--numNode];
    new->freq = new->left->freq + new->right->freq;
    nodes[numNode++] = new;
  }
  HuffmanTree = nodes[0];
}

void traverse(node *t, int depth){
  int i;

  if (t->left == NULL && t-> right == NULL){
    printf("%c\t", t->c);
    for (i=0; i<depth; i++)
      printf("%1d", bits[i]);
    printf("\n");
    return;
  }
  if (t->left != NULL){
    bits[depth] = 0;
    traverse(t->left, depth+1);
  }
  if (t->right != NULL){
    bits[depth] = 1;
    traverse(t->right, depth+1);
  }
}

void huffmanCodes(node *t, int depth){
  int i;

  if (t->left == NULL && t-> right == NULL){
    code[t->c] = 0;
    codeLen[t->c] = depth;
    for (i=0; i<depth; i++)
      code[t->c] |= (bits[i] << (31-i));
    return;
  }
  if (t->left != NULL){
    bits[depth] = 0;
    huffmanCodes(t->left, depth+1);
  }
  if (t->right != NULL){
    bits[depth] = 1;
    huffmanCodes(t->right, depth+1);
  }
}

void binaryFile(void){
  FILE *in, *out;
  unsigned buffer, bitsInBuffer;
  unsigned char c;
  int i;

  in = fopen("", "rb");
  out = fopen(".huf", "wb");
  fwrite(&fileLen, sizeof(int), 1, out);
  buffer = 0;
  bitsInBuffer = 0;
  for (i=0; i<fileLen; i++){
    fread(&c, sizeof(unsigned char), 1, in);
    buffer |= (code[c] >> bitsInBuffer);
    bitsInBuffer += codeLen[c];
    while (bitsInBuffer >= 8){
      c = buffer >> 24;
      fwrite(&c, sizeof(unsigned char), 1, out);
      buffer = buffer << 8;
      bitsInBuffer -= 8;
    }
  }
  if (bitsInBuffer){
    c = buffer >> 24;
    fwrite(&c, sizeof(unsigned char), 1, out);
  }
  fclose(in);
  fclose(out);
}

int main(int argc, char *argv[]){
  int i;

  init();
  ipfile();
  minHeap();
  traverse(HuffmanTree, 0);
  huffmanCodes(HuffmanTree, 0);
  for (i=0; i<256; i++){
    if (freq[i]){
      printf("%3d %c %2d ", i, (char)i, codeLen[i]);
      Bits(code[i]);
      printf("\n");
    }
  }
  binaryFile();

  return 0;
}