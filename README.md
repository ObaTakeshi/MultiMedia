# MultiMedia

コンパイル
$ gcc -o 2017multiA.c 2017multiA.c bmp.c image.c -lm

実行
./2017multiA

回転する角度は数字(°)を記入

出力画像

A-1.bmp = linear.bmp                線形変換した画像

A-2.bmp = original_hist.bmp         元の画像のヒストグラム(全色)

A-3.bmp = linear_hist.bmp           線形変換した画像のヒストグラム(全色)

A-4.bmp = linear_mono_hist.bmp      線形変換した画像のヒストグラム(RGB各色)

A-5.bmp = turn_nohole.bmp           線形変換した画像を回転させた画像
