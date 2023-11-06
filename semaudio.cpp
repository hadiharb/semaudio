//
//  main.cpp
//  audioClassifier
//
//  Ghanni
//
//Copyright (c) 2012 Ghanni. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <vector>
#include "bcport.h"
#include "GSignalProcessor.h"
#include "HHNN.h"

using namespace std;

//#include "FPExtract.h"
//#include "HHNN.h"

int Min(float *values,int offset)
{
    float minn=values[0];
    int index=0;
    for(int i=0;i<offset;i++)
    {
        if(values[i]<minn){minn=values[i];index=i;}
    }
    return index;
}

float compare(GFVExtractor *s1, GFVExtractor **ss,int offset, int numinst, int &winner) // s1 is the query, ss an array of references, offset is the sample index in the query, numinst is the number of classes (references)
{
    float a,b,c,kl,dista=0,mindist=1000000,bufD[10000];
    int disti=0,mindisti=-1;
    a=b=c=kl=0;
    
    
    for (int n = 0; n < numinst; n++)
    {
        // bufD = new float(ss[n].N);
        for (int WI = 0; WI < ss[n]->get_cntfbuf(); WI++)
        {
            for (int k=1;k<GF_PASSFREQ+1;k++)
            {
                a+=pow(s1->m_featbuf[offset].var[k-1],2);
                b+=pow(ss[n]->m_featbuf[WI].var[k-1],2);
                c+=pow(s1->m_featbuf[offset].mean[k-1]-ss[n]->m_featbuf[WI].mean[k-1],2);
            }
            kl=a/(b+0.0000000001)+b/(a+0.0000000001)+c*(1/(a+0.0000000001)+1/(b+0.0000000001));
            bufD[WI] = kl;
        }
        disti = Min(bufD, ss[n]->get_cntfbuf());
        dista = bufD[disti];
        
        if (dista<mindist) {
            mindist = dista;
            mindisti = n;
        }
        
        //delete [] bufD;
    }
    winner = mindisti;
    return mindist;
}

void wavAppend(char *f1, char *f2)
{
    
    unsigned char header[44];
    unsigned char buffer;
    unsigned int size1=0;
    int offset1 = 0;
    long totell, len;
    
    FILE *pFile;
    FILE *pFile1;
    
    pFile=fopen(f1, "r+");
    totell = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    len = ftell(pFile) - totell;
    fseek(pFile, 0, SEEK_SET);
    fread(header,1,44,pFile);
    //nbchannel=header[22];
    //bitres=header[34];
    //freq=header[24]+256*header[25];
    //size=16777216*header[7]+65536*header[6]+256*header[5]+header[4];
    //len=16777216*header[43]+65536*header[42]+256*header[41]+header[40];

    unsigned int buf=0;
    int dp1=0;
    for(int i=0;i<len;i++)
    {
        fseek(pFile,i,0);
        fread(&buf,1,4,pFile);
        if (buf=='atad') {
            dp1=i;
            break;
        }
    }
    offset1 = dp1;
    if (dp1<len-4) {
        fseek(pFile,dp1+4,0);
        fread(&buf,1,4,pFile);
        //len=16777216*buf[3]+65536*buf[2]+256*buf[1]+buf[0];
    }
    if (buf+dp1+4+4<len+1) {
        fseek(pFile,buf+dp1+4+4,0);
        //fread(&buf,1,4,pFile);
        //len=16777216*buf[3]+65536*buf[2]+256*buf[1]+buf[0];
    }
    //buffer = new unsigned char[buf];
    size1 = buf;
    
    pFile1=fopen(f2, "r");
    totell = ftell(pFile1);
    fseek(pFile1, 0, SEEK_END);
    len = ftell(pFile1) - totell;
    fseek(pFile1, 0, SEEK_SET);
    
    buf=0;
    dp1=0;
    for(int i=0;i<len;i++)
    {
        fseek(pFile1,i,0);
        fread(&buf,1,4,pFile1);
        if (buf=='atad') {
            dp1=i;
            break;
        }
    }
    if (dp1<len-4) {
        fseek(pFile1,dp1+4,0);
        fread(&buf,1,4,pFile1);
        for (int i=0; i<buf; i++) {
            fread(&buffer,1,1,pFile1);
            fwrite(&buffer,1,1,pFile);
        }
        //len=16777216*buf[3]+65536*buf[2]+256*buf[1]+buf[0];
    }
    fseek(pFile,offset1+4,0);
    size1+=buf;
    fwrite(&size1,1,4,pFile);
    fseek(pFile,4,0);
    size1+=offset1;
    fwrite(&size1,1,4,pFile);
    
    fclose(pFile);
    fclose(pFile1);
    //return buf;
}

int generateSingleWav(char * wavdir)
{
    DIR *dp;
    AnsiString ft;
    AnsiString listwavs[2000];
    struct dirent *dirp;
    if ((dp = opendir((char*)wavdir)) == NULL)
    {
        printf("ERR\nError reading directory\n");
        pthread_exit(NULL);
        return 252;
    }
    
    int CL = 0;
    
    while ((dirp = readdir(dp)) != NULL)
    {
        int l = strlen(dirp->d_name);
        if (l>4)
        {
            //if (strcmp(dirp->d_name+(l-4),".hnn")==0)
            //    listhnns.push_back(string(dirp->d_name));
            if (strcmp(dirp->d_name+(l-4),".wav")==0) {
                listwavs[CL] = AnsiString(wavdir)+"/"+AnsiString(dirp->d_name);
                ft = dirp->d_name;
                CL++;
            }
            
        }
    }
    closedir(dp);
    if (CL<2) {
        return 0;
    }
    for (int i=1; i<CL; i++) {
        wavAppend(listwavs[0].c_str(), listwavs[i].c_str());
    }
    return 1;
}


void mixFiles(char *f1, char *f2, char *f)
{
    char nbchannel, bitres;
    int freq, size, size2, l;
    unsigned char header[44];
    unsigned char header2[44];
    unsigned char header3[44];
    long totell, len, len2;
    
    
    FILE *pFile;
    FILE *pFile2;
    FILE *pFile3;
    char buffer[2560000];
    //char buffer2[2560000];
    
    pFile=fopen(f1, "r");
    totell = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    len = ftell(pFile) - totell;
    fseek(pFile, 0, SEEK_SET);

    
    pFile2=fopen(f2, "r");
    totell = ftell(pFile2);
    fseek(pFile2, 0, SEEK_END);
    len2 = ftell(pFile2) - totell;
    fseek(pFile2, 0, SEEK_SET);
    
    pFile3=fopen(f, "w+");
    if(pFile==NULL) {
        perror("Error opening file.");
    }
    else {
        fread(header,1,44,pFile);
        nbchannel=header[22];
        bitres=header[34];
        freq=header[24]+256*header[25];
        size=16777216*header[7]+65536*header[6]+256*header[5]+header[4];
        len=16777216*header[43]+65536*header[42]+256*header[41]+header[40];
        //size=size-44;
        l = int(size/2);
        //fread(buffer,1,len-4200,pFile);
        
        fread(header2,1,44,pFile2);
        nbchannel=header2[22];
        bitres=header2[34];
        freq=header2[24]+256*header2[25];
        size2=16777216*header2[7]+65536*header2[6]+256*header2[5]+header2[4];
        //size2=size2-44;
        l = int(size2/2);
        //fread(buffer2,1,len2-4200,pFile2);
        
        for (int i = 0; i<44; i++) {
            header3[i]=header2[i];
        }
        
        header3[7] = (size+size2+8-8400)/16777216;
        header3[6] = ((size+size2+8-8400)-16777216*header3[7])/65536;
        header3[5] = ((size+size2+8-8400)-16777216*header3[7]-65536*header3[6])/256;
        header3[4] = ((size+size2+8-8400)-16777216*header3[7]-65536*header3[6]+256*header3[5]);
        
        header3[43] = (size+size2-28-8400)/16777216;
        header3[42] = ((size+size2-28-8400)-16777216*header3[43])/65536;
        header3[41] = ((size+size2-28-8400)-16777216*header3[43]-65536*header3[42])/256;
        header3[40] = ((size+size2-28-8400)-16777216*header3[43]-65536*header3[42]+256*header3[41]);
/*
        header[19] = (size+size2+44+44)/16777216;
        header[18] = ((size+size2+44+44)-16777216*header[19])/65536;
        header[17] = ((size+size2+44+44)-16777216*header[19]-65536*header[18])/256;
        header[16] = ((size+size2+44+44)-16777216*header[19]-65536*header[18]+256*header[17]);

        l = int(header[7]);
        l = int(header[6]);
        l = int(header[5]);
        l = int(header[4]);
        
        header[43] = (size+size2+44+44)/16777216;
        header[42] = ((size+size2+44+44)-16777216*header[43])/65536;
        header[41] = ((size+size2+44+44)-16777216*header[43]-65536*header[42])/256;
        header[40] = ((size+size2+44+44)-16777216*header[43]-65536*header[42]+256*header[41]);
        
   */
        fwrite(header3,1,44,pFile3);
        fread(buffer,1,size,pFile);
        fwrite(buffer, 1, size-4200, pFile3);
        fread(buffer,1,size2,pFile2);
        fwrite(buffer, 1, size2-4200, pFile3);
        
        /*
       while(fgets(buffer, sizeof(buffer), pFile)) {
            fwrite(buffer,1, sizeof(buffer),pFile3);
        }
    
       while(fgets(buffer, sizeof(buffer), pFile2)) {
            fprintf(pFile3, "%s", buffer);
        }
         */
    }
    fclose(pFile);
    fclose(pFile2);
    fclose(pFile3);
}


int main(int argc, const char * argv[])
{
    argc=3;
    argv[1]="train";
    argv[1]="classify";
    argv[2]="/Users/hadiharb/hadi/audiotag/train6/emotion_hadi/fromiPhone/test_big";
    bool verbose = true;
    
    /*
    AnsiString wav1 = "/Users/hadiharb/hadi/audiotag/train6/trainm/vbtrain/2 labs barking-01.wav";
    AnsiString wav2 = "/Users/hadiharb/hadi/audiotag/train6/trainm/vbtrain/2 labs barking-02.wav";
    AnsiString wav3 = "/Users/hadiharb/hadi/audiotag/train6/gender/female";
    
    generateSingleWav(wav3.c_str());
   
    //mixFiles(wav3.c_str(),wav2.c_str(),wav3.c_str());
     */
    AnsiString f1,f2,f3,f4,f5,f6,f1fp,f2fp,f3fp,f4fp,f5fp,f6fp, ft;
    
    AnsiString listwavs[GF_MAXCLASSES];
    AnsiString listcnames[GF_MAXCLASSES];
    AnsiString listfnames[GF_MAXCLASSES];
    
    f1fp = AnsiString(argv[2])+"/m1.nnf";
    f2fp = AnsiString(argv[2])+"/m2.nnf";
    //f2fp = AnsiString(argv[2])+"/m3_1.nnf";
    f3fp = AnsiString(argv[2])+"/m3.nnf";
    f4fp = AnsiString(argv[2])+"/m4.nnf";
    f5fp = AnsiString(argv[2])+"/m5.nnf";
    f6fp = AnsiString(argv[2])+"/m6.nnf";
    //f4fp = AnsiString(argv[2])+"/m3_2.nnf";
    /*
    
    AnsiString f1fp2, f2fp2, f3fp2, f4fp2;
    AnsiString f1fp3, f2fp3, f3fp3, f4fp3;
    AnsiString f1fp4, f2fp4, f3fp4, f4fp4;

    f1fp2 = AnsiString(argv[2])+"/m12.nnf";
    f2fp2 = AnsiString(argv[2])+"/m22.nnf";
    f3fp2 = AnsiString(argv[2])+"/m32.nnf";
    f4fp2 = AnsiString(argv[2])+"/m42.nnf";
    
    f1fp3 = AnsiString(argv[2])+"/m13.nnf";
    f2fp3 = AnsiString(argv[2])+"/m23.nnf";
    f3fp3 = AnsiString(argv[2])+"/m33.nnf";
    f4fp3 = AnsiString(argv[2])+"/m43.nnf";
    
    f1fp4 = AnsiString(argv[2])+"/m14.nnf";
    f2fp4 = AnsiString(argv[2])+"/m24.nnf";
    f3fp4 = AnsiString(argv[2])+"/m34.nnf";
    f4fp4 = AnsiString(argv[2])+"/m44.nnf";
    */
    
    if (argc<3)
    {
        printf("ERR\nMissing parameters\n Usage:\n semaudio train PATH \n semaudio classify PATH\n");
        pthread_exit(NULL);
        return 254;
    }
    
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir((char*)argv[2])) == NULL)
    {
        printf("ERR\nError reading directory\n");
        pthread_exit(NULL);
        return 252;
    }
    
    int CL = 0;
    
    while ((dirp = readdir(dp)) != NULL)
    {
        int l = strlen(dirp->d_name);
        if (l>4)
        {
            //if (strcmp(dirp->d_name+(l-4),".hnn")==0)
            //    listhnns.push_back(string(dirp->d_name));
            if (strcmp(dirp->d_name+(l-4),".wav")==0) {
                listwavs[CL] = AnsiString(argv[2])+"/"+AnsiString(dirp->d_name);
                ft = dirp->d_name;
                listcnames[CL] = ft.SubString(1, l-4);
                listfnames[CL] = listcnames[CL];
                CL++;
            }
            
        }
    }
    closedir(dp);
    

    
    //std::cout << "\n";
    
    //GFVExtractor* s1;
    GFVExtractor* ss[GF_MAXCLASSES];

    //s1 = new GFVExtractor(100, 100, 0.2f);
    
    int minoffset = GF_MAXSAMPLES;
    for (int i=0; i<CL; i++) {
        ss[i] = new GFVExtractor(GF_ITW_SIZE, GF_ITW_FRAC, 0.2f);
        ss[i]->TakeWavFile(listwavs[i].c_str());
        if (ss[i]->get_cntfbuf()<minoffset) {
            minoffset = ss[i]->get_cntfbuf();
        }
    }
    
    HHNN *nn1, *nn2, *nn3, *nn4, *nn5, *nn6;

    
    NNoutput *n1, *n2, *n3, *n4, *n5, *n6, *na;

    
    nn1=new HHNN(38,150,CL);
    nn2= new HHNN(38,150,CL);
    nn3= new HHNN(38,150,CL);
    nn4= new HHNN(38,150,CL);
    nn5= new HHNN(38,150,CL);
    nn6= new HHNN(38,150,CL);
    /*
     HHNN *nn12, *nn22, *nn32, *nn42;
     HHNN *nn13, *nn23, *nn33, *nn43;
     HHNN *nn14, *nn24, *nn34, *nn44;
     NNoutput *na1, *na2, *na3, *na4;
    nn12=new HHNN(38,150,CL);
    nn22= new HHNN(38,150,CL);
    nn32= new HHNN(38,250,CL);
    nn42= new HHNN(38,150,CL);
    
    nn13=new HHNN(38,150,CL);
    nn23= new HHNN(38,150,CL);
    nn33= new HHNN(38,250,CL);
    nn43= new HHNN(38,150,CL);
    
    nn14=new HHNN(38,150,CL);
    nn24= new HHNN(38,150,CL);
    nn34= new HHNN(38,250,CL);
    nn44= new HHNN(38,150,CL);
    */
    
    if (strcmp(argv[1],"train")==0) {
        /*
        nn1->initlearn(minoffset, CL, ss,listcnames, TYPE_MFCC);
        for(int i=0;i<9000;i++){
            float re=nn1->start(0.1);
            std::cout<<re<<" "<<i<<"\n";
            if(re<0.007)break;
        }
        nn1->save(f1fp.c_str());
        */
        
        nn2->initlearn(minoffset, CL, ss,listcnames, TYPE_MFSC);
        for(int i=0;i<9000;i++){
            float re=nn2->start(0.1);
            std::cout<<re<<" "<<i<<"\n";
            if(re<0.007)break;
        }
        nn2->save(f2fp.c_str());
        
        
        nn3->initlearn(minoffset, CL, ss,listcnames, TYPE_PGM);
        for(int i=0;i<9000;i++){
            float re=nn3->start(0.1);
            std::cout<<re<<" "<<i<<"\n";
            if(re<0.007)break;
        }
        nn3->save(f3fp.c_str());
        
        /*
        nn4->initlearn(minoffset, CL, ss,listcnames, TYPE_MEL);
        for(int i=0;i<9000;i++){
            float re=nn4->start(0.1);
            std::cout<<re<<" "<<i<<"\n";
            if(re<0.007)break;
        }
        nn4->save(f4fp.c_str());
*/
       
        nn5->initlearn(minoffset, CL, ss,listcnames, TYPE_ALL);
        for(int i=0;i<9000;i++){
            float re=nn5->start(0.3);
            std::cout<<re<<" "<<i<<"\n";
            if(re<0.007)break;
        }
        nn5->save(f5fp.c_str());
        /*
        nn6->initlearn(minoffset, CL, ss,listcnames, TYPE_SCBA);
        for(int i=0;i<9000;i++){
            float re=nn6->start(0.3);
            std::cout<<re<<" "<<i<<"\n";
            if(re<0.007)break;
        }
        nn6->save(f6fp.c_str());
     */
        
    }
    
    if (strcmp(argv[1],"classify")==0) {
        nn1->open(f1fp.c_str());
        nn2->open(f2fp.c_str());
        nn3->open(f3fp.c_str());
        nn4->open(f4fp.c_str());
        nn5->open(f5fp.c_str());
        nn6->open(f6fp.c_str());
        /*
        nn12->open(f1fp2.c_str());
        nn22->open(f2fp2.c_str());
        nn32->open(f3fp2.c_str());
        nn42->open(f4fp2.c_str());
        
        nn13->open(f1fp3.c_str());
        nn23->open(f2fp3.c_str());
        nn33->open(f3fp3.c_str());
        nn43->open(f4fp3.c_str());
        
        nn14->open(f1fp4.c_str());
        nn24->open(f2fp4.c_str());
        nn34->open(f3fp4.c_str());
        nn44->open(f4fp4.c_str());
        */

        for (int c=0; c<nn3->nclasses(); c++) {
            listcnames[c] = nn3->getClassName(c);
        }
        int allcounts[GF_MAXCLASSES];
        float allproba[GF_MAXCLASSES];
        int allhits = 0;
        int hits = 0;
        int all = 0;
        AnsiString txtout="",txtdecision="";
        int prev = -1;
        int prev2 = -2;
        int prev3 = -3;
        int current = 0;
        int smoothed = 0;
        for (int j=0; j<CL; j++) {
            for (int l=0; l<GF_MAXCLASSES; l++) {
                allcounts[l]=0;
                allproba[l]=0;
            }
            //txtout="\n"+listwavs[j]+":\n";
            for(int i=0;i<ss[j]->get_cntfbuf();i++){
                n1=nn1->getOutput(i, ss[j], TYPE_MFCC);
                n2=nn2->getOutput(i, ss[j], TYPE_MFSC);
                n3=nn3->getOutput(i, ss[j], TYPE_PGM);
                n4=nn4->getOutput(i, ss[j], TYPE_MEL);
                n5=nn5->getOutput(i, ss[j], TYPE_ALL);
                n6=nn6->getOutput(i, ss[j], TYPE_SCBA);
                //n1=nn1->getOutput(i, ss[j], TYPE_PGM);
                //n2=nn2->getOutput(i, ss[j], TYPE_PGM);
                
                
                //n4=nn4->getOutput(i, ss[j], TYPE_PGM);
                na=nn3->getAverage(3,n2,n3,n5);
                /*
                na1=nn3->getAverage(n1, n2, n3, n4);
                
                delete n1;delete n2;delete n3;delete n4;
                n1=nn12->getOutput(i, ss[j], TYPE_MFCC);
                n2=nn22->getOutput(i, ss[j], TYPE_MFSC);
                n4=nn42->getOutput(i, ss[j], TYPE_MEL);
                n3=nn32->getOutput(i, ss[j], TYPE_PGM);
                na2=nn3->getAverage(n1, n2, n3, n4);
                
                delete n1;delete n2;delete n3;delete n4;
                n1=nn13->getOutput(i, ss[j], TYPE_MFCC);
                n2=nn23->getOutput(i, ss[j], TYPE_MFSC);
                n4=nn43->getOutput(i, ss[j], TYPE_MEL);
                n3=nn33->getOutput(i, ss[j], TYPE_PGM);
                na3=nn3->getAverage(n1, n2, n3, n4);
                
                delete n1;delete n2;delete n3;delete n4;
                n1=nn14->getOutput(i, ss[j], TYPE_MFCC);
                n2=nn24->getOutput(i, ss[j], TYPE_MFSC);
                n4=nn44->getOutput(i, ss[j], TYPE_MEL);
                n3=nn34->getOutput(i, ss[j], TYPE_PGM);
                na4=nn3->getAverage(n1, n2, n3, n4);
                
                na=nn3->getAverage(na1, na2, na3, na4);
                delete na1;delete na2;delete na3;delete na4;
                
                */
                
                
                //txtout += AnsiString(i) + ": ";
                for (int c=0; c<nn3->nclasses(); c++) {
                    //txtout += listcnames[c]+": "+AnsiString(na->semantic[c+1])+" ";
                    allproba[c] += na->semantic[c+1];
                }
                //txtout += "\n";
                
                
                //txtout += AnsiString(i)+"s. Winner:" + listcnames[na->winnerIndex-1]+" E="+ss[j]->m_featbuf[i].mE+" \n";
                //allcounts[na->winnerIndex-1]++;
                prev3 = prev2;
                prev2 = prev;
                prev = current;
                if (na->semantic[na->winnerIndex]>0&&na->winnerIndex!=nn3->nclasses()) {
                    allcounts[na->winnerIndex-1]++;
                    current = na->winnerIndex;
                    if (1||(current == prev&& current==prev2 && current==prev3)) {
                        //txtout += listcnames[na->winnerIndex-1]+": "+AnsiString(na->semantic[na->winnerIndex])+" "+AnsiString(i)+"s." +AnsiString(current)+" "+AnsiString(prev)+" "+AnsiString(prev2)+" "+AnsiString(prev3)+"\n";
                        smoothed++;
                    }
                    
                }
                else {
                    allcounts[nn3->nclasses()-1]++;
                    current =nn3->nclasses();
                }
                
                delete na;
            }
            txtdecision = "Filename = "+listfnames[j]+"\n";
            txtdecision += "Probabilities = ";
            AnsiString bestmatch = "";
            float maxproba = 0;
            for (int c=0; c<nn3->nclasses(); c++) {
                if (maxproba<allproba[c]) {
                    maxproba = allproba[c];
                    bestmatch = listcnames[c];
                }
                txtout += listcnames[c]+": "+AnsiString(allcounts[c])+" ";
                txtdecision += listcnames[c]+": "+AnsiString(allproba[c]/ss[j]->get_cntfbuf())+"; ";
                if (listcnames[c]==listfnames[j]) {
                    hits += allcounts[c];
                }
            }
            
            //txtout += " E="+AnsiString(ss[j]->get_E())+" Hit="+AnsiString(hits)+" Smoothed="+AnsiString(smoothed)+" Total="+ss[j]->get_cntfbuf()+"\n";
            txtout += " E="+AnsiString(ss[j]->get_E())+" Hit="+AnsiString(hits)+" Total="+ss[j]->get_cntfbuf()+"\n";
            txtdecision += "\n";
            txtdecision += "Best match = "+bestmatch+"\n";
            allhits += hits;
            all +=ss[j]->get_cntfbuf();
            hits = 0;
            if (verbose) {
                std::cout <<txtout.c_str();
            }
            else {
                std::cout <<txtdecision.c_str();
            }
            
            txtout = "";
            txtdecision ="";
            
        }
        //std::cout<<"Total hits: "<<allhits<<" / "<<all<<"\n";
        
    }
    return 0;
}

