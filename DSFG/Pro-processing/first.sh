make clean
make
echo "----------first.sh----------";
echo "待处理文件相对位置:$1"
echo "结点数$2"
echo "边数$3"

./handleDict $1 dict.txt
echo "dict.txt successfully!"

./handleInputSort $1 datasmall.txt $2 $3
echo "datasmallSort.txt successfully!"

make clean