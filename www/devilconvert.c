//gcc -o devilconvert -lIL -lILU devilconvert.c
#include <IL/il.h>
#include <IL/ilu.h>

int main(int argc, char *argv[]) {
    int i;
    if (argc != 3)
        exit(1);

    ilInit();
    iluInit();
    ilBindImage(1);

    ilLoadImage(argv[1]);
    i = ilGetError();
    if(i != IL_NO_ERROR) {
        printf("load error : %s\n",iluErrorString(i));
        exit(1);
    }
        
    ilSaveImage(argv[2]);
    i = ilGetError();
    if(i != IL_NO_ERROR) {
        printf("save error : %s\n",iluErrorString(i));
        exit(1);
    }
}
