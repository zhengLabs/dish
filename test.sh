#!/bin/bash

echo "---------start shell-----" 
echo "获取 $1 目录下的访问序列"
echo "使用 $2 程序进行测试"
echo "分别以RS(3,1)RS(4,2)RS(6,3)RS(8,4)作为参数"


for file in `ls $1"/"*.csv`
do

    for i in 1 2 3 4
    do
        if [ $i == 1 ] 
        then 
            j=3
        else 
            j=`expr $i \* 2`
        fi
        echo "正在执行 ${file}  $j $i"
        echo $(date)>> test.log
        echo "${file} $j $i" >> test.log
        $2 ${file} $j $i >> test.log
        echo -e '\n\n\n' >> test.log
    done

done
