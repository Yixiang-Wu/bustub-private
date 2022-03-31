### 项目相关
1. build/test 下运行测试
2. --gtest_also_run_disabled_tests 或者 到XXX_test.cpp的源文件删除DISABLED_XXX前缀
3. 面向测试用例编程，测试用例在XXX_test.cpp文件
4. 异常在common/exception.h查看，必要的时候要throw Exception(int type, string msg)
5. LOG的使用
  1. 一般在XXX_test.cpp文件中包含common/logger.h
  2. 在需要输出查看的地方，假如LOG_DEBUG或者LOG_INFO

------
### CPP相关
1. 父类的析构函数会在子类的析构函数运行后运行，所以不需要重复释放空间（free 2 错误）
2. 空间
  1. new linear_ = new T[row * col];
  2. delete [] linear_;
3. 函数后+const，表示这个函数不允许对于类成员变量进行修改（是一种规范写法，对于getter函数等，都应该在后面加上const）
4. 函数后+override，表示这个函数是重写了父类的函数（是一种规范写法，能够避免手抖，o打成O这样的情况）
5. 函数后+=default，让系统给自己+一个默认函数，一般是构造和析构
6. 父构造->子构造->子析构->父析构
7. 在写一个类之前，在前面加上template <typename T>
------
### 更好看一点
1. public, protected, private
2. public继承不改变，protected继承改public，private继承全部变private（一般用public就OK了）
3. 想用多态，就要虚函数，父加virtual关键字，子默认也加了，但最好显式加上
4. 想要抽象函数，用纯虚
5. 当类中存在一个及以上虚函数时，需要将析构函数也变成虚函数
6. 函数一般public，成员变量一般protected或者private
7. friend,友元，一般情况不使用，container的友元是iterator
8. inline，减少函数调用时间。类中函数都是默认inline
------
### 多态实现
1. vptr
2. 每个类有虚函数表，其指针就是对象的指针
3. new为谁，就到谁的虚函数表去查找
