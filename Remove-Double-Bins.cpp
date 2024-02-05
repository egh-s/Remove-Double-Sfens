#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <algorithm>    // std::sort

#include <iostream>
#include <cstdio>
#include <io.h>

#define SIZE 16      // 8 OR 16 BYTES 
#define DEBUG 0      // 0=OFF | 1=ON

char bin_one[256] = "n:/train/tempval.bin";
char bin_out[256] = "n:/train/new.bin";	
char debug  [256] = "n:/train/debug.bin";

void puntjes(void);

#if SIZE==16
struct hash
{   unsigned long long sum;
    unsigned long long offset; };	
struct hash *sfen;

#else

struct hash
{   unsigned long long sum; };
struct hash* sfen;
#endif

unsigned long long key=0;                   // offset

unsigned long long ms,gb;
char *p_memory,*bgn;
unsigned long long limit;
FILE *fpc,*fpo,*fpd;
char w[200],fn[200],buf1[200],buf2[200],buf3[200];
unsigned long long count=0;
int dubbel=0;
int x,y,i,j,a,z;
int t1, t2, time_taken;
unsigned long long k1,k2,k3,k4,k5;
float f1,f2,f3;

unsigned long long zettel;
unsigned long long bin_size;

#define MAX_DOUBLES 100000000               // 100M
long long unsigned doubles[MAX_DOUBLES];    
unsigned long long pos;

int compare(const void* l_, const void* r_) {       // in case of using qsort
    uint64_t l = *(const uint64_t*)l_;
    uint64_t r = *(const uint64_t*)r_;
    if (l < r) return -1;
    if (l > r) return  1;
    return 0;
}

int main(int argc, char *argv[]) {

        fpc = fopen(bin_one, "rb");
        if (fpc == 0) { printf("\nFile %s not present...\n\n", fn); gets_s(w); exit(1); }

        // get filesize file one				
        _fseeki64(fpc, 0, SEEK_END);
        bin_size = _ftelli64(fpc); bin_size = bin_size / 40;
        fclose(fpc);
        zettel=bin_size; puntjes(); printf("File : %s contains %s sfens\n\n",bin_one,buf2);
        strcpy(buf3,buf2);

        printf("Type memory to allocate in Gb "); gets_s(w);
        gb=atoi(w); gb*=1024*1048576;

        for (ms = gb; ms >= 2000; ms -= 100000)     // allocate 8 Gb
         { p_memory = (char*)malloc(ms); if (p_memory) break; }
           printf("\nFree memory = %lld Mb\n",ms>>20); 
	    free(p_memory);
	    p_memory=(char*)malloc(ms);
	    sfen=(struct hash *)p_memory;
	    bgn=p_memory;
	    p_memory=p_memory+ms;
	    limit=p_memory-bgn;                          // calulate maximum number entries        
        limit/=SIZE;                                 // length structure     
	    printf("\nAllocated space for %lld million sfen entries\n\n",limit>>20);
 		
	    fpc=fopen(bin_one,"rb");	          
        if (fpc == 0) { printf("\nFile %s not present, try again\n\n", fn); gets_s(w); exit(1); }  	

        printf("\nReading file into memory....\n\n");

lees:   x=fread(&k1,8,1,fpc); if (x<1) { fclose(fpc); goto done; }
	    fread(&k2,8,1,fpc);
	    fread(&k3,8,1,fpc);
	    fread(&k4,8,1,fpc);
	    fread(&k5,8,1,fpc);
        
        sfen[count].sum = k1 + k2 + k3 + k4;
        
        #if SIZE==16    
        sfen[count].offset = key;
        key += 40;
        #endif

        count++; if (count > limit) { printf("\n\nMaximum memory reached... \a"); goto done; }

        if (count % 10000000 == 0) { zettel=count; puntjes(); printf("Reading %s of %s\n", buf2, buf3); }    // each 10 million
        
        goto lees;


done:   zettel=count;  puntjes();
        printf("\n\nDone... counted %s sfens [%lldM]", buf2,count >> 20);
    
        fclose(fpc);

        printf("\n\nSorting %s sfens",buf2);

        clock_t t1;
        t1 = clock();
        clock_t t2;

        qsort(sfen, count, SIZE, compare);

        t2 = clock();
        t2 = t2 - t1;
        time_taken = t2 / CLOCKS_PER_SEC;

        printf("\n\nSorting done... time %d seconds",time_taken);

	    printf("\n\nChecking for doubles ");
		
        k1=0;

        #if SIZE==16
        for(x=0; x<count; x++)
        {  if (dubbel >= MAX_DOUBLES) { printf("Maximum doubles reached..."); break; }
           if (sfen[x].sum == k1) { doubles[dubbel]=sfen[x].offset; dubbel++; continue; }
           k1=sfen[x].sum; }

        #else
        for (x=0; x<count; x++)
         { if (sfen[x].sum == k1) { dubbel++; continue; }
            k1 = sfen[x].sum; }
        #endif

        f1=count; f2=dubbel; f2=f2*100; f3=f2/f1;
        zettel = dubbel; puntjes(); strcpy(buf3,buf2);
        printf("\n\nDoubles found : %s [%.1f%%] ",buf2,f3);

        if (SIZE == 8) { printf("\n\nDone, press enter to exit program "); gets_s(w); exit(0); }
        
        // Part II, remove doubles
        // =======================
        
        printf("\n\nSorting offsets of doubles found in %s", bin_one);
       
        qsort(doubles, dubbel, 8, compare);

        #if DEBUG
        // copy to disk for checking
        fpo=fopen("doubles.bin", "wb");
        for (x=0; x<dubbel; x++) fwrite(&doubles[x],8,1,fpo);
        fclose(fpo);
        #endif

        // delete all offsets and create the new data without doubles
        // ==========================================================

        printf("\n\nLast part, %s is stored into the new file %s without the doubles\n\n", bin_one,bin_out);
        
        fpc = fopen(bin_one, "rb");
        fpo = fopen(bin_out, "wb");
        
        #if DEBUG
        fpd = fopen(debug, "wb");       // gather deleted doubles for checking with bin-to-epd
        #endif

        pos = -40;                      // offset fpc
        x = 0;                          // pointer offset doubles table
                
next:   y = fread(&k1, 8, 1, fpc); if (y < 1) { fclose(fpc); goto finito; }
        fread(&k2, 8, 1, fpc);
        fread(&k3, 8, 1, fpc);
        fread(&k4, 8, 1, fpc);
        fread(&k5, 8, 1, fpc);

        pos += 40;

        if (pos == doubles[x]) 
          { x++; 
            if (x % 100000 == 0) { zettel = x; puntjes(); printf("Removing %s of %s doubles\n", buf2,buf3); }
            
            #if DEBUG
            fwrite(&k1, 8, 1, fpd);
            fwrite(&k2, 8, 1, fpd);
            fwrite(&k3, 8, 1, fpd);
            fwrite(&k4, 8, 1, fpd);
            fwrite(&k5, 8, 1, fpd);
            #endif
            
            goto next;  }

door:   fwrite(&k1, 8, 1, fpo);
        fwrite(&k2, 8, 1, fpo);
        fwrite(&k3, 8, 1, fpo);
        fwrite(&k4, 8, 1, fpo);
        fwrite(&k5, 8, 1, fpo);

        goto next;


finito: fclose(fpc);
        fclose(fpo);

        #if DEBUG
        fclose(fpd);
        #endif

        zettel = x; puntjes(); printf("Removed %s of %s doubles\n\n", buf2,buf3);

        printf("Created file : %s",bin_out);

        printf("\n\nAll done, press enter to exit program "); gets_s(w); exit(0);

	    return 0;
}

void puntjes()						// input = zettel
                                    // output= string buf2
{
    int l, x;						// uses string buf1
                                    // max = 999.999.999.999
    sprintf(buf1, "%lld", zettel);

    l = strlen(buf1);

    if (l > 9) {
        strncpy(buf2, buf1, l - 9); buf2[l - 9] = 0;
        for (x = l - 9; x <= l; x++) buf1[x - (l - 9)] = buf1[x]; buf1[9] = 0;
        strcat(buf2, ".");
        strncat(buf2, buf1, 3);
        strcat(buf2, ".");
        for (x = 0; x <= 6; x++) buf1[x] = buf1[x + 3]; buf1[6] = 0;
        strncat(buf2, buf1, 3);
        strcat(buf2, ".");
        for (x = 0; x <= 3; x++) buf1[x] = buf1[x + 3]; buf1[3] = 0;
        strcat(buf2, buf1); return;
    }

    if (l > 6) {
        strncpy(buf2, buf1, l - 6); buf2[l - 6] = 0;
        for (x = l - 6; x <= l; x++) buf1[x - (l - 6)] = buf1[x]; buf1[6] = 0;
        strcat(buf2, ".");
        strncat(buf2, buf1, 3);
        strcat(buf2, ".");
        for (x = 0; x <= 3; x++) buf1[x] = buf1[x + 3]; buf1[3] = 0;
        strcat(buf2, buf1); return;
    }

    if (l > 3) {
        strncpy(buf2, buf1, l - 3); buf2[l - 3] = 0;
        for (x = l - 3; x <= l; x++) buf1[x - (l - 3)] = buf1[x]; buf1[3] = 0;
        strcat(buf2, ".");
        strcat(buf2, buf1);
        return;
    }

    strcpy(buf2, buf1);

}
