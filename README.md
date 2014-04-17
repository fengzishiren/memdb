使用C99实现的简单NoSql
目前支持字符串、整形、列表的写入、存取和删除操作
=====

协议：redis协议

语言：C99

目前支持操作：GET、SET、INCR、DECR、LPUSH、LINDEX、LRANGE

可以使用redis官方各语言版客户端以及redis自带交互式命令行工具redis-cli
目前该项目的测试使用redis官方测试工具以及redis-cli工具
当然也可以使用我的另一个项目pyredis（为redis写的python客户端）来作为通讯客户端

