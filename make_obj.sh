#!/bin/sh
CHAINOBJ=$1
ELFFILE=$2
TARGET=$3

if [ -f ${TARGET} ];then
    echo "---删除原有的旧反汇编文件[${TARGET}]---"
    rm ${TARGET}
fi

${CHAINOBJ} -S ${ELFFILE} > ${TARGET}

echo "---生成反汇编文件[${TARGET}]成功---"
