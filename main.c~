#include <stdio.h>
#include "image.h"

int main(){

char *fname = "./SAMPLE.bmp";
 char *wname = "./out.bmp";

 // int by = 3;

 ImageData *img;
 ImageData *outimg;
 puts("1");
 readBMPfile(fname,&img);
 puts("2");
 outimg = createImage(img->width,img->height,img->depth);
 puts("3");
 printf("read[%s]\n",fname);

 // transform(img,outimg);

 writeBMPfile(wname,outimg);
 puts("4");
 printf("write[%s]\n",wname);
 disposeImage(img);
}
