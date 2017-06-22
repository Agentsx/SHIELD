
export LD_LIBARARY_PATH=./libs:$LD_LIBARARY_PATH
echo $LD_LIBARARY_PATH

./shield -f conf/shield.cfg
