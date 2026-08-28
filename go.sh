#usage "./go.sh drop.c"

file=$1;
len=${#file};
file_out=${file:0:len-2}

gcc ${file} -o ${file_out} -I/usr/include/SDL2 -L/usr/include/SDL2/lib -lSDL2 -lm -O3 && ./${file_out}

