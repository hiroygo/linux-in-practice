* strace でシステムコールを確認する
* `-o` で結果をファイルに書き出す
* `-T` で各システムコールの処理にかかった時間を出力する
```
$ cc -o hello hello.c
$ strace -o hello.c.log ./hello 
$ strace -o hello.py.log python3 ./hello.py 
$ strace -T -o hello.c.log ./hello 
```
