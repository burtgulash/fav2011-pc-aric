# Aric - Arithmetic coding utility

Seminar assignment from course **Programming in C**. 
Aim of this assignment is to make a program to compress and decompress arbitrary files using arithmetic coding.

## Usage
```
usage: aric -c FILE COMPRESSED
       aric -d COMPRESSED FILE
```

To compress an input **FILE** to **COMPRESSED** output file use the flag **-c**.
To decompress the **COMPRESSED** file back use the second option **-d** with output file **FILE**.

## Example usage

Compress
```
./aric -c coder.c out.aric
    output: 3883 bytes to 2879 bytes
```

Decompress
```
./aric -d out.aric coder_decompressed.c
    output: 2878 bytes to 3883 bytes
```

Verify that the original file is identical to the decompressed file
``` bash
if cmp -s coder.c coder_decompressed.c
then
    echo OK
else
    echo FAILED
fi
```

## Documentation
Stub of [documentation](doc/aric.pdf) for the assignment in czech language.

## Course details
* [KIV/PC](http://www.kiv.zcu.cz/studies/predmety/pc/) fall 2011
* Lecturer [Ing. Kamil Ekštein, Ph.D.](https://www.kiv.zcu.cz/cz/katedra/osoby-seznam/osoba-detail.html?login=kekstein)
* Faculty of Applied Sciences - University of West Bohemia (Fakulta Aplikovaných Věd - ZČU Plzeň)
