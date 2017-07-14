#include <stdio.h>
#include "image.h"
#include <math.h>

void turn(ImageData *img, ImageData *outimg, int theta);
void make_mono_histgram();
void make_mix_histgram(ImageData *img, ImageData *histimg);
void linear(ImageData *img, ImageData *outimg);

int main(){
    
    //画像ファイルのファイル名とパス指定
    char *fname = "./SAMPLE.bmp";
    char *linearname = "A-1.bmp";           //"./linear.bmp";
    char *original_histname = "A-2.bmp";    //"./original_hist.bmp";
    char *linear_histname = "A-3.bmp";      //"./linear_hist.bmp";
    char *linear_mono_histname = "A-4.bmp"; //"./linear_mono_hist.bmp";
    char *turn_name = "A-5.bmp";            //"./turn_nohole.bmp";
    
    ImageData *img;
    ImageData *linearimg;
    ImageData *original_histimg;
    ImageData *linear_histimg;
    ImageData *linear_mono_histimg;
    ImageData *turnimg;

    //元画像の読み込み
    readBMPfile(fname,&img);

    //回転に必要
    int size,theta;
    size = (int)(sqrt(img->width*img->width + img->height*img->height));

    //空画像の作成
    linearimg = createImage(img->width,img->height,img->depth);
    original_histimg = createImage(256, 256, img->depth);
    linear_histimg = createImage(256, 256, img->depth);
    linear_mono_histimg = createImage(256, 768, img->depth);
    turnimg = createImage(size,size,img->depth);
    linear(img,linearimg);
    
    //元画像のヒストグラム
    make_mix_histgram(img,original_histimg);
    
    //画像の出力
    writeBMPfile(original_histname,original_histimg);
    writeBMPfile(linearname,linearimg);
    
    //変換後のヒストグラム
    make_mix_histgram(linearimg, linear_histimg);
    make_mono_histgram(linearimg, linear_mono_histimg);
    
    writeBMPfile(linear_histname, linear_histimg);
    writeBMPfile(linear_mono_histname, linear_mono_histimg);
    
    //画像の回転
    printf("線形変化した画像が回転する角度を入力してください(°)>>");
    scanf("%d", &theta);
    turn(linearimg,turnimg, theta);
    writeBMPfile(turn_name,turnimg);
    
    //画像領域の解放
    disposeImage(img);
    disposeImage(linearimg);
    disposeImage(original_histimg);
    disposeImage(linear_histimg);
    disposeImage(linear_mono_histimg);
    disposeImage(turnimg);
}

void turn(ImageData *img,ImageData *outimg, int theta){
    int i, j, x, y;
    int x1_center = (int)(img->width/2), y1_center = (int)(img->height/2);
    int x2_center = (int)(outimg->width/2), y2_center = (int)(outimg->height/2);
    int x_move = x2_center-x1_center, y_move = y2_center-y1_center;
    double rad = ((double)theta/180)*M_PI;

    Pixel pix;
    Pixel black;
    black.r = 0; black.g = 0; black.b = 0;
    
    ImageData *tempimg;
    tempimg = createImage(outimg->width, outimg->height, outimg->depth);
    for(i=0; i<img->width; i++){
        for(j=0; j<img->height; j++){
            getPixel(img, i, j, &pix);
            setPixel(tempimg, i+x_move, j+y_move, &pix);
        }
    }

    for(i=0;i<outimg->height;i++){
        for(j=0;j<outimg->width;j++){
            x = (int)((j-x2_center)*cos(rad)-(i-y2_center)*sin(rad)+x1_center);
            y = (int)((j-x2_center)*sin(rad)+(i-y2_center)*cos(rad)+y1_center);
            if(x>=0 && x<img->width && y>=0 && y<img->height){
                getPixel(img, x, y, &pix);
                setPixel(outimg,j,i,&pix);
            } else {
                setPixel(outimg,j,i,&black);
            }
        }
    }
}


//各色のヒストグラムの出力
void make_mono_histgram(ImageData *img, ImageData *histimg) {
    int i, x, y;
    long int hist[256][3] = {};
    Pixel pix;
    
    for(x=0;x<img->width;x++) {
        for(y=0;y<img->height;y++) {
            getPixel(img,x,y,&pix);
            hist[pix.r][0]++;
            hist[pix.g][1]++;
            hist[pix.b][2]++;
        }
    }
    int l;
    int h = histimg->height;
    int max[3] = {255, 255, 255};
    int takasa[3];
    
    for(i=0;i<256;i++) {
        for(l=0;l<3;l++) {
            if(max[l] < hist[i][l]) {
                max[l] = hist[i][l];
            }
        }
    }
    
    for(x=0;x<histimg->width;x++) {
        for(l=0;l<3;l++) {
            h = histimg->height / 3;
            takasa[l] = (int)(h / (double)max[l] * hist[x][l]);
            
            if(takasa[l] > h) {
                takasa[l] = h;
            }
            
            //いまいちなやり方(時間があったら直す)
            for(y=0;y<h;y++) {
                if(l == 0) {
                    if(y < takasa[l]) {
                        pix.r = 255; pix.g = 0; pix.b = 0;
                    } else {
                        pix.r = 0; pix.g = 0; pix.b = 0;
                    }
                    
                } else if(l == 1) {
                    if(y < takasa[l]) {
                        pix.r = 0; pix.g = 255; pix.b = 0;
                    } else {
                        pix.r = 0; pix.g = 0; pix.b = 0;
                    }
                    
                }else if(l == 2) {
                    if(y < takasa[l]) {
                        pix.r = 0; pix.g = 0; pix.b = 255;
                    } else {
                        pix.r = 0; pix.g = 0; pix.b = 0;
                    }
                }
                setPixel(histimg,x,h+l*256-1-y,&pix);
            }
        }
    }
}

//全色のヒストグラム出力
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
        for(l=0;l<3;l++) {
            if(max < hist[i][l]) {
                max = hist[i][l];
            }
        }
    }
    
    for(x=0;x<histimg->width;x++) {
        for(l=0;l<3;l++) {
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
            } else if(y >= takasa[0] && y < takasa[1] && y < takasa[2]) {
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

//線形変換
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
