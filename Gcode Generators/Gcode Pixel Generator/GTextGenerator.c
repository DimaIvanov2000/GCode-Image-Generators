#include <stdio.h>

int pixelSize=5;
FILE*fp;

int sprite[100][100]={0};
char file[100],oFile[100];
int xThick,yThick;
int x,y;
int feedRateXY=6000;
int feedRateZ=50;
int i,j,k;
char c;
int xCur=0;

/*
G21

G01 F15 Z0.5
G01 F500 X28 Y22
G01 F15 Z0
G01 F500 X81 Y18
*/

int main(void)
{
    printf("Enter a File Name (.txt):\n");
    scanf("%s",&file);
    fp=fopen(file,"r");
    c=fgetc(fp);
    xThick=0;
    yThick=0;
    while(c!=10&&c!=-1)
    {
        sprite[0][xThick]=c-48;
        xThick++;
        c=fgetc(fp);
    }
    yThick=1;
    if(c!=-1)
    {
        yThick++;
        xCur=0;
        c=fgetc(fp);
        while(c!=-1)
        {
            if(c==10)
            {
                yThick++;
                xCur=0;
            }
            else
            {
                sprite[yThick-1][xCur]=c-48;
                xCur++;
            }
            c=fgetc(fp);
        }
    }
    printf("\n\nImage: %i x %i  (l x h)\n\n",xThick,yThick);
    for(y=0;y<yThick;y++)
    {
        for(x=0;x<xThick;x++)
        {
            if(sprite[y][x]==0) printf("%c",177);
            if(sprite[y][x]==1) printf("%c",176);
            if(sprite[y][x]==2) printf(" ");
        }
        printf("\n");
    }
    fclose(fp);
    fflush(fp);
    printf("\n Making Gcode ...\n\n");
    strcat(oFile,file);
    printf("%s",oFile);
    for(i=99;i>=0;i--)
    {
        if(oFile[i]!=0)
        {
            oFile[i]=0;
            oFile[i-1]=0;
            oFile[i-2]=0;
            oFile[i-3]=0;
            break;
        }
    }
    strcat(oFile,"_Gcode.txt");
    fp=fopen(oFile,"w");
    fprintf(fp,"G21\n\nG01 F%i Z0.5\n",feedRateZ);
    for(y=0;y<yThick;y++)
    {
        for(x=0;x<xThick;x++)
        {
            if(sprite[y][x]>0)
            {
                fprintf(fp,"G01 F%i X%i Y%i\n",feedRateXY,x*pixelSize,y*pixelSize);
                fprintf(fp,"G01 F%i Z0\n",feedRateZ);
                fprintf(fp,"G01 F%i X%i Y%i\n",feedRateXY,(x+1)*pixelSize,y*pixelSize);
                fprintf(fp,"G01 F%i X%i Y%i\n",feedRateXY,(x+1)*pixelSize,(y+1)*pixelSize);
                fprintf(fp,"G01 F%i X%i Y%i\n",feedRateXY,x*pixelSize,(y+1)*pixelSize);
                fprintf(fp,"G01 F%i X%i Y%i\n",feedRateXY,x*pixelSize,y*pixelSize);
                if(sprite[y][x]==2)
                {
                    fprintf(fp,"G01 F%i X%i Y%i\n",feedRateXY,(x+1)*pixelSize,(y+1)*pixelSize);
                    fprintf(fp,"G01 F%i Z0.5\n",feedRateZ);
                    fprintf(fp,"G01 F%i X%i Y%i\n",feedRateXY,(x+1)*pixelSize,y*pixelSize);
                    fprintf(fp,"G01 F%i Z0\n",feedRateZ);
                    fprintf(fp,"G01 F%i X%i Y%i\n",feedRateXY,x*pixelSize,(y+1)*pixelSize);
                }
                fprintf(fp,"G01 F%i Z0.5\n",feedRateZ);
            }
        }
    }
    fclose(fp);
    fflush(fp);
    printf("Gcode Complete!\n");
}
