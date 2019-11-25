# TAI-Shazam

To build, go inside the programs folder and do:  
        make

To test:  

        ./executables/wavhist <input file> <channel>  
          
        ./executables/wavcp <input file> <output file>  
          
        ./executables/wavcmp <original file> <modified file>  
          
        ./executables/wavquant <input file> <output file> <byte resolution>  
          
        ./executables/wavcb -f 'filename -d 'directory -b block size -o overlaping factor -c codebook size -i max iterations in Kmeans -t number of threads -w outputfile (if using -f) or outputpath (if using -d)  
        Use at least -f or -d options  
          
        ./executables/wavfind <directory with codebooks> <audio sample file> <blockSize>  
