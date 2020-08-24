#!/usr/bin/env bash
IMAGEMAGICK=/d/myProgram/ImageMagick-7.0.10-28-portable-Q16-HDRI-x64/
CONVERT=$IMAGEMAGICK/convert
IDENTIFY=$IMAGEMAGICK/identify

TMPFILE=_tmp.pbm

function svg2h
{
    RESIZE=$1
    IN=$2
    OUT=$3
    [ -f ${TMPFILE} ] && rm ${TMPFILE}
    # ${CONVERT} -resize ${RESIZE} ${IN} -depth 8 -negate -compress none  _tmp.pgm
    ${CONVERT} -resize ${RESIZE} ${IN} -compress none ${TMPFILE}

    ${IDENTIFY} -format "const int ${OUT}_width  = %w;\n" ${TMPFILE}
    ${IDENTIFY} -format "const int ${OUT}_height = %h;\n" ${TMPFILE}
    echo -e 
    echo -e "const unsigned char ${OUT}_d[] = {"
    sed -e '1,3d' -e "s/ //g" -e "s/\(........\)/0b\1,/g" -e "s/^/  /" ${TMPFILE}
    echo -e "};"
    [ -f ${TMPFILE} ] && rm ${TMPFILE}
}

if [ "$1" != "" ]; then
    (
        svg2h $1 ./svg/cat_0.svg cat0_$1
        svg2h $1 ./svg/cat_1.svg cat1_$1
        svg2h $1 ./svg/cat_2.svg cat2_$1
        svg2h $1 ./svg/cat_3.svg cat3_$1
        svg2h $1 ./svg/cat_4.svg cat4_$1
        svg2h $1 ./svg/cat_sleep.svg cat_sleep_$1
    ) > cat_$1.h
else
    $0 320x240
    $0 160x120
    $0  80x60
    $0  32x24
fi
