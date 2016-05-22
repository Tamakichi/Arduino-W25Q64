# Arduino-W25Q64
Arduino用 SPI接続フラッシュメモリW25Q64ライブラリ

![結線図](img/demo.jpg)

##概要
Arduinoでwinbond社製 SPI接続フラッシュメモリ**W25Q64**を利用するためのライブラリです。  
**W25Q32**でも利用出来ます。  

![W25Q64 DIP](img/W25Q64.jpg)


ライブラリ名称: **W25Q64**  
ヘッダファイル: `#include <W25Q64.h>`  

##インストール
**W25Q64**フォルダを**Arduino IDE**の**libraries**フォルダに置いて下さい。  

##回路図
**W25Q64**は3.3V稼働のため、5V稼働のArduinoで利用する場合はレベルシフト等の対応を行って下さい。  
(電源部のパスコンは省略しても動くようです)  

![回路図](img/Schematic1.png)
![接続図](img/Schematic2.png)

##サンプルプログラム
スケッチ名：**sample.ino**  
**W25Q64**に対して、書き込みと読み込みを行いその結果を表示します。  

シリアル通信の速度は115200で接続します。  
![接続図](img/sample_console.png)

