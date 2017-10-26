#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <windows.h>

int N, M, color;

void fillheader(char header[]) {
    int filesize;
    // BITMAPFILEHEADER
    BITMAPFILEHEADER bfh;
    ZeroMemory(&bfh, sizeof(bfh));
    
    bfh.bfType = 0x4d42;                        // ���������, ������ ���� 'BM'
	bfh.bfSize = (color == 2) ? 54 + M*ceil(3 * N / 4.0) * 4 : 54 + M * ceil(N / 4.0) * 4 + 1024;                            // ��������� ������ �����
    bfh.bfReserved1 = 0;                        //
    bfh.bfReserved2 = 0;                        //
    bfh.bfOffBits = color ? 54 : 54 + 256 * 4;    // ������ ���������� ������, �� ��������� ������ �������
    memcpy(header, &bfh, 14);                   // �������� � ������ header

    // BITMAPINFOHEADER;
    BITMAPINFOHEADER bih;
    ZeroMemory(&bih, 40);

    bih.biSize = 40;                            // ������ ��������� BITMAPINFOHEADER
    bih.biWidth = N;                            // ������
    bih.biHeight = M;                           // ������, ������� �������� ��� ������ ���� ���������� ����� �����
    bih.biPlanes = 1;                           //
    bih.biBitCount = color ? 24 : 8;            // ����� ��� �� �������
    memcpy(header + 14, &bih, 40);              // �������� � ������ header
}


void filldata(char data[], int **r, int **g, int **b) {
    int i, j;
	int linesize = ceil(3 * N / 4.0) * 4;
	int k = 0;
	if (color == 2)
	{
		for (i = M - 1; i >= 0; i--)
		{
			for (j = 0; j < N; j++)
			{
				data[(M - 1 - i) * linesize + j * 3] = b[i][j];
				data[(M - 1 - i) * linesize + j * 3 + 1] = g[i][j];
				data[(M - 1 - i) * linesize + j * 3 + 2] = r[i][j];				
			}
		}
	}
	else 
	{
		linesize = ceil(N / 4.0) * 4;
	for (i = M - 1; i >= 0; i--) {
		for (j = 0; j < N; j++) {
			data[(M - 1 - i) * linesize + (j)] = b[i][j];
		}
	}
}
    // ��������� ������.
    // ������: ���������� ����� �����, � ������� ����� ������� b, g, r
    // � ������ �� ���� ������ b
}

void fillpalette(char palette[]) {
	if (color == 2) return;
	// ���� ��, ���� ��������� palette �������
	int i, j=0;
	int linesize = ceil(3 * N / 4.0) * 4;
	for (i = 0; i < 1024; i += 4)
	{
		palette[i] = j;
		palette[i + 1] = j;
		palette[i + 2] = j;
		palette[i + 3] = 0;
		j++;
	}
	i = 0;
	// 0 0 0 0 1 1 1 0 2 2 2 0 3 3 3 0 ... 255 255 255 0
}
int main(char argc, char* argv[]) {
    int i, j, **r=0, **g=0, **b=0;
    std::ifstream f;
    char *filename; 
    if (argc > 1) filename = argv[1]; else filename = "inputgray.txt";
    f = std::ifstream(filename);
    if (f.fail()) {
        std::cerr << "could not open file\n";
        return -1;
    }

    f >> N >> M >> color;
    // �������� ������ ��� ���� (��� ��) ��� ��� (��� ��) ������� MxN 
    b = new int*[M]; for (i = 0; i < M; i++) b[i] = new int[N];
    if (color==2) {
        g = new int*[M]; for (i = 0; i < M; i++) g[i] = new int[N];
        r = new int*[M]; for (i = 0; i < M; i++) r[i] = new int[N];
    }

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            if (color==2)
                f >> r[i][j] >> g[i][j] >> b[i][j];
            else
                f >> b[i][j];
        }
    }
    f.close();

    char *bmpfilename = new char[strlen(filename) + 4];
    strcpy(bmpfilename, filename);
    strcat(bmpfilename, ".bmp");
    FILE* bmpfile = fopen(bmpfilename, "wb");
    char header[54];
    char palette[4 * 256];

    int datasize = color==2 ? M * ceil(3 * N / 4.0) * 4 : M * ceil( N / 4.0) * 4;   // !!!!!!! ������ ���������� ������, ����� ������ ������ ��� ������ 4 ������
                                                            // ��������� ��� �� ������, ������ �������� � ���� 1.
    char* data = new char[datasize];

    fillheader(header);         // ��������� ���������
    fillpalette(palette);       // ��������� ������� (���� ����)
    filldata(data, r, g, b);    // ��������� ������ ���������� ������

    fwrite(header, 1, 54, bmpfile);             // �������� ���������
    if (color==0) {       
        fwrite(palette, 1, 4 * 256, bmpfile);   // ���� ��, �������� �������
    }
    fwrite(data, 1, datasize, bmpfile);         // �������� ���������� ������

    fclose(bmpfile);
}