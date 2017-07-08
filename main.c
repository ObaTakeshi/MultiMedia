#include <stdio.h>
#include "image.h"

void make_mix_histgram(ImageData *img, ImageData *histimg);
void linear(ImageData *img, ImageData *outimg);

int main(){
    
    char *fname = "./SAMPLE.bmp";
    char *linearname = "./linear.bmp";
    char *original_histname = "./original_hist.bmp";
    char *linear_histname = "./linear_hist.bmp";
    
    ImageData *img;
    ImageData *linearimg;
    ImageData *original_histimg;
    ImageData *linear_histimg;
    
    readBMPfile(fname,&img);
    
    linearimg = createImage(img->width,img->height,img->depth);
    original_histimg = createImage(256, 256, img->depth);
    linear_histimg = createImage(256, 256, img->depth);
    
    linear(img,linearimg);
    
    make_mix_histgram(img,original_histimg);
    
    writeBMPfile(original_histname,original_histimg);
    writeBMPfile(linearname,linearimg);
    
    make_mix_histgram(linearimg, linear_histimg);
    
    writeBMPfile(linear_histname, linear_histimg);
    
    disposeImage(img);
    disposeImage(linearimg);
    disposeImage(original_histimg);
}

void make_mix_histgram(ImageData *img, ImageData *histimg) {
    int i,x,y;
    long int hist[256][3] = {};
    Pixel pix;
    
    for(x=0;x<img->width;x++){
        for(y=0;y<img->height;y++){
            getPixel(img,x,y,&pix);
            hist[pix.r][0]++;
            hist[pix.g][1]++;
            hist[pix.b][2]++;
        }
    }
    int l;
    int h = histimg->height;
    int max = 255;
    int takasa[3] = {};
    
    for(i=0;i<256;i++) {
        for(l=0;l<3;l++){
            if(max < hist[i][l]) {
                max = hist[i][l];
            }
        }
    }
    
    for(x=0;x<histimg->width;x++) {
        for(l=0;l<3;l++){
            takasa[l] = (int)(h / (double)max * hist[x][l]);
            
            if(takasa[l] > h) {
                takasa[l] = h;
            }
        }
        for(y=0;y<h;y++) {
            if(y < takasa[0] && y < takasa[1] && y < takasa[2]) {
                pix.r = 255 ;pix.g = 255; pix.b = 255;
            } else if(y < takasa[0] && y < takasa[1] && y >= takasa[2]) {
                pix.r = 255; pix.g = 255; pix.b = 0;
            } else if(y < takasa[0] && y >= takasa[1] && y < takasa[2]) {
                pix.r = 255; pix.g = 0; pix.b = 255;
            } else if(y < takasa[0] && y >= takasa[1] && y >= takasa[2]) {
                pix.r = 255; pix.g = 0; pix.b = 0;
            } else if(y >= takasa[0] && y < takasa[1] && y < takasa[2]){
                pix.r = 0; pix.g = 255; pix.b = 255;
            } else if(y >= takasa[0] && y < takasa[1] && y >= takasa[2]) {
                pix.r = 0; pix.g = 255; pix.b = 0;
            } else if(y >= takasa[0] && y >= takasa[1] && y < takasa[2]) {
                pix.r = 0; pix.g = 0; pix.b = 255;
            } else if(y >= takasa[0] && y >= takasa[1] && y >= takasa[2]) {
                pix.r = 0; pix.g = 0; pix.b = 0;
            }
            setPixel(histimg,x,h-1-y,&pix);
        }
    }
}

void linear(ImageData *img,ImageData *outimg) {
    int x,y,i; //for文用
    int max[3] = {}; //全要素0で初期化
    int min[3] = {255,255,255};
    Pixel pix; //r,g,b
    
    //最大・最小階調を調べる
    for(x=0;x<img->width;x++) {
        for(y=0;y<img->height;y++) {
            getPixel(img,x,y,&pix);
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
            getPixel(img,x,y,&pix);
            pix.r = (int)(((double)(pix.r - min[0]) / (max[0] - min[0])) * 255);
            pix.g = (int)(((double)(pix.g - min[1]) / (max[1] - min[1])) * 255);
            pix.b = (int)(((double)(pix.b - min[2]) / (max[2] - min[2])) * 255);
            setPixel(outimg,x,y,&pix);
        }
    }
}
