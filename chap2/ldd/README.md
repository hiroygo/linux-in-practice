* `ldd` コマンドで共有ライブラリへの依存関係を表示する
```
$ go build hello.go 
$ ldd ./hello
        動的実行ファイルではありません

$ cc -o hello hello.c 
$ ldd ./hello
        linux-vdso.so.1 (0x00007ffddd7fa000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f281ba5d000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f281bc6a000)
```
