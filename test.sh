#!/bin/bash

echo "---------start shell-----" 
echo "获取 $1 目录下的访问序列"
echo "使用 $2 程序进行测试"
echo "分别以RS(3,1)RS(4,2)RS(6,3)RS(8,4)作为参数"




for file in `ls $1"/"*_[^0].csv`
do


    echo "正在执行 ${file} 3 1 "

    echo $(date)>> test.log
    echo "${file} 3 1 " >> test.log
    $2 ${file} 3 1  >> test.log
    echo -e '\n\n\n' >> test.log

    echo "正在执行 ${file} 4 2 "
    echo $(date)>> test.log
    echo "${file} 4 2 " >> test.log
    $2 ${file} 4 2  >> test.log
    echo -e '\n\n\n'>> test.log

    echo "正在执行 ${file} 6 3 "
    echo $(date)>> test.log
    echo "${file} 6 3 " >> test.log
    $2 ${file} 6 3  >> test.log
    echo -e '\n\n\n'>> test.log

    echo "正在执行 ${file} 8 4 "
    echo $(date)>> test.log
    echo "${file} 8 4 " >> test.log
    $2 ${file} 8 4  >> test.log
    echo -e '\n\n\n'>> test.log

done
