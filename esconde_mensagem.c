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

void imprime(struct BITMAP_header header, struct DIB_header dibheader, struct Image image, char arquivo[])
{
    FILE *fp = fopen(arquivo, "w");
    if (fp == NULL)
    {
        printf("Nao foi possivel abrir o arquivo\n");
        return;
    }
    int i;
    fwrite(header.name, 2, 1, fp);
    fwrite(&header.size, 3 * sizeof(int), 1, fp);
    fwrite(&dibheader, sizeof(struct DIB_header), 1, fp);
    fseek(fp, header.image_offset, SEEK_SET);
    fwrite(image.rgb, 3 * image.largura * image.altura, sizeof(unsigned char), fp);
    fclose(fp);
}

void estenografia(char s[], struct Image image)
{
    int pixel = 0, id_s = 0, bit_letra = 0;
    char c = '@'; // indica fim e inicio d frase
    bit_letra = 0;
    while (id_s < strlen(s))
    {
        while (bit_letra < 8)
        {
            if (image.rgb[pixel] & 1)
                image.rgb[pixel] ^= 1;
            if (s[id_s] & (1 << bit_letra))
                image.rgb[pixel] |= 1;
            pixel++, bit_letra++;
        }
        bit_letra = 0, id_s++;
    }
    while (bit_letra < 8)
    {
        if (image.rgb[pixel] & 1)
            image.rgb[pixel] ^= 1;
        if (c & (1 << bit_letra))
            image.rgb[pixel] |= 1;
        pixel++, bit_letra++;
    }
}

void openbmp(char s[], char arquivo[])
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

    estenografia(s, image);
    imprime(header, dibheader, image, arquivo);

    fclose(fp);
    freeImage(image);
}

int main()
{
    char s[TAMFRASE], arquivo[TAMFRASE];
    printf("Digite uma frase: ");
    scanf("%[^\n]", s);
    printf("Digite o nome da imagem na qual a mensagem sera escondida: ");
    scanf("%*c%[^\n]", arquivo);
    openbmp(s, arquivo);
}