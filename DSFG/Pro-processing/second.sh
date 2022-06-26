make clean
make
echo "----------second.sh----------";
echo "待处理文件相对位置:$1"

./handleRes $1 resbig.txt
echo "resbig.txt successfully!"

make clean