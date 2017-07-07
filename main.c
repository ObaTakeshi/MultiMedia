#include <stdio.h>
#include "image.h"

void transform(ImageData *img, ImageData *outimg);

int main(){
    
    char *fname = "./SAMPLE.bmp";
    char *wname = "./out.bmp";
    
    // int by = 3;
    
    ImageData *img;
    ImageData *outimg;
    
    readBMPfile(fname,&img);
    
    outimg = createImage(img->width,img->height,img->depth);
    
    printf("read[%s]\n",fname);
    
    transform(img,outimg);
    
    writeBMPfile(wname,outimg);
    
    printf("write[%s]\n",wname);
    disposeImage(img);
}

void transform(ImageData *img,ImageData *outimg){
    int x,y;
    int a,b;
    int set,get;
    Pixel pix;
    
    for(x=0;x<img->width;x++){
        for(y=0;y<img->height;y++){
            a = getPixel(img,x,y,&pix);
            if (a == -1){
                printf("getError(%d,%d)¥n",x,y);
            }
            b = setPixel(outimg,x,y,&pix);
            if (b == -1){
                printf("setError(%d,%d)¥n",x,y);
            }
        }
    }
}
