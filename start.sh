echo "执行的文件名：$0" "参数个数为 $#" 

if [ "$#" -ne 1 ];
then
    echo "输入的参数为 $*";
    echo "应当输入一个参数以选择功能" 
    echo "参数可选范围[init/run]" 
    exit 0;
fi

if [ "$1"x = "init"x ];
then
    echo "执行Init初始化" 
    echo "----------检测build文件夹状态----------" ;
    if [ ! -d "./build/" ];then
        echo "----------build文件夹不存在----------" ;
    else
        echo "----------build文件夹存在----------" ;
        echo "----------开始删除build文件夹----------" ;
        rm -rf build
        echo "----------删除build文件夹成功----------" ;
    fi
    echo "----------开始创建build文件夹----------" ;
    mkdir build
    echo "----------创建build文件夹完成----------" ;
    echo "----------开始更新models文件夹----------" ;
    echo y | dg_ctl create model models
    echo "----------更新models文件夹完成----------" ;


elif [ "$1"x = "run"x ];
then
    echo "----------进入build文件夹----------" ;
    cd ./build
    echo "----------开始编译cmake----------" ;
    cmake ..
    clear
    echo "----------开始编译make----------" ;
    make
    clear
    echo "----------开始运行----------" ;
    ./Novel_platform
    echo "----------结束运行----------" ;
    echo "----------退出build文件夹----------" ;
    cd ..

elif [ "$1"x = "close"x ];
then
    # 设置项目名
    echo "----------设置项目名----------" ;
    process_name="Novel_platform"
    # 获取Pid
    echo "----------获取Pid----------" ;
    pid=$(pidof $process_name)
    
    if [ "$pid" = "" ] ; then
        echo "process not exist"
    else
        echo "should kill the process"
        echo "the pid = $pid"
        echo "----------开始停止进程----------" ;
        kill -9 $pid
    fi
else 
    echo "参数不在可选范围内[init/run]" 
fi











# 文件比较符

# -e 判断对象是否存在
# -d 判断对象是否存在，并且为目录
# -f 判断对象是否存在，并且为常规文件
# -L 判断对象是否存在，并且为符号链接
# -h 判断对象是否存在，并且为软链接
# -s 判断对象是否存在，并且长度不为0
# -r 判断对象是否存在，并且可读
# -w 判断对象是否存在，并且可写
# -x 判断对象是否存在，并且可执行
# -O 判断对象是否存在，并且属于当前用户
# -G 判断对象是否存在，并且属于当前用户组
# -nt 判断file1是否比file2新  [ "/data/file1" -nt "/data/file2" ]
# -ot 判断file1是否比file2旧  [ "/data/file1" -ot "/data/file2" ]