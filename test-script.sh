#!/bin/bash
TEST_IMG="Explod.png image1.jpeg image2.jpeg image3.png image4.png"
TEST_TXT="OurFriendtheDog.txt MobyDick.txt AmerikastaPalatessa.txt Chinois.txt Florante.txt"
TEST_AUDIO=
CD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/"
echo "$CD"
STATIC=$CD"Huffman_Static/"
ADAPTATIVE=$CD"Huffman_Dynamic/"
WD=$CD"test_compression/"
IMG=$WD"img/"
AUDIO=$WD"audio/"
TXT=$WD"txt/"
ENCOD=$STATIC"encodeHuff"
DECOD=$STATIC"decodeHuff"
echo "$ENCOD"
echo "$DECOD"
echo "----TESTING SCRIPT STATIC HUFFMAN----"
echo "****TESTING TEXT***"
for file in $TEST_TXT
do
    echo ""
    echo "Compressing $file to $file.compStat"
    eval "$ENCOD $TXT$file $TXT$file.comStat"
    echo "Decompressing $file.comStat to $file.decompStat"
    eval "$DECOD $TXT$file.comStat $TXT$file.decompStat"
    echo "Removing $TXT$file.comStat"
    eval "rm -f $TXT$file.comStat"
    echo "Diff between $file and $file.decompStat"
    eval "diff $TXT$file $TXT$file.decompStat"
    echo "End diff"
done

echo "****TESTING IMAGE***"
for file in $TEST_IMG
do
    echo ""
    echo "Compressing $file to $file.comStat"
    eval "$ENCOD $IMG$file $IMG$file.comStat"
    echo "Decompressing $file.comStat to $file.decompStat"
    eval "$DECOD $IMG$file.comStat $IMG$file.decompStat"
    echo "Removing $IMG$file.comStat"
    eval "rm -f $IMG$file.comStat"
    echo "Diff between $file and $file.decompStat"
    eval "diff $IMG$file $IMG$file.decompStat"
    echo "End diff"
done

ENCOD=$ADAPTATIVE"encodeHuff"
DECOD=$ADAPTATIVE"decodeHuff"
echo ""
echo "----TESTING SCRIPT ADAPTATIVE HUFFMAN----"
echo "****TESTING TEXT***"
for file in $TEST_TXT
do
    echo ""
    echo "Compressing $file to $file.comDyn"
    eval "$ENCOD $TXT$file $TXT$file.comDyn"
    echo "Decompressing $file.comDyn to $file.decompDyn"
    eval "$DECOD $TXT$file.comDyn $TXT$file.decompDyn"
    echo "Removing $TXT$file.comDyn"
    eval "rm -f $TXT$file.comDyn"
    echo "Diff between $file and $file.decompDyn"
    eval "diff $TXT$file $TXT$file.decompDyn"
    echo "End diff"
done

echo "****TESTING IMAGE***"
for file in $TEST_IMG
do
    echo ""
    echo "Compressing $file to $file.comDyn"
    eval "$ENCOD $IMG$file $IMG$file.comDyn"
    echo "Decompressing $file.comDyn to $file.decompDyn"
    eval "$DECOD $IMG$file.comDyn $IMG$file.decompDyn"
    echo "Removing $IMG$file.comDyn"
    eval "rm -f $IMG$file.comDyn"
    echo "Diff between $file and $file.decompDyn"
    eval "diff $IMG$file $IMG$file.decompDyn"
    echo "End diff"
done

exit 0
