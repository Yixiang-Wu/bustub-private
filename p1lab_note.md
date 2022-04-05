### 调试
1. 按照课程提示，启动调试mode
2. gdb xxx_test
3. b xxx_test.cpp:28
4. 开始愉快调试！
--------
### C++
1. new 出来才可以delete
2. list的使用，想要变成循环链表，就要加额外判断条件（list.end())
3. list只是双向链表，不是循环链表
-------
### 多线程
1. 内核支持线程，用户级线程区别。
2. 进程和线程区别。
3. C++中，thread头文件，std::ref, join和detach，编译时 -pthread， 函数定义&， 传参+ref
4. numeric中 accumulate
5. time中 clock，毫秒
-------
### 线程安全，互斥访问
1. mutex
  1. 声明mutex变量
  2. lock，unlock，try_lock
2. atomic
  1. 将要互斥访问的基本变量声明为atomic类型，例如atomic_int cnt
  2. 像使用基本变量一样使用它
  3. 比mutex效率高
--------
### bug
1. 两个函数加了同一个锁，若是A要调用B
   - 解锁->调用->加锁
2. strcpy -> memcpy
3. 设置meta_data
