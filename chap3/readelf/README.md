# readelf
* 開始アドレスを得るには `-h`
```
$ readelf -h /bin/sleep
ELF ヘッダ:
  マジック:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  クラス:                            ELF64
  データ:                            2 の補数、リトルエンディアン
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI バージョン:                    0
  型:                                DYN (共有オブジェクトファイル)
  マシン:                            Advanced Micro Devices X86-64
  バージョン:                        0x1
  エントリポイントアドレス:               0x2850
  プログラムヘッダ始点:          64 (バイト)
  セクションヘッダ始点:          37336 (バイト)
  フラグ:                            0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         13
  Size of section headers:           64 (bytes)
  Number of section headers:         30
  Section header string table index: 29
```
