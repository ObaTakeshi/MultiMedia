#include <stdio.h>
#include "image.h"

void make_mono_histgram();
void make_mix_histgram(ImageData *img, ImageData *histimg);
void linear(ImageData *img, ImageData *outimg);
void centerOfHistgram(ImageData *img, ImageData *imgout);

int main(){
    
    //画像ファイルのファイル名とパス指定
    char *fname = "./SAMPLE.bmp";
    char *linearname = "./linear.bmp";
    char *original_histname = "./original_hist.bmp";
    char *linear_histname = "./linear_hist.bmp";
    char *linear_mono_histname = "./linear_mono_hist.bmp";
    char *green = "./red.bmp";
    
    ImageData *img;
    ImageData *linearimg;
    ImageData *original_histimg;
    ImageData *linear_histimg;
    ImageData *linear_mono_histimg;
    ImageData *green_img;
    
    //画像の読み込み
    readBMPfile(fname,&img);
    
    //空画像の作成
    linearimg = createImage(img->width,img->height,img->depth);
    original_histimg = createImage(256, 256, img->depth);
    linear_histimg = createImage(256, 256, img->depth);
    linear_mono_histimg = createImage(256, 768, img->depth);
    green_img = createImage(img->width,img->height,img->depth);
    
    linear(img,linearimg);
    
    centerOfHistgram(img, green_img);
    make_mix_histgram(img,original_histimg);
    
    writeBMPfile(green, green_img);
    writeBMPfile(original_histname,original_histimg);
    writeBMPfile(linearname,linearimg);
    
    make_mix_histgram(linearimg, linear_histimg);
    make_mono_histgram(linearimg, linear_mono_histimg);
    
    writeBMPfile(linear_histname, linear_histimg);
    writeBMPfile(linear_mono_histname, linear_mono_histimg);
    
    //画像領域の解放(必要あるのか?)
    disposeImage(img);
    disposeImage(linearimg);
    disposeImage(original_histimg);
    disposeImage(linear_histimg);
    disposeImage(linear_mono_histimg);
}

void centerOfHistgram(ImageData *img, ImageData *imgout) {
    int i, l, x, y;
    int g_r = 0, g_g = 0, g_b = 0;
    long int hist[257][3] = {};
    Pixel pix;
    for(x=0;x<img->width;x++) {
        for(y=0;y<img->height;y++) {
            getPixel(img,x,y,&pix);
            hist[pix.r][0]++;
            hist[pix.g][1]++;
            hist[pix.b][2]++;
        }
    }
    
    for(i=0;i<256;i++) {
        for(l=0;l<3;l++) {
            hist[256][l] = hist[256][l] + hist[i][l] * (i+1);
        }
        g_r += hist[i][0];
        g_g += hist[i][1];
        g_b += hist[i][2];
    }
    
    g_r = (int)(hist[256][0] / g_r);
    g_g = (int)(hist[256][1] / g_g);
    g_b = (int)(hist[256][2] / g_b);
    
    printf("%d,%d,%d\n",g_r,g_g,g_b);
    
    for(x=0;x<img->width;x++) {
        for(y=0;y<img->height;y++) {
            getPixel(img,x,y,&pix);
            
            if(pix.g >= g_g || pix.b >= g_b) {
                pix.r = 255;
                pix.g = 255;
                pix.b = 255;
            }
            setPixel(imgout,x,y,&pix);
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
    //下記はmake_mono_histgram冒頭と重複する操作なので関数にまとめる(時間があったら)
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
    //--------------------------------------------------------------------
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
        
        //いまいちなやり方(時間があったら直す)
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
