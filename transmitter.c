/* 
 * File:   transmitter.c
 * Author: daniel
 *
 * Created on September 20, 2011, 12:42 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>


/*
 * 
 */



void Convert2Bin(int, char[]);

int main(int argc, char** argv) {
     char userFile[256], dir[256];
     char fileName[256]; /**< name of file to be open */
     FILE* fins;         // Input file stream
     FILE* fos;          // Output file stream
     long fSize;         // Size of the file in characters
     char*  binBuffer;      // Buffer of the file converted to binary.
     char buffsyn[8] = "10010110"; // Sync Character in binary.
     char* frame;        // What its name says. 1 Frame 
     char cBuffer[8],  ext[256];          // Input character in binary
     char* dataSbin;     // Size of the data block in binary.
     char* extBin;
     int buffin;         // Input buffer
     char garbage[1];

     /*
      * counter i, Buffer Length, Number of Frames, Data Size.
      */
     int i, blen, framen, dataSize;

     while(1){
    /** Get the file location, file name and character to replace space with */
        printf("Enter 'exit' for dir to quit. \n");
        printf("Enter directory of the file: \n Ex: /home/user/\n");
        fgets(dir,sizeof(dir), stdin);
        if(strncmp(dir,"exit",4)==0){
            break;
        }
        printf("Enter filename: \n Ex: myfile\n");
        fgets(userFile, sizeof(userFile),stdin);
        if(strncmp(userFile,"exit",4)==0){
            break;
        }
        printf("Enter file extension: \n Ex: .txt\n");
        fgets(ext, sizeof(ext),stdin);
        if(strncmp(ext,"exit",4)==0){
            break;
        }
        if(dir[strlen(dir)-1]== '\n'){
            dir[strlen(dir)-1]= '\0';
        }
        if(userFile[strlen(userFile)-1]== '\n'){
            userFile[strlen(userFile)-1]= '\0';
        }
        if(ext[strlen(ext)-1]== '\n'){
            ext[strlen(ext)-1]= '\0';
        }


        /** Prep the strings for the server thread */
        strcpy(fileName,dir);
        strcat(fileName,userFile);
        strcat(fileName,ext);
        
        
        fins = fopen(fileName, "rb");
        fos = fopen("output.bin", "wb");
        if (fins==NULL||fos==NULL) {fputs ("File error",stderr); exit (1);}
        
        fseek (fins , 0 , SEEK_END);
        fSize = ftell (fins);
        rewind (fins);      
        blen = fSize*8;
        
        binBuffer = (char*) malloc (sizeof(char)*(blen));
        if (binBuffer == NULL) {fputs ("Memory error",stderr); exit (2);}
     

        while((buffin=fgetc(fins))!=EOF){
            Convert2Bin(buffin, cBuffer);
            strncat(binBuffer, cBuffer, 8);
        }
        
        extBin = (char*) malloc (sizeof(char)*(32));
        if (extBin == NULL) {fputs ("Memory error",stderr); exit (2);}
        
        for(i=0;i<4;i++){
            Convert2Bin(ext[i],cBuffer);
            strncat(extBin, cBuffer, 8);
        }
        
        frame = (char*) malloc (sizeof(char)*(88));
          if (frame == NULL) {fputs ("Memory error",stderr); exit (2);}
        
        dataSbin = (char*) malloc (sizeof(char)*(8));
          if (dataSbin == NULL) {fputs ("Memory error",stderr); exit (2);}
        
        strncpy(frame, buffsyn, 8);
        
        Convert2Bin(32,dataSbin);
        
        strncat(frame, dataSbin, 8);
        strncat(frame, extBin, 32);
        strncat(frame, buffsyn, 8);
        
        
        fwrite(frame,1,56,fos);

        if(blen%64==0){
           framen = blen/64;
        }else{
           framen = blen/64 + 1;
        }
        
        for(i= 0;i<framen;i++){
          frame = (char*) malloc (sizeof(char)*(88));
          if (frame == NULL) {fputs ("Memory error",stderr); exit (2);}
          
          dataSbin = (char*) malloc (sizeof(char)*(8));
          if (dataSbin == NULL) {fputs ("Memory error",stderr); exit (2);}
          
          strncpy(frame, buffsyn,8);
          
          if (i==framen-1 && blen%64!=0){
              dataSize = blen%64;
          }else{
              dataSize = 64;
          }
          
          Convert2Bin(dataSize, dataSbin);
          
          strncat(frame, dataSbin, 8);
          strncat(frame, binBuffer + i*64,dataSize);
          strncat(frame, buffsyn, 8);
          

          fwrite(frame,1,dataSize+24,fos); // Writes to the output file.
        }

        
        fclose(fins);
        fclose(fos);
        free(binBuffer);
     }


    return (EXIT_SUCCESS);
}


/* 
 * Converts and integer into a binary string to make it
 * editable by humans and to symbolized the binary data.
 */ 
void Convert2Bin(int bin, char str[8])
{
    unsigned int mask;      // used to check each individual bit, 
                            //    unsigned to alleviate sign 
                            //    extension problems
    int parity,i;
    parity = 0;
    i=0;
    mask = 128;      // Set only the high-end bit
    while (mask)            // Loop until MASK is empty
    {
        if (bin & mask){     // test the masked bit
              str[i] = '1';   // if true, value is 1
              parity++;
        }
            else 
              str[i] = '0';   // if false, value is 0
        i++;              // next character
        mask >>= 1;         // shift the mask 1 bit
    }
    if(parity%2==1){
        str[0]='1';               // Add the Leading parity bit
    }else{
        str[0]='0';
    }
}