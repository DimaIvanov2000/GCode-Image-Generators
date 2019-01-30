#include <stdio.h>
#include <math.h>

//Variables
float unitSize=0;
float fineness;
float slope;
float feedRateXY=0;
float feedRateZ=0;
float distUp=0;
int labelLengths[4]={13,11,17,10};
float *setupVars[4]={&feedRateXY,&feedRateZ,&distUp,&unitSize};
char setupNames[4][30]={"X/Y FeedRate","Z FeedRate","Lift-up Distance","Unit Size (mm)"};
float furthestX;
float bwSlope;

float roundingError=0.001;

int main(void)
{

    //Important Storage
    float points[100][2]={0};

    //Temps
    FILE*fp;
    FILE*nfp;
    char c;
    int numPoints=0;
    char file[100]={0};
    char oFile[100]={0};
    int numPointsUsing=0;
    float curNum=0;
    int curDec=0;
    int pointsUsing[100]={0};
    int i,j,k;
    char shapeType;
    int numTransitions;
    int exit;
    float transitionPoints[100][2];
    float lineX;


    printf("Enter a File Name (.txt):\n");
    scanf("%s",&file);
    fp=fopen(file,"r");
    printf("\n Making Gcode ...\n\n");
    strcat(oFile,file);
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
    nfp=fopen(oFile,"w");
    printf("Creating: %s\n",oFile);

    //Make the darned thing lol v v v
    //Add G21
    fprintf(nfp,"G21\n\n");

    //Adjusting constants at the beginning
    for(i=0;i<4;i++)
    {
        for(j=0;j<labelLengths[i];j++)
        {
            fgetc(fp);
        }
        c=fgetc(fp);
        while(c!=10&&c!='.')
        {
            *setupVars[i]*=10;
            *setupVars[i]+=c-48;
            c=fgetc(fp);
        }
        if(c=='.')
        {
            curDec=1;
            c=fgetc(fp);
            while(c!=10)
            {
                *setupVars[i]+=(c-48)*(float)pow(10,-curDec);
                curDec++;
                c=fgetc(fp);
            }
        }
        printf("\n%s: %f",setupNames[i],*setupVars[i]);
    }

    //Variable Code-Reading
    c=fgetc(fp);
    while(c!=-1)
    {
        //If you found a point
        if(c=='(')
        {
            curNum=0;
            c=fgetc(fp);
            while(c!=','&&c!='.')
            {
                curNum=(curNum*10)+c-48;
                c=fgetc(fp);
            }
            if(c=='.')
            {
                curDec=1;
                c=fgetc(fp);
                while(c!=',')
                {
                    curNum+=(c-48)*(float)pow(10,-curDec);
                    curDec++;
                    c=fgetc(fp);
                }
            }
            points[numPoints][0]=curNum*unitSize;
            curNum=0;
            c=fgetc(fp);
            while(c!=')'&&c!='.')
            {
                curNum=(curNum*10)+c-48;
                c=fgetc(fp);
            }
            if(c=='.')
            {
                curDec=1;
                c=fgetc(fp);
                while(c!=')')
                {
                    curNum+=(c-48)*(float)pow(10,-curDec);
                    curDec++;
                    c=fgetc(fp);
                }
            }
            points[numPoints][1]=curNum*unitSize;
            printf("\nPoint %i: (%f,%f)",numPoints+1,points[numPoints][0],points[numPoints][1]);
            numPoints++;
        }

        //If you found a shapey boi
        if(c=='S')
        {
            c=fgetc(fp);
            shapeType=c;

            pointsUsing[0]=0;
            numPointsUsing=0;
            fgetc(fp);
            c=0;
            while(c!=' ')
            {
                c=fgetc(fp);
                while(c!=','&&c!=' ')
                {
                    pointsUsing[numPointsUsing]*=10;
                    pointsUsing[numPointsUsing]+=c-48;
                    c=fgetc(fp);
                }
                //printf("\n%i",pointsUsing[numPointsUsing]);
                numPointsUsing++;
                pointsUsing[numPointsUsing]=0;
            }
            c=fgetc(fp);
            curNum=0;
            while(c!=' '&&c!='.')
            {
                curNum=(curNum*10)+c-48;
                c=fgetc(fp);
            }
            if(c=='.')
            {
                curDec=1;
                c=fgetc(fp);
                while(c!=' ')
                {
                    curNum+=(c-48)*(float)pow(10,-curDec);
                    curDec++;
                    c=fgetc(fp);
                }
            }
            slope=curNum;
            c=fgetc(fp);
            curNum=0;
            while(c!=10&&c!=-1&&c!='.')
            {
                curNum=(curNum*10)+c-48;
                c=fgetc(fp);
            }
            if(c=='.')
            {
                curDec=1;
                c=fgetc(fp);
                while(c!=10&&c!=-1)
                {
                    curNum+=(c-48)*(float)pow(10,-curDec);
                    curDec++;
                    c=fgetc(fp);
                }
            }
            fineness=curNum;
            printf("\nS/F: %f,%f",slope,fineness);
            if(shapeType=='P'||shapeType=='A')
            {
                fprintf(nfp,"G01 F%f Z%f\n",feedRateZ,distUp);
                fprintf(nfp,"G01 F%f X%f Y%f\n",feedRateXY,points[pointsUsing[0]-1][0],points[pointsUsing[0]-1][1]);
                fprintf(nfp,"G01 F%f Z0\n",feedRateZ);
                for(i=0;i<numPointsUsing;i++)
                {
                    fprintf(nfp,"G01 F%f X%f Y%f\n",feedRateXY,points[pointsUsing[(i+1)%numPointsUsing]-1][0],points[pointsUsing[(i+1)%numPointsUsing]-1][1]);
                }
                //fprintf()
            }

            printf("%i",c);

            if(shapeType=='F'||shapeType=='A')
            {
                furthestX=points[pointsUsing[0]-1][0]-(points[pointsUsing[0]-1][1]/slope);
                //printf("\n%f",furthestX);
                for(i=1;i<numPointsUsing;i++)
                {
                    curNum=points[pointsUsing[i]-1][0]-(points[pointsUsing[i]-1][1]/slope);
                    if(curNum<furthestX)
                    {
                        furthestX=curNum;
                    }
                }
                printf("\nfurth x:%f",furthestX);
                lineX=furthestX;
                exit=0;
                numTransitions=0;
                while(exit==0)
                {
                    //printf("\n numpointsdude %i",numPointsUsing);
                    exit=1;
                    numTransitions=0;
                    for(i=0;i<numPointsUsing;i++)
                    {
                        //printf("\n%f",(points[pointsUsing[i]-1][1]-(slope*(points[pointsUsing[i]-1][0]-lineX)))*(points[pointsUsing[(i+1)%numPointsUsing]-1][1]-(slope*(points[pointsUsing[(i+1)%numPointsUsing]-1][0]-lineX))));
                        //printf("\nAAA %f",fabsf(points[pointsUsing[i]-1][1]-(slope*(points[pointsUsing[i]-1][0]-lineX))));
                        ///*
                        if(fabsf(points[pointsUsing[i]-1][1]-(slope*(points[pointsUsing[i]-1][0]-lineX)))<roundingError
                           ||fabsf(points[pointsUsing[(i+1)%numPointsUsing]-1][1]-(slope*(points[pointsUsing[(i+1)%numPointsUsing]-1][0]-lineX)))<roundingError
                        )
                        {

                                printf("\nFlat out point intersection");
                            exit=0;
                        }
                        //*/
                        else if((points[pointsUsing[i]-1][1]-(slope*(points[pointsUsing[i]-1][0]-lineX)))*(points[pointsUsing[(i+1)%numPointsUsing]-1][1]-(slope*(points[pointsUsing[(i+1)%numPointsUsing]-1][0]-lineX)))<0)
                        {
                            if(points[pointsUsing[i]-1][0]==points[pointsUsing[(i+1)%numPointsUsing]-1][0])
                            {
                                transitionPoints[numTransitions][0]=points[pointsUsing[i]-1][0];
                                transitionPoints[numTransitions][1]=slope*(points[pointsUsing[i]-1][0]-lineX);
                            }
                            else
                            {
                                //printf("\nslope b/w points: %f",((points[pointsUsing[(i+1)%numPointsUsing]-1][1]-points[pointsUsing[i]-1][1])/(points[pointsUsing[(i+1)%numPointsUsing]-1][0]-points[pointsUsing[i]-1][0])));
                                bwSlope=((points[pointsUsing[(i+1)%numPointsUsing]-1][1]-points[pointsUsing[i]-1][1])/(points[pointsUsing[(i+1)%numPointsUsing]-1][0]-points[pointsUsing[i]-1][0]));
                                transitionPoints[numTransitions][0]=((slope*lineX)-(bwSlope*points[pointsUsing[i]-1][0])+points[pointsUsing[i]-1][1])/(slope-bwSlope);
                                //transitionPoints[numTransitions][0]=((slope*lineX)-(((points[pointsUsing[(i+1)%numPointsUsing]-1][1]-points[pointsUsing[i]-1][1])/(points[pointsUsing[(i+1)%numPointsUsing]-1][0]-points[pointsUsing[i]-1][0]))*points[pointsUsing[i]-1][0])+points[pointsUsing[i]-1][1])/(slope-((points[pointsUsing[(i+1)%numPointsUsing]-1][1]-points[pointsUsing[i]-1][1])/(points[pointsUsing[(i+1)%numPointsUsing]-1][0]-points[pointsUsing[i]-1][0])));
                                transitionPoints[numTransitions][1]=slope*(transitionPoints[numTransitions][0]-lineX);
                            }
                            printf("\nLine Int #%i-%i: (%f,%f)",i,i+1,transitionPoints[numTransitions][0],transitionPoints[numTransitions][1]);
                            numTransitions++;
                            exit=0;
                        }
                    }
                    //printf("\n%i",numTransitions);

                    for(i=0;i<numTransitions-1;i++)
                    {
                        for(j=0;j<numTransitions-1;j++)
                        {
                            if(transitionPoints[j][1]>transitionPoints[j+1][1])
                            {
                                curNum=transitionPoints[j+1][1];
                                transitionPoints[j+1][1]=transitionPoints[j][1];
                                transitionPoints[j][1]=curNum;
                                curNum=transitionPoints[j+1][0];
                                transitionPoints[j+1][0]=transitionPoints[j][0];
                                transitionPoints[j][0]=curNum;
                            }
                        }
                    }
                    printf("\nNEW LINE!");
                    for(i=0;i<numTransitions;i++)
                    {
                        //printf("\nint: %f,%f",transitionPoints[i][0],transitionPoints[i][1]);
                    }

                    for(i=0;i<numTransitions/2;i++)
                    {
                        fprintf(nfp,"G01 F%f Z%f\n",feedRateZ,distUp);
                        fprintf(nfp,"G01 F%f X%f Y%f\n",feedRateXY,transitionPoints[i*2][0],transitionPoints[i*2][1]);
                        fprintf(nfp,"G01 F%f Z0\n",feedRateZ);
                        fprintf(nfp,"G01 F%f X%f Y%f\n",feedRateXY,transitionPoints[(i*2)+1][0],transitionPoints[(i*2)+1][1]);
                    }
                    lineX+=fineness;
                }
            }
        }

        c=fgetc(fp);
    }

    fclose(fp);
    fflush(fp);
    fclose(nfp);
    fflush(nfp);
}
