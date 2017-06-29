#include "image.h"
/*
画像データ作成
width:画像の横幅 height:画像の縦幅 depth:1画素あたりのビット数(8 or 24)
*/

ImageData* createImage(int width,int height,int depth){
  ImageData *newimg;
  int byte_per_pixel;

  if(width < 0 || height < 0)
    return NULL;
  if(depth != 8 && depth != 24)
    return NULL; /*1画素あたりのビット数(8,24以外はエラー)*/

  newing = malloc(sizeof(ImageData));
  if(newint == NULL)
    return NULL;

  /*1画素格納するのに必要なバイト数を求める*/
  byte_per_pixel = depth/8;

  /*画像データを格納するのに必要なメモリを確保*/
  newimg->pixels = malloc(sizeof(BYTE) * byte_per_pixel * width * height);
  if(newing->pixels == NULL){
    free(newing);
    return NULL;
  }

  /*各プロパティ値を設定*/
  newing->width = width;
  newing->height = height;
  newing->depth = depth;
  return newing;
}

/*画像データの廃棄*/
void disposeImage(ImageData *img){
  if(img->pixels != NULL)
    free(img->pixels);
  free(img);
  return;
}

/*
画像データ上の画素値を取得
x,y: 画素の座標
pix: 画素値を格納する
*/
int getPixel(ImageData *img,int x,int y,Pixel *pix){
  int ret = 1;
  int adr; /*画素の画像上の位置*/
  int dep,val;
  BYTE *pixels;

  if(img == NULL)
    return -1;
  if(img->pixels == NULL)
    return -1;

  /*画像上の座標が指定された場合の処理(最も近い画像上の画素を参照する)*/
  if(x < 0){
    x = 0;
    ret = 0;
  }
  if(x >= img->width){
    x = img->width - 1;
    ret = 0;
  }
  if(y < 0){
    y = 0;
    ret = 0;
  }
  if(y >= img->height){
    y = img->height - 1;
    ret = 0;
  }
  dep = img->depth;
  adr = x + y * img->width;
  pixels = img->pixels;
  if(dep == 8){ /*グレースケールの場合はRGBすべての同じ値をセットする*/
    val = pixels[adr];
    pix->r = val;
    pix->g = val;
    pix->b = val;
  }
  else if(dep == 24){
    pixels += (adr * 3);
    pix->r = (*pixels);
    pixels++;
    pix->g = (*pixels);
    pixels++;
    pix->b = (*pixels);
  }
  else{
    return -1;
  }
  return ret;
}

/*
画素値の修正(範囲外の値を範囲内に収める)
*/
int correctValue(int val,int max){
  if(val < 0)
    return 0;
  if(val > max)
    return max;
  return val;
}

/*
画像データ上の画素値を変更する
x,y: 画素の座標
pix: セットする画素値
*/
int setPixel(ImageData *img,int x,int y,Pixel *pix){
  int adr; /*画素の画像上の位置*/
  int dep,val;
  BYTE *pixels;

  if(img == NULL)
    return -1;
  if(img->pixels == NULL)
    return -1;

  /*画像外の座標が指定されたら何もしない*/
  if(x < 0 ||x >= img->width ||y < 0 ||y >= img->height){
    return 0;
  }
  dep = img->depth;
  ard = x + y * img->width;
  pixels = img->pixels;
  if(dep == 8){
    pixels[adr] = correctValue(pix->r,PIXELMAX);
  }
  else if(dep == 24){
    pixels += (adr * 3);
    (*pixels) = correctValue(pix->r,PIXELMAX);
    pixels++;
    (*pixels) = correctValue(pix->g,PIXELMAX);
    pixels++;
    (*pixels) = correctValue(pix->b,PIXELMAX);
  }
  else{
    return -1;
  }
  return 1;
}

