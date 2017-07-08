#include <stdio.h>
#include "image.h"

void linear(ImageData *img, ImageData *outimg);

int main(){
    
    char *fname = "./SAMPLE.bmp";
    char *wname = "./out.bmp";
    
    ImageData *img;
    ImageData *outimg;
    
    readBMPfile(fname,&img);
    
    outimg = createImage(img->width,img->height,img->depth);
    
    printf("read[%s]\n",fname);
    
    linear(img,outimg);
    
    writeBMPfile(wname,outimg);
    
    printf("write[%s]\n",wname);
    disposeImage(img);
}

void linear(ImageData *img,ImageData *outimg){
    int x,y,i; //for文用
    int error; //エラーチェッカー(いらないかも)
    int max[3] = {}; //全要素0で初期化
    int min[3] = {255,255,255};
    Pixel pix; //r,g,b
    
    //最大・最小階調を調べる
    for(x=0;x<img->width;x++) {
        for(y=0;y<img->height;y++) {
            error = getPixel(img,x,y,&pix);
            if (error == -1){
                printf("getError(%d,%d)¥n",x,y);
                return;
            }
            if(max[0] < pix.r) {
                max[0] = pix.r;
            }
            if(max[1] < pix.g) {
                max[1] = pix.g;
            }
            if(max[2] < pix.b) {
                max[2] = pix.b;
            }
            if(min[0] > pix.r) {
                min[0] = pix.r;
            }
            if(min[1] > pix.g) {
                min[1] = pix.g;
            }
            if(min[2] > pix.b) {
                min[2] = pix.b;
            }
        }
    }

    for(x=0;x<img->width;x++) {
        for(y=0;y<img->height;y++) {
            error = getPixel(img,x,y,&pix);
            if (error == -1){
                printf("getError(%d,%d)¥n",x,y);
                return;
            }
            pix.r = (int)(((double)(pix.r - min[0]) / (max[0] - min[0])) * 255);
            pix.g = (int)(((double)(pix.g - min[1]) / (max[1] - min[1])) * 255);
            pix.b = (int)(((double)(pix.b - min[2]) / (max[2] - min[2])) * 255);
            error = setPixel(outimg,x,y,&pix);
            if (error == -1) {
                printf("setError(%d,%d)¥n",x,y);
                return;
            }
        }
    }
}
