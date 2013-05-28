# !/bin/sh

python genconfig.py
cd nginx

if cmp -s "../config" "../config.bk" ; then
    echo "config file is not changed"
else
    ./configure --add-module=/root/projects/lookaround
fi

make
make install
cd ..

