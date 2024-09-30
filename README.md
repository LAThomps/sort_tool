# sort_tool
Recreating the UNIX command line "sort" function from scratch.

Built in python and c.

The python implementation is one file and uses the `argparse` library to be used like a standard CLI tool.

The c implementation is two files that must be compiled together (`sort_.c` and `hash_chr.c`). `sort_.c` is<br>
the main code with the sort algorithms, and `hash_chr.c` is the hash table if the unique argument is passed.<br>

`(for c)`<br>
The hash function sums the ASCII value of each character and subtracts 65 (to make the ASCII value of "A"<br>
start at the 0 index of the hash table). The maximum word length is 100 characters, so the maximum hash value<br>
is 12135 (the value of 100 "z"s minus 65). This hashes 52<sup>100</sup> possible word values down to 12135 unique keys.<br>
Linear probing by adding 12135 is used to handle collisions, and there is some extra logic to genuinely free up every<br>
single slot in the hash table if needed. With the maximum word count of 109215, a word would have to have 9 other<br>
matches before the extra logic kicks in. With the dramatic reduction in possible keys from the hash function, this is<br>
expected with files of sufficient amounts of unique words.

