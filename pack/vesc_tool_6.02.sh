#!/bin/sh

appname=`basename $0 | sed s,\.sh$,,`
dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

# set font path
#QT_QPA_FONTDIR=$dirname/fonts
#export QT_QPA_FONTDIR

# set library path
LD_LIBRARY_PATH=$LD_LIBRATY_PATH:$dirname
export LD_LIBRARY_PATH
$dirname/$appname "$@"

