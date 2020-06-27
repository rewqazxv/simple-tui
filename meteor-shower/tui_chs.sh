#!/bin/bash

# 参数设定
r=25
t=5
e='#'
m=''
s=7
l=4
n=10
# 1为真 0为假
flagN=0
flagf=0
strflag[0]="关"
strflag[1]="开"

inputbox() {
    reply=`dialog --stdout --inputbox "$1" 0 0`
}

msgbox() {
    dialog --stdout --msgbox "$1" 0 0
}

settings() {
local res=`dialog --stdout --title "设置" \
    --cancel-label "返回" --menu "" 6 0 0 \
    1 "刷新频率  [$r]" \
    2 "地面厚度  [$t]" \
    3 "地面字符  ['$e']" \
    4 "流星字符  [${m:+"'"}${m:-"随机"}${m:+"'"}]" \
    5 "流星速度  [$s]" \
    6 "速度分级  [$l]" \
    7 "流星个数  [$n]" \
    8 "禁用颜色  [${strflag[$flagN]}]" \
    9 "永不停止  [${strflag[$flagf]}]"`
case $res in
    (1) if inputbox "设定刷新率"; then 
            if [[ `echo "$reply > 0" | bc 2>&1` == 1 ]]; then
                r=`echo $reply | bc`
            else
                msgbox "输入有误: 刷新率是浮点数且要大于0."
            fi
        fi;;
    (2) if inputbox "设定地面厚度(过大将无法开始演示)"; then
            if ((reply>=0)) 2>/dev/null; then
                t=$((reply))
            else
                msgbox "输入有误: 地面厚度是整数且大于等于0."
            fi
        fi;;
    (3) if inputbox "设定地面字符(最多取1位)"; then
            e=${reply:0:1}
        fi;;
    (4) if inputbox "设定流星字符(最多取1位, 留空使用随机大写字母)"; then
            m=${reply:0:1}
        fi;;
    (5) if inputbox "设定流星速度"; then 
            if [[ `echo "$reply > 0" | bc 2>&1` == 1 ]]; then
                s=`echo $reply | bc`
            else
                msgbox "输入有误: 流星速度是浮点数且要大于0."
            fi
        fi;;
    (6) if inputbox "设定速度分级"; then
            if ((reply>0)) 2>/dev/null; then
                l=$((reply))
            else
                msgbox "输入有误: 速度分级是整数且大于0."
            fi
        fi;;
    (7) if inputbox "设定流星个数(过大将无法开始演示)"; then
            if ((reply>0)) 2>/dev/null; then
                n=$((reply))
            else
                msgbox "输入有误: 流星个数是整数且大于0."
            fi
        fi;;
    (8) flagN=$((! flagN));;
    (9) flagf=$((! flagf));;
    (*) return 1;
esac
}

menu() {
local res=`dialog --stdout --title "流星雨" \
    --cancel-label "退出" --menu "" 6 0 0 \
    1 "开始" \
    2 "设置"`
case $res in
    (1) ./meteor-shower -r $r -t $t -e "$e" -m "$m" -s $s -l $l -n $n `if ((flagN)); then echo "-N"; fi` `if ((flagf)); then echo "-f"; fi`;;
    (2) while settings; do :; done;;
    (*) return 67
esac
}

if ! type -t dialog 2>&1 >/dev/null; then
    echo "错误: 找不到程序'dialog'" >&2
    exit 4
elif ! [[ -x ./meteor-shower ]]; then
    msgbox "错误: 找不到可执行文件'meteor-shower'"
    exit 5
elif ! type -t bc 2>&1 >/dev/null; then
    msgbox "错误: 需要程序'bc'但没有找到"
    exit 6
fi

while true; do
    menu
    statue=$?
    if ((statue==0)); then
        continue
    elif ((statue==32)); then 
        msgbox "错误: 无法显示全部内容. 可能是流星个数过多或地面厚度过大, 或终端尺寸过小."
    elif ((statue==67)); then
        exit 0
    else
        exit $statue
    fi
done
