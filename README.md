# PHP hash collision finder

This is a tool written in C++, which demonstrates hash collisions (currently only md5) modulo PHP equivalence (== comparison operator). That is, it searches the string $s, such as:
`strlen($s) == $n && md5($s) == '0'`

This property can be exploited in various signature verification algorithms, and this tool was written for the purpose of vulnerability demonstration without sending a lot of web requests.

Compile it with g++ or with clang++ (some g++ versions cannot compile this due to some bugs). FROM_CHAR and TO_CHAR macros define the brute force alphabet.

Example of usage:
```
$ ./hashcol asdINJ123 4
Started thread from: 32
Started thread from: 56
Started thread from: 80
Started thread from: 104
FOUND 96 40 47 51 
$ php -r 'var_dump(md5("asd".chr(96).chr(40).chr(47).chr(51)."123")=="0");'
bool(true)
```
