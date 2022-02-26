# tingx

## 如何扩展一个模块
- 继承Module类
- 实现ProcessStatus Process(Descriptor* pDescriptor);
- 使用ModuleRegister注册模块



## examples

### 扩展一个echo模块
```c++
// echo_test.cc
#include "core/tingx_socket.h"
#include "core/tingx_config.h"
#include "core/tingx_utilies.h"
#include <iostream>
#include <sys/socket.h>


using namespace tingx;

class EchoTestModule : public Module { // 继承Module类
public:
    EchoTestModule(const char *name, ModuleType type, std::vector<Command>* com) : Module(name, type, com) { }
    virtual ProcessStatus Process(Descriptor* pDescriptor); // 实现process函数
};

ProcessStatus EchoTestModule::Process(Descriptor* pDescriptor) {
    std::string recvbuf(1024, 0);
    int n = read(pSocket->Getfd(), &recvbuf[0], recvbuf.length());
    if (n == 0) {
        return CLOSE;
    } else {
        write(pSocket->Getfd(), &recvbuf[0], recvbuf.length());
    }
    return OK;
}

static ModuleRegister<EchoTestModule> echo_test("EchoTest", ModuleType::NORMAL, nullptr);  // 注册模块
```

配置文件
```
# 设置EchoTest模块监听65535端口
EchoTest { 
    listen  65535;
}
```