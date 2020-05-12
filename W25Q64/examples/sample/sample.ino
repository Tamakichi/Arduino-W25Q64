//
// SPI シリアルフラッシュメモリ W25Q64 操作検証プログラム
// W25Q64のメモリ領域構造
//   総バイト数 8388608
//   メモリ空間 24ビットアドレス指定 0x00000 - 0x7FFFFF 
//   ブロック数 128 (64KB/ブロック)
//   セクタ数 2048  ( 4KB/セクタ)
//   総セクタ数 2048
//
//  修正 2020/05/12, Unique ID取得の修正
//

#include <string.h>
#include <W25Q64.h>

//
// 書込みデータのダンプリスト
// dt(in) : データ格納先頭アドレス
// n(in)  : 表示データ数
//
void dump(byte *dt, uint32_t n) {
  char buf[64];
  uint16_t clm = 0;
  byte data;
  byte sum;
  byte vsum[16];
  byte total =0;
  uint32_t saddr =0;
  uint32_t eaddr =n-1;
  
  Serial.println("----------------------------------------------------------");
  for (uint16_t i=0;i<16;i++) vsum[i]=0;  
  for (uint32_t addr = saddr; addr <= eaddr; addr++) {
    data = dt[addr];
    if (clm == 0) {
      sum =0;
      sprintf(buf,"%05lx: ",addr);
      Serial.print(buf);
    }

    sum+=data;
    vsum[addr % 16]+=data;
    
    sprintf(buf,"%02x ",data);
    Serial.print(buf);
    clm++;
    if (clm == 16) {
      sprintf(buf,"|%02x ",sum);
      Serial.print(buf);      
      Serial.println("");
      clm = 0;
    }    
  }
  Serial.println("----------------------------------------------------------");
  Serial.print("       ");
  for (uint16_t i=0; i<16;i++) {
    total+=vsum[i];
    sprintf(buf,"%02x ",vsum[i]);
    Serial.print(buf);
  }
  sprintf(buf,"|%02x ",total);
  Serial.print(buf);      
  Serial.println("");
  Serial.println("");
}

void setup() {
    byte buf[256];        // 取得データ
    byte wdata[16];       // 書込みデータ
    
    uint16_t n;           // 取得データ数
    
    W25Q64_begin(10);     // フラッシュメモリ利用開始
    Serial.begin(115200);
    
    // JEDEC IDの取得テスト
    W25Q64_readManufacturer(buf);
    Serial.print("JEDEC ID : ");
    for (byte i=0; i< 3; i++) {
      Serial.print(buf[i],HEX);
      Serial.print(" ");
    }
    Serial.println("");
    
    // Unique IDの取得テスト
    memset(buf,0,8);
    W25Q64_readUniqieID(buf);
    Serial.print("Unique ID : ");
    for (byte i=0; i< 8; i++) {
      Serial.print(buf[i],HEX);
      Serial.print(" ");
    }
    Serial.println("");
    
    // データの読み込み(アドレス0から256バイト取得)
    memset(buf,0,256);
    n =  W25Q64_read(0,buf, 256);
    Serial.print("Read Data: n=");
    Serial.println(n,DEC);
    dump(buf,256);

    // 高速データの読み込み(アドレス0から256バイト取得)
    memset(buf,0,256);
    n =  W25Q64_fastread(0,buf, 256);
    Serial.print("Fast Read Data: n=");
    Serial.println(n,DEC);
    dump(buf,256);

    // セクタ単位の削除
    n = W25Q64_eraseSector(0,true);
    Serial.print("Erase Sector(0): n=");
    Serial.println(n,DEC);
    memset(buf,0,256);
    n =  W25Q64_read (0,buf, 256);
    dump(buf,256);
 
    // データ書き込みテスト
    for (byte i=0; i < 16;i++) {
      wdata[i]='A'+i;
    }  
    n =  W25Q64_pageWrite(0, 10, wdata, 16);
    Serial.print("page_write(0,10,d,16): n=");
    Serial.println(n,DEC);
    memset(buf,0,256);
    n =  W25Q64_read (0,buf, 256);
    dump(buf,256);

    // ステータスレジスタ1の取得
    buf[0] = W25Q64_readStatusReg1();
    Serial.print("Status Register-1: ");
    Serial.print(buf[0],BIN);
    Serial.println("");

    // ステータスレジスタ2の取得
    buf[0] = W25Q64_readStatusReg2();
    Serial.print("Status Register-2: ");
    Serial.print(buf[0],BIN);
    Serial.println("");
}

void loop() {

}
