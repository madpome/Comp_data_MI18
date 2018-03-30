#!/bin/bash
TEST_FILE="Explod.png MobyDick.txt AmerikastaPalatessa.txt Chinois.txt \
Florante.txt image1.jpeg image2.jpeg image3.png image4.png \
OurFriendtheDog.txt"
CD="~/CompressionDonnées/"
WD=$CD"test_huff_stat/"
HUFF=$CD"huffman"
DECOD=$HUFF"Decod"
echo "Testing script for static huffman compressor"

for file in $TEST_FILE
do
    echo ""
    echo "Compressing $file to $file.comp"
    eval "$HUFF $WD$file $WD$file.comp"
    echo "Decompressing $file.comp to $file.huffed"
    eval "$DECOD $WD$file.comp $WD$file.huffed"
    echo "Removing $WD$file.comp"
    eval "rm -f $WD$file.comp"
    echo "Diff between $file and $file.huffed"
    eval "diff $WD$file $WD$file.huffed"
    echo "End diff"
done
exit 0
