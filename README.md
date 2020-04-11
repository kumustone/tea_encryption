# tea_encryption

Tea加解密c++ 版实现，接口简单，内部对padding已经做了封装，只需要调用接口即可；

```
bool encrypt(const void *input, int input_len, DataBuffer &out);

bool decrypt(const void *input, int input_len, DataBuffer &out);
```

编译运行：
> g++ demo.cpp encryption.cpp -o demo

> ./demo
