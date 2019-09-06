#!/bin/bash 

for i in *.jpg; 
do 
j=${i//\.jpg/}; 
#convert -size 800x600 $i ${j}_klein.jpg; 
#convert -scale 50% -quality 75% $i ${j}_klein.jpg;
#To resize smaller side to 200 if it is larger than 200 and preserve aspect ratio, use
#convert -resize "200^>" $i ${j}_klein.jpg;
#Y-achse in Pixeln ist angegeben. die X-achse wird skaliert. Ohne das x wäre die X-Achse angegeben.
convert -resize "x100>" $i ${j}_klein.jpg;
done
#mv *klein.jpg kleinere_Versionen

#Hifle hierzu:
# http://xahlee.org/img/imagemagic.html
