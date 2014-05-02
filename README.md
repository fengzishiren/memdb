使用C99实现的简单NoSql
=====


协议：redis协议

语言：C99

编译器：gcc、clang均测试通过 其他编译器上未进行测试


目前支持的数据类型：字符串、整形、列表

支持的操作：写入、读取和删除操作

具体支持命令：GET、SET、DEL、INCR、DECR、LPUSH、LINDEX、LRANGE


支持redis官方各语言版客户端以及redis自带交互式命令行工具redis-cli

当然也可以使用我的另一个项目pyredis（为redis写的python客户端，尚不成熟）来作为通讯客户端

目前该项目测试使用redis官方测试工具以及redis-cli工具

