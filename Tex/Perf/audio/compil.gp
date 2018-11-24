set terminal png
set output "decodecompaudio2.png"
set title 'Comparaison des performances du decodage (pour des fichiers audio)'
set xlabel 'Taille (en Mo)'
set ylabel 'Temps (en secondes)'
plot "decodecompaudio.txt" using 1:2 with lines title 'HSMI' lt rgb "red", "decodecompaudio.txt" using 1:3 with lines title 'HDMI Non opti' lt rgb "blue", "decodecompaudio.txt" using 1:4 with lines title 'HDMI Opti' lt rgb "purple"

set output "encodecompaudio2.png"
set title "Comparaison des performances de l'encodage (pour des fichiers audio)"
set xlabel 'Taille (en Mo)'
set ylabel 'Temps (en secondes)'
plot "encodecompaudio.txt" using 1:2 with lines title 'HSMI' lt rgb "red", "encodecompaudio.txt" using 1:3 with lines title 'HDMI Non opti' lt rgb "blue", "encodecompaudio.txt" using 1:4 with lines title 'HDMI Opti' lt rgb "purple"