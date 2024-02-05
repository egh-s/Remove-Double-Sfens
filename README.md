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
  
</pre>
