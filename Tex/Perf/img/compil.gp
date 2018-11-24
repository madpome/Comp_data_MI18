set terminal png
set output "decodecompimg2.png"
set title 'Comparaison des performances du decodage (pour des fichiers images)'
set xlabel 'Taille (en Mo)'
set ylabel 'Temps (en secondes)'
plot "decodecompimg.txt" using 1:2 with lines title 'HSMI' lt rgb "red", "decodecompimg.txt" using 1:3 with lines title 'HDMI Non opti'lt rgb "blue", "decodecompimg.txt" using 1:4 with lines title 'HDMI Opti' lt rgb "purple"

set output "encodecompimg2.png"
set title "Comparaison des performances de l'encodage (pour des fichiers images)"
set xlabel 'Taille (en Mo)'
set ylabel 'Temps (en secondes)'
plot "encodecompimg.txt" using 1:2 with lines title 'HSMI' lt rgb "red", "encodecompimg.txt" using 1:3 with lines title 'HDMI Non opti' lt rgb "blue", "encodecompimg.txt" using 1:4 with lines title 'HDMI Opti' lt rgb "purple"