#include <stdio.h>
#include "image.h"

int main(){

char *fname = "./SAMPLE.bmp";
 char *wname = "./out.bmp";

 // int by = 3;

 ImageData *img;
 ImageData *outimg;

 readBMPfile(fname,&img);

 outimg = createImage(img->width,img->height,img->depth);

 printf("read[%s]\n",fname);

 // transform(img,outimg);

 writeBMPfile(wname,outimg);

 printf("write[%s]\n",wname);
 disposeImage(img);
}
