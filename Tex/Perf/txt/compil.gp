set terminal png
set output "decodecomptxt2.png"
set title 'Comparaison des performances du decodage (pour des fichiers textes)'
set xlabel 'Taille (en Ko)'
set ylabel 'Temps (en secondes)'
plot "decodecomptxt.txt" using 1:2 with lines title 'HSMI' lt rgb "red", "decodecomptxt.txt" using 1:3 with lines title 'HDMI Non opti' lt rgb "blue", "decodecomptxt.txt" using 1:4 with lines title 'HDMI Opti' lt rgb "purple"

set output "encodecomptxt2.png"
set title "Comparaison des performances de l'encodage (pour des fichiers textes)"
set xlabel 'Taille (en Ko)'
set ylabel 'Temps (en secondes)'
plot "encodecomptxt.txt" using 1:2 with lines title 'HSMI' lt rgb "red", "encodecomptxt.txt" using 1:3 with lines title 'HDMI Non opti' lt rgb "blue", "encodecomptxt.txt" using 1:4 with lines title 'HDMI Opti' lt rgb "purple"