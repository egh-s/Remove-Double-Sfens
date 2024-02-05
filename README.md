# Remove-Double-Sfens
<pre>
Fast count and remove double sfens (compressed EPD's typically 40 bytes) 
for large NNUE data.
</pre>

# Short description
<pre>
1. From each SFEN a 64 bit checksum is made from the first 32 bytes 
   which contains the EPD, the color, the EP square and the castle 
   bits and stored into memory together with the offset of the SFEN.

2. The table is sorted and the doubles are counted. Doubles and their
   offset are stored in a new table with the offset only and sorted
   ready for the final step.

3. A new .BIN file is created skipping all offsets found in the table.
</pre>

# Operation
<pre>
1. At program start you are prompted to type the wished memory use in Gb
   and the util will allocate that amount. 

2. #define SIZE 16      // 8 OR 16 BYTES 

   SIZE = 8  : will count doubles only and needs only 8 bytes per SFEN.
   SIZE = 18 : will count and delete the doubles and needs 16 bytes per SFEN.

   This makes the util extremely memory hungry, limitations per Gb :

       Gb    Count   Delete  |   Gb    Count   Delete
        8     1B      500M   |  128     16B      8B
       16     2B       1B    |  256     32B     16B
       32     4B       2B    |  512     64B     32B
       64     8B       4B    |
</pre>

# Speed
<pre>
Aside from all the disk operations the main bottleneck is the sorting of 
billions of sfens. On my Ryzen 9 5900X using 24Gb the sorting of 3 billion
sfens took 9:45, counted 23 million doubles, 0.7%

As a rule of fist using qsort, double data will double sorting time.
</pre>

# Else
<pre>
1. #define MAX_DOUBLES 100000000               // 100M

   For storing doubles space is reserved for 100 millon sfens, increase 
   this number when you have more of them.

2. While the checksum approach is less effective than hashing (which would 
   seriously slowdown the disk reading the number of collisions is low, 
   about 1%. Meaning that in the above example the number is 0,007% of
   all positions.
</pre>
