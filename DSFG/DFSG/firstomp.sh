make clean
make
echo "输入文件位置$1"
echo "基准结果文件位置 /res/res0.txt"
echo "并行结果文件位置 /res/res1.txt"
echo "结点数$2"
echo "边数$3"
echo "遍历起点$4"

echo "------omp------"
./ompDFS $1 ../res/res1.txt $2 $3 $4
echo "------serial------"
./serialDFS $1 ../res/res0.txt $2 $3 $4


make clean