# SimpleKeyValueDb
## 前言
一个单机简易内存键值存储，键值对存储部分参考《redis设计与实现》。将存储部分作为服务，在本地5481端口，网络部分实现参考陈硕的mudo。代码风格为C with class，使用了部分modern C++特性,代码总量为3600行，算是一个C++的小项目。由于网络部分的限制，只能在unix环境下使用。

## 键值对存储
### 数据类型
* 字符串
* 列表
* 集合
* 有序集合
* 哈希
### 底层数据结构
* 字符串，直接采用std::string,std::string默认内存分配策略是两倍指数增长，redis 字符串是字符串大小为1M以下采用两倍策略，1M以上时每次重新分配空间都只增加1M
* 列表，采用双向链表实现，具体实现参见 core/List.hpp
* 集合，采用std::unordered_set实现.redis 是使用字典实现，只不过dict的value部分为空，只使用key部分存储；这两种底层实现都是使用的hashtable，只不过redis的hashtable的rehash策略有所不同，std::unordered_set直接新建一个新的hashtable，将旧的hashtable中的内容全部一次性复制到新的表中。redis则是新建一个hashtable，新值的插入则插入到新表中，每一次查询过程都将旧表中的内容复制到新表，直到完成。
* 有序集合，仿照redis采用跳跃表实现，具体实现参见 core/SkipList.hpp
* 哈希，采用std::unordered_map实现，redis使用字典实现，底层都是hashtable，区别不再赘述。
## 指令
目前只实现了简易的指令
### 字符串
* set key value
* get key
* strlen key
* getrange key start end
### 列表
* lpush key value
* lpop key
* lgetall key
* llen key
### 集合
* sadd key value
* sdel key value
* sgetall key
### 有序集合
* oadd key score member
* odel key socre
* ogetall key
### 哈希
* hadd key field1 field2
* hdel key field1
* hgetall key
### 数据库层面指令
* select number
* del key
* expire key seconds
* pexpire key milliseconds
* save filename
* load filename  
#### 注
* select是选择当前数据库，默认为0
* expire是设定键过期指令，秒为单位，采用惰性删除策略。pexpire是设定毫秒过期时间，由于存储的是time_t，事实上毫秒会转化成秒，所以目前没有毫秒精度，有时间再更改。
* save是将当前数据库中的内容存储到指定文件中，文件内容没有压缩，是直接可读的文本内容,不过在整数存储上对int和long之类的进行了优化，前一个字节存放数字编码长度，后面紧跟的几个字节存放数字内容。
* load是将指定文件中的内容加载到当前数据库中

指令识别采用string比较，效率较低，可以改成通过哈希进行映射。

## 网络
### 实现
参考陈硕的mudo，使用epoll，reactor模式，没有官方网络库的C++写起来略微难受，不过借此机会熟悉了一把epoll。使用epoll监听事件，但是事件执行是单线程串行的，具体参考 net 目录下，略微有点混乱。

## 编译
基本上所有的代码都写在以 .hpp 为后缀的文件中，在根目录下直接make即可生成可执行文件，client可执行文件在 client 目录下，server可执行文件在 server 目录下。client和server非常简陋，仅仅是一个输入，一个执行并返回结果。server没有日志，也不是守护进程，所有信息都直接打印到控制台。

