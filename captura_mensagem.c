#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAMFRASE 100

struct BITMAP_header
{
    char name[2]; // BM
    unsigned int size;
    int garbage;
    unsigned int image_offset;
};

struct DIB_header
{
    unsigned int header_size;
    unsigned int altura, largura;
    unsigned short int colorplanes, bitsperpixel;
    unsigned int compression;
    unsigned int image_size;
};

struct Image
{
    int largura, altura;
    unsigned char *rgb;
};

struct Image readImage(FILE *fp, int altura, int largura)
{
    struct Image pic;
    int i;
    pic.rgb = (unsigned char *)malloc(3 * altura * largura * sizeof(unsigned char));
    pic.altura = altura;
    pic.largura = largura;
    fread(pic.rgb, 3 * largura * altura, sizeof(unsigned char), fp);
    return pic;
}

void freeImage(struct Image pic)
{
    int i;
    free(pic.rgb);
}

void captura_frase(char frase[], struct Image image)
{
    int i, pixel = 0, id_s = 0, bit_letra = 0;
    for (i = 0; i < 50; i++)
    {
        frase[i] = (char)0;
    }
    char c = (char)0; // indica fim e inicio d frase
    bit_letra = 0;
    while (id_s < 49)
    {
        while (bit_letra < 8)
        {
            c |= ((image.rgb[pixel] & 1) << bit_letra);
            pixel++, bit_letra++;
        }
        if (c == '@')
        {
            frase[id_s] = '\0';
            break;
        }
        bit_letra = 0;
        frase[id_s++] = c;
        c = (char)0;
    }
    printf("A frase eh: %s\n", frase);
}

void openbmp(char arquivo[])
{
    FILE *fp = fopen(arquivo, "r");
    if (fp == NULL)
    {
        printf("Nao foi possivel abrir o arquivo\n");
        return;
    }
    struct BITMAP_header header;
    struct DIB_header dibheader;

    fread(header.name, 2, 1, fp);
    if (header.name[0] != 'B' || header.name[1] != 'M')
    {
        printf("Imagem invalida\n");
        return;
    }
    fread(&header.size, 3 * sizeof(int), 1, fp);
    fread(&dibheader, sizeof(struct DIB_header), 1, fp);
    fseek(fp, header.image_offset, SEEK_SET);
    struct Image image = readImage(fp, dibheader.altura, dibheader.largura);

    char frase[TAMFRASE];
    captura_frase(frase, image);

    fclose(fp);
    freeImage(image);
}

int main()
{
    char arquivo[TAMFRASE];
    printf("Digite o nome da imagem na qual a mensagem sera escondida: ");
    scanf("%[^\n]", arquivo);
    openbmp(arquivo);
}