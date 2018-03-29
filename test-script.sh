#!/bin/bash
TEST_FILE="Explod.png MobyDick.txt"
WD="test_huff_stat/"
HUFF="./huffman"
DECOD="./huffmanDecod"
echo "Testing script for static huffman compressor"

for file in $TEST_FILE
do
    echo ""
    echo "Compressing $file to $file.comp"
    eval "$HUFF $WD$file $WD$file.comp"
    echo ""
    echo "Decompressing $file.comp to $file.huffed"
    eval "$DECOD $WD$file.comp $WD$file.huffed"
done
exit 0
