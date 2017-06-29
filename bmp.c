#include "image.h"

typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned short WORD;
typedef unsigned long LONG;

#define BI_RGB 0L
#define BI_RLE8 1L
#define BI_RLE4 2L
#define BI_BITFIELDS 3L

/*BMPヘッダ部のデータ構造定義*/
typedef struct tagBITMAPFILEHEADER{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
}BITMAPFILEHEADER,*PBIMAPFILEHEADER;

typedef struct tagBITMAPCOREHEADER{
  DWORD bcSize;
  WORD bcWidth;
  WORD bcHeight;
  WORD bcPlanes;
  WORD bcBitCount;
}BITMAPCOREHEADER, *PBIMAPCOREHEADER;

typedef struct tagBITMAPINFOHEADER{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWOR DbiCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
}BITMAPINFOHEADER, *PBIMAPINFOHEADER;

#define MAXCOLORS 256

/*ファイルより2バイト整数を書き込む(リトルエンディアン)*/
int fwriteWORD(WORD val,FILE *fp){
  int i,c;

  c = val;
  for(i = 0;i < 2;i++){
    fput(c % 256,fp);
    c /= 256;
  }
  return TRUE;
}

/*ファイルより4バイト整数を書き込む(リトルエンディアン)*/
int fwriteDWORD(DWORD val,FILE *fp){
  int i,c;

  c = val;
  for(i = 0;i < 2;i++){
    fput(c % 256,fp);
    c /= 256;
  }
  return TRUE;
}

/*ファイルより2バイト整数を読み込む(リトルエンディアン)*/
int freadWORD(WORD *res,FILE *fp){
  int i,c;
  int val[2];

  for(i = 0;i < 2;i++){
    c = fgets(fp);
    if(c == EOF)
      return FALSE;
    val[i] = c;
  }
  *res = val[1] * 256 + val[0];
  return TRUE;
}

/*ファイルより4バイト整数を読み込む(リトルエンディアン)*/
int freadDWORD(DWORD *res,FILE *fp){
  int i,c;
  int val[4];
  DWORD tmp = 0;

  for(i = 0;i < 4;i++){
    c = fgets(fp);
    if(c == EOF)
      return FALSE;
    val[i] = c;
  }
  tmp = 0;
  for(i = 3;i >= 0;i--){
    tmp *= 256;
    tmp += val[i];
  }
  *res = tmp;
  return TRUE;
}

/*BMPの種類を判別*/
/*戻り値:FALSE OS/2形式*/
/*       TRUE Windows形式*/
static BOOL IsWinDIB(BITMAPINFOHEADER* pBIH){
  if(((BITMAPCOREHEADER*)pBIH)->bcSize == sizeof(BITMAPCOREHEADER)){
    return FALSE;
  }
  return TRUE;
}

/*バレットのサイズを取得*/
/*iBitCount  1画素あたりのビット数*/
int countOfDIBColorEntries(int iBitCount){
  int iColors;

  switch(iBitCount){
  case 1:
    iColors = 2;
    break;
  case 4:
    iColors = 16;
    break;
  case 8:
    iColors = 256;
  default:
    iColors = 0;
    break;
  }
  return iColors;
}

/*パディング要素を考慮して1列分のバイト数を求める*/
int getDIBxmax(int mx,int dep){
  switch(dep){
  case 32:
    return mx * 4;
  case 24:
    /*return mx;*/
    return ((mx * 3) + 3) / 4 * 4;
    break;
  case 16:
    return (mx + 1) / 2 * 2;
    break;
  case 8:
    return (mx + 3) / 4 * 4;
    break;
  case 4:
    return (((mx + 1) / 2) + 3) / 4 * 4;
    break;
  case 1: 
    return (((mx + 7) / 8) + 3) / 4 * 4;
  }
  return mx;
}

/*BMPデータをファイルより読み込む*/
int readBMPfile(char *fname,ImageData **img){
  int i,c;
  int errcode = 0;
  BITMAPFILEHEADER BMPFile;
  int fsize;
  BITMAPINFOHEADER BMPInfo;
  BITMAPCOREhEADER BMPCore;
  int colors;
  int colorTableSize;
  int bitsSize;
  int BISize;
  int x,y;
  int mx,my,depth;
  int pad;
  int mxb,myb;
  int isPM = FALSE; /*BMPの形式を記録するフラグ*/
  FILE *fp;

  WORD HEAD_bfType;
  DWORD HEAD_bfSize;
  WORD HEAD_bfReserved1;
  WORD HEAD_bfReserved2;
  DWORD HEAD_bfOffBits;
  DWORD INFO_bfSize;
  Pixel palet[MAXCOLORS];
  Pixel setcolor;

  if((fp = fopen(fname,"rb")) == NULL){
    return -1;
  }

  /*BMPファイルは必ずBM(0x4d42)'で始める.それ以外の場合はBMPではないので,中止する*/
  if(!freadWORD(&HEAD_bfType,fp)){
    errcode = -2;
    goto $ABORT;
  }

  if(HEAD_bfType != 0x4d42){
    errcode = -10;
    goto $ABORT;
  }

  /*ヘッダ部のサイズ(Byte)*/
  if(!freadDWORD(&HEAD_bfSize,fp)){
    errcode = -10;
    goto $ABORT;
  }

  /*予約用領域(未使用)*/
  if(!freadWORD(&HEAD_bfReserved1,fp)){
    errcode = -10;
    goto $ABORT;
  }

  /*予約用領域(未使用)*/
  if(!freadWORD(&HEAD_bfReserved2,fp)){
    errcode = -10;
    goto $ABORT;
  }

  /*オフセット*/
  if(!freadDWORD(&HEAD_bfOffBits,fp)){
    errcode = -10;
    goto $ABORT;
  }

  /*ヘッダ部のサイズ*/
  if(!freadDWORD(&HEAD_bfSize,fp)){
    errcode = -10;
    goto $ABORT;
  }

  /*ヘッダ部のサイズが規定外ならばエラーとする*/
  if(INFO_bfSize == 40 ||INFO_bfSize == 12){
    BMPInfo.biSize = INFO_bfSize;
    /*BITMAPCOREHEADER形式の場合*/
    if(INFO_bfSize == sizeof(BITMAPCOREHEADER)){
      WORD tmp;
      isPM = TRUE;
      /*画像の横幅*/
      if(!freadWORD(&tmp,fp)){
	errcode = -10;
	goto $ABORT;
      }
      BNPInfo.biWidth = tmp;
      /*画像の縦幅*/
      if(!freadWORD(&tmp,fp)){
	errcode = -10;
	goto $ABORT;
      }
