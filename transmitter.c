/* 
 * File:   transmitter.c
 * Author: Daniel Huelsman 
 * Class: Data Communications 
 * Project 1
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
void HammingEncode (char[],char[]);
void CRCEncode (char[],char[]);

int main(int argc, char** argv) {
     char userFile[256], dir[256];
     char fileName[256]; /**< name of file to be open */
     FILE* fins;         // Input file stream
     FILE* fos;          // Output file stream
     long fSize;         // Size of the file in characters
     char*  binBuffer;      // Buffer of the file converted to binary.
     char buffsyn[8] = "00010110"; // Sync Character in binary.
     char* frame;        // What its name says. 1 Frame 
     char cBuffer[8], ceBuffer[13],  ext[256];          // Input character in binary
     char* dataSbin;     // Size of the data block in binary.
     char* extBin;
     int buffin;         // Input buffer
     char encoding[5];       // Stores encoding selection
     /*
      * counter i, Buffer Length, Number of Frames, Data Size.
      */
     int i, blen, framen, dataSize;

     while(1){
    /** Get the file location, file name and file extension */
        printf("Enter 'exit' for dir to quit. \n");
        printf("Enter 1 for Hamming Encoding \nEnter 2 For CRC\n");
        fgets(encoding, sizeof(encoding), stdin);
        if(strncmp(encoding,"exit",4)==0){
            break;
        }
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
        /*Replace newline characters with null characters*/
        if(dir[strlen(dir)-1]== '\n'){
            dir[strlen(dir)-1]= '\0';
        }
        if(userFile[strlen(userFile)-1]== '\n'){
            userFile[strlen(userFile)-1]= '\0';
        }
        if(ext[strlen(ext)-1]== '\n'){
            ext[strlen(ext)-1]= '\0';
        }


        /*Prep the filename*/
        strcpy(fileName,dir);
        strcat(fileName,userFile);
        strcat(fileName,ext);
        
        /*Open the input file stream*/
        fins = fopen(fileName, "rb");
        fos = fopen("output.bin", "wb");
        if (fins==NULL||fos==NULL) {fputs ("File error",stderr); exit (1);}
        
        /*Find the file Size*/
        fseek (fins , 0 , SEEK_END);
        fSize = ftell (fins);
        rewind (fins);      
        blen = fSize*13;
        
        binBuffer = (char*) malloc (sizeof(char)*(blen));
        if (binBuffer == NULL) {fputs ("Memory error",stderr); exit (2);}
     
        /*Read the file into a Buffer*/
        while((buffin=fgetc(fins))!=EOF){
            Convert2Bin(buffin, cBuffer);
            if(encoding[0]=='1'){
                HammingEncode(cBuffer,ceBuffer);
            }else if(encoding[0]=='2'){
                CRCEncode(cBuffer,ceBuffer);
            }
            strncat(binBuffer, ceBuffer, 13);
        }
        
        extBin = (char*) malloc (sizeof(char)*(52));
        if (extBin == NULL) {fputs ("Memory error",stderr); exit (2);}
        
        /*Convert extension into binary*/
        for(i=0;i<4;i++){
            Convert2Bin(ext[i],cBuffer);
            if(encoding[0]=='1'){
                HammingEncode(cBuffer,ceBuffer);
            }else if(encoding[0]=='2'){
                CRCEncode(cBuffer,ceBuffer);
            }
            strncat(extBin, ceBuffer, 13);
        }
        
        frame = (char*) malloc (sizeof(char)*(143));
          if (frame == NULL) {fputs ("Memory error",stderr); exit (2);}
        
        dataSbin = (char*) malloc (sizeof(char)*(13));
          if (dataSbin == NULL) {fputs ("Memory error",stderr); exit (2);}
        
        /*Pack the extension into the first frame*/
        if(encoding[0]=='1'){
                HammingEncode(buffsyn,ceBuffer);
            }else if(encoding[0]=='2'){
                CRCEncode(buffsyn,ceBuffer);
            }
        strncpy(frame, ceBuffer, 13);        
        Convert2Bin(52,dataSbin);
        if(encoding[0]=='1'){
                HammingEncode(dataSbin,ceBuffer);
            }else if(encoding[0]=='2'){
                CRCEncode(dataSbin,ceBuffer);
            }
        strncat(frame, ceBuffer,13 );
        strncat(frame, extBin, 52);
        if(encoding[0]=='1'){
                HammingEncode(buffsyn,ceBuffer);
            }else if(encoding[0]=='2'){
                CRCEncode(buffsyn,ceBuffer);
            }
        strncat(frame, ceBuffer, 13);
        
        /*Write the first frame to the file*/
        fwrite(frame,1,91,fos);

        /*Find the number of Frames*/
        if(blen%104==0){
           framen = blen/104;
        }else{
           framen = blen/104 + 1;
        }
        
        /*
         * Create all of the frames and write each frame to the file
         * as it is created
         */
        for(i= 0;i<framen;i++){
          frame = (char*) malloc (sizeof(char)*(143));
          if (frame == NULL) {fputs ("Memory error",stderr); exit (2);}
          
          dataSbin = (char*) malloc (sizeof(char)*(13));
          if (dataSbin == NULL) {fputs ("Memory error",stderr); exit (2);}
          
          if(encoding[0]=='1'){
                HammingEncode(buffsyn,ceBuffer);
            }else if(encoding[0]=='2'){
                CRCEncode(buffsyn,ceBuffer);
            }
          strncpy(frame, ceBuffer,13);
          
          /*Figure out if it is a full 64 bits or not*/
          if (i==framen-1 && blen%104!=0){
              dataSize = blen%104;
          }else{
              dataSize = 104;
          }
          
          Convert2Bin(dataSize, dataSbin); 
          if(encoding[0]=='1'){
                HammingEncode(dataSbin,ceBuffer);
            }else if(encoding[0]=='2'){
                CRCEncode(dataSbin,ceBuffer);
            }
          strncat(frame, ceBuffer, 13);
          strncat(frame, binBuffer + i*104,dataSize);
          if(encoding[0]=='1'){
                HammingEncode(buffsyn,ceBuffer);
            }else if(encoding[0]=='2'){
                CRCEncode(buffsyn,ceBuffer);
            }
          strncat(frame, ceBuffer, 13);
          
          fwrite(frame,1,dataSize+39,fos); // Writes to the output file.
        }

        /*Close file streams and free buffer*/
        fclose(fins);
        fclose(fos);
        //free(binBuffer);
     }

    return (EXIT_SUCCESS);
}


/* 
 * Converts and integer into a binary string to make it
 * editable by humans and to symbolized the binary data.
 */ 
void Convert2Bin(int bin, char str[8]){
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
        str[0]='0';               // Add the Leading parity bit
    }else{
        str[0]='1';
    }
    
}

void HammingEncode (char str[8],char hstr[13]){
    int pOne = 0, pTwo=0, pFour=0, pEight=0;
    int i, pCount=0;
    
    if (str[0]=='1'){
        pOne++;
        pTwo++;
    }if (str[1]=='1'){
        pOne++;
        pFour++;
    }if (str[2]=='1'){
        pTwo++;
        pFour++;
    }if (str[3]=='1'){
        pOne++;
        pTwo++;
        pFour++;
    }if (str[4]=='1'){
        pOne++;
        pEight++;
    }if (str[5]=='1'){
        pTwo++;
        pEight++;
    }if (str[6]=='1'){
        pOne++;
        pTwo++;
        pEight++;
    }if (str[7]=='1'){
        pFour++;
        pEight++;
    }
    if(pOne%2==1){
        hstr[1]='0';
    }else
        hstr[1]='1';
    if(pTwo%2==1){
        hstr[2]='0';
    }else
        hstr[2]='1';
    hstr[3]=str[0];
    if(pFour%2==1){
        hstr[4]='0';
    }else
        hstr[4]='1';
    hstr[5]=str[1];
    hstr[6]=str[2];
    hstr[7]=str[3];
     if(pEight%2==1){
        hstr[8]='0';
    }else
        hstr[8]='1';
    hstr[9]=str[4];
    hstr[10]=str[5];
    hstr[11]=str[6];
    hstr[12]=str[7];
    
    for(i=1; i<13; i++){
        if(hstr[i]=='1'){
            pCount++;
        }
    }
    if(pCount%2!=1){
        hstr[0]='1';
    }else{
        hstr[0]='0';
    }
}

void CRCEncode(char str[8], char crstr[13]){
int i,j,n=8,g=6,a=13,arr[13],q[13],gen[6];
// Convert to Int array
for(i=0;i<n;i++){
    if(str[i]=='1'){
        arr[i]=1;
    }else{
        arr[i]=0;
    }
}
gen[0]=1;
gen[1]=0;
gen[2]=0;
gen[3]=0;
gen[4]=1;
gen[5]=1;
//Append Trailing Zeros
for(i=0;i< g-1;++i){
    arr[n+i]=0;
}
//Copy original data into output
for(i=0;i< n;++i){
    q[i]= arr[i];
}
//Calculate CRC
for(i=0;i< n;++i){
    if(arr[i]==0){
        for(j=i;j< g+i;++j)
        arr[j] = arr[j]^0;
    }else{
        arr[i] = arr[i]^gen[0];
        arr[i+1]=arr[i+1]^gen[1];
        arr[i+2]=arr[i+2]^gen[2];
        arr[i+3]=arr[i+3]^gen[3];
        arr[i+4]=arr[i+4]^gen[4];
        arr[i+5]=arr[i+5]^gen[5];
    }
}
//copy CRC into output
for(i=n;i< a;i++){
    q[i]=arr[i];
}
//Convert back to a string
for(i=0;i<a;i++){
    if(q[i]==1){
        crstr[i]='1';
    }else{
        crstr[i]='0';
    }
}
}