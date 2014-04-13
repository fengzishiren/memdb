# -*- coding:utf-8 -*-
'''
做两件事情：
生成redis命令的待发送字符串形式
解析redis返回的字符串


Created on 2014年4月2日

@author: Lunatic Poet
'''
import socket


class Connection(object):
    def __init__(self, host, port=6379, timeout=None):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.settimeout(timeout)
        self.socket.connect((host, port))
        
    @classmethod
    def connect(cls, host, port, timeout):
        return cls(host, port, timeout)
    
    def send(self, cmd):
        # print 'request:', str(cmd)
        self.socket.send(str(cmd))
        
    def read(self):
        return self.socket.recv(2048)
    
    def close(self):
        self.socket.close()
        


class Command(object):
    '''
    *<参数数量> CR LF
    $<参数 1 的字节数量> CR LF
    <参数 1 的数据> CR LF
    ...
    $<参数 N 的字节数量> CR LF
    <参数 N 的数据> CR LF
    
    *3
    $3
    SET
    $5
    mykey
    $7
    myvalue
    
    这个命令的实际协议值如下：

    "*3\r\n$3\r\nSET\r\n$5\r\nmykey\r\n$7\r\nmyvalue\r\n"
    '''
    
    suffix = '\r\n'
    
    def __init__(self, cmd=None):
        self.args = []
        if cmd:
            self.args = self.parse(cmd)
        
    def append(self, arg):
        self.args.append(arg)
    
    def parse(self, cmd):
        '''
        SET name key
        '''
        return str(cmd).split(' ')

    
    @classmethod
    def verify(cls, cmd):
        '''
        类似：*3\r\n$3\r\nSET\r\n$5\r\nmykey\r\n$7\r\nmyvalue\r\n
        split:*3\r\n, 3\r\nSET\r\n,  5\r\nmykey\r\n, 7\r\nmyvalue\r\n
        '''  
        try:
            cmd = str(cmd)
            # 不是以*开头
            if not cmd.startswith('*'):
                raise Exception()
            
            args = cmd.split('$')
            head = args[0]
            start = head.find(cls.suffix)
            
            if start == -1:  # 没有后缀\r\n
                raise Exception()
            
            # ignore '*'
            count = int(head[1:start])
            
            if args.__len__() - 1 != count:
                raise Exception()
           
            for arg in args[1:args.__len__()]:
                cn = arg.split(cls.suffix)
                # eg.['3', 'SET', '']
                if cn.__len__() != 3 or int(cn[0]) != cn[1].__len__():
                    raise Exception()
            return True
        except:
            # catch all exceptions
            # translate to CmdErr
            # raise CmdErr('redis 命令错误!\n\t -' + cmd)
            return False
    
    def __str__(self):
        
        ret = '*' + str(self.args.__len__()) + self.suffix
        for arg in self.args:
            ret = ret + '$' + str(len(arg)) + self.suffix + arg + self.suffix
        return ret

class Parser(object):
    '''
    通过检查服务器发回数据的第一个字节， 可以确定这个回复是什么类型：
    
    状态回复（status reply）的第一个字节是 "+"
    错误回复（error reply）的第一个字节是 "-"
    整数回复（__integer reply）的第一个字节是 ":"
    批量回复（__bulk reply）的第一个字节是 "$"
    多条批量回复（__multi __bulk reply）的第一个字节是 "*"
'''
    suffix = '\r\n'
    def __init__(self, reply):
        # print 'reply', reply, 'end'
        resp_type = {'+':self.__stat, '-':self.__err, ':':self.__integer,
                     '$':self.__bulk, '*':self.__multi}
        pos = reply.find(self.suffix)
        if pos == -1:
            return
        self.exe = resp_type[reply[0]]
        self.reply = reply
        
    def parse(self):
        if self.exe:
            return self.exe(self.reply)
        return Result(None)
        
    def __stat(self, reply):
        ret = reply[1:reply.__len__() - self.suffix.__len__()]
        return Result(ret)
        
    def __err(self, reply):
        ret = reply[1:reply.__len__() - self.suffix.__len__()]
        return Result(ret, True)
        
    __integer = __stat
        
    def __bulk(self, reply):
        items = reply[1:reply.__len__()].split(self.suffix)
        _len = int(items[0])
        data = items[1]
        if _len == data.__len__():
            return Result(data)
        else:
            return Result(None)
            
    def __multi(self, reply):
        items = reply[1:reply.__len__()].split(self.suffix)
        _len = int(items[0])
        # ignore *
        index = 1
        lineno = 0
        retval = ''
        for item in items[1:items.__len__() - 1]:
            if item[0] == ':':
                lineno = lineno + 1
                retval = retval + str(lineno) + ') ' + retval + item[1:items.__len__()] + '\n'
            elif item[0] == '$':
                lineno = lineno + 1
                retval = retval + str(lineno) + ') ' + items[index + 1] + '\n'
            index = index + 1
        return Result(retval[0:-1])  # note '\n'

class Result(object):
    def __init__(self, msg, err=False):
        self.msg = msg
        self.err = err
    def __str__(self):
        return self.msg

if __name__ == '__main__':
    # AutoLock()
    con = Connection("localhost")
    while True:
        cmd = raw_input(">>>");
        con.send(Command(cmd))
        ret = con.read()
        data = Parser(ret).parse()
        print data
  
    pass
