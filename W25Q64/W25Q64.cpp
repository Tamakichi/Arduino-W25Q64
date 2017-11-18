//
// フラッシュメモリW25Q64 からのフォントデータ取得
// 作成 2014/10/14
// 修正 2016/05/21
// 修正 2017/11/18,Arduin STM32対応
//

#include <arduino.h>
#include <SPI.h>
#include <W25Q64.h>

#define SPI_SLAVE_SEL_PIN    10     // チップセレクトピン番号
#define MAX_BLOCKSIZE        128    // ブロック総数
#define MAX_SECTORSIZE       2048   // 総セクタ数

#define CMD_WRIRE_ENABLE      0x06
#define CMD_WRITE_DISABLE     0x04
#define CMD_READ_STATUS_R1    0x05
#define CMD_READ_STATUS_R2    0x35
#define CMD_WRITE_STATUS_R    0x01 // 未実装
#define CMD_PAGE_PROGRAM      0x02
#define CMD_QUAD_PAGE_PROGRAM 0x32 // 未実装
#define CMD_BLOCK_ERASE64KB   0xd8
#define CMD_BLOCK_ERASE32KB   0x52
#define CMD_SECTOR_ERASE      0x20
#define CMD_CHIP_ERASE        0xC7
#define CMD_ERASE_SUPPEND     0x75 // 未実装
#define CMD_ERASE_RESUME      0x7A // 未実装
#define CMD_POWER_DOWN        0xB9
#define CMD_HIGH_PERFORM_MODE 0xA3 // 未実装
#define CMD_CNT_READ_MODE_RST 0xFF // 未実装
#define CMD_RELEASE_PDOWN_ID  0xAB // 未実装
#define CMD_MANUFACURER_ID    0x90
#define CMD_READ_UNIQUE_ID    0x4B
#define CMD_JEDEC_ID          0x9f

#define CMD_READ_DATA         0x03
#define CMD_FAST_READ         0x0B
#define CMD_READ_DUAL_OUTPUT  0x3B // 未実装
#define CMD_READ_DUAL_IO      0xBB // 未実装
#define CMD_READ_QUAD_OUTPUT  0x6B // 未実装
#define CMD_READ_QUAD_IO      0xEB // 未実装
#define CMD_WORD_READ         0xE3 // 未実装

#define SR1_BUSY_MASK	0x01
#define SR1_WEN_MASK	0x02

static uint8_t cspin ;

//
// フラッシュメモリ W25Q64の利用開始
// 
void W25Q64_begin(uint8_t cs) {
	cspin = cs;
    pinMode (cspin, OUTPUT);
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
#if defined (__STM32F1__)
    SPI.setClockDivider(SPI_CLOCK_DIV8); // 2014.10.1 DIV2 からDIV4に変更
#else
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 2014.10.1 DIV2 からDIV4に変更
#endif
    SPI.setDataMode(SPI_MODE0);
    W25Q64_deselect();
}

//
// フラッシュメモリ W25Q64の利用終了
// 
void W25Q64_end() {
  W25Q64_powerDown();
   SPI.end();
}

//
// チップセレクト
// フラッシュメモリ操作を選択にする
//
void W25Q64_select() {
   digitalWrite(cspin, LOW); 
}

//
// チップディセレクト
// フラッシュメモリ操作を有非選択にする
//
void W25Q64_deselect() {
   digitalWrite(cspin, HIGH); 
}

//
// ステータスレジスタ1の値取得
// 戻り値: ステータスレジスタ1の値
//
byte W25Q64_readStatusReg1() {
  byte rc;
  W25Q64_select();
  SPI.transfer(CMD_READ_STATUS_R1);
  rc = SPI.transfer(0xFF);
  W25Q64_deselect();
  return rc;
}

//
// ステータスレジスタ2の値取得
// 戻り値: ステータスレジスタ2の値
//
byte W25Q64_readStatusReg2() {
  byte rc;
  W25Q64_select();
  SPI.transfer(CMD_READ_STATUS_R2);
  rc = SPI.transfer(0xFF);
  W25Q64_deselect();
  return rc;
}

//
// JEDEC ID(Manufacture, Memory Type,Capacity)の取得
// d(out) :Manufacture, Memory Type,Capacityの３バイトを格納する
//
void W25Q64_readManufacturer(byte* d) {
  W25Q64_select();
  SPI.transfer(CMD_JEDEC_ID);
  for (byte i =0; i <3; i++) {
    d[i] = SPI.transfer(0x00);
  } 
  W25Q64_deselect();
}

//
// Unique IDの取得
// d(out): Unique ID 7バイトを返す  
//
void W25Q64_readUniqieID(byte* d) {
  W25Q64_select();
  SPI.transfer(CMD_READ_UNIQUE_ID);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  for (byte i =0; i <7; i++) {
    d[i] = SPI.transfer(0x00);
  }
 W25Q64_deselect(); 
}

//
// 書込み等の処理中チェック
// 戻り値: true:作業 、false:アイドル中
//
boolean W25Q64_IsBusy() {
  uint8_t r1;
  W25Q64_select();
  SPI.transfer(CMD_READ_STATUS_R1);
  r1 = SPI.transfer(0xff);
  W25Q64_deselect();
  if(r1 & SR1_BUSY_MASK)
    return true;
  return false;
}

//
//　パワーダウン指定 
//
void W25Q64_powerDown() {
  W25Q64_select();
  SPI.transfer(CMD_POWER_DOWN);
  W25Q64_deselect();
}

//
// 書込み許可設定
//
void W25Q64_WriteEnable() {
  W25Q64_select();
  SPI.transfer(CMD_WRIRE_ENABLE);
  W25Q64_deselect();
}

//
// 書込み禁止設定
//
void W25Q64_WriteDisable() {
  W25Q64_select();
  SPI.transfer(CMD_WRITE_DISABLE);
  W25Q64_deselect();
}

//
// データの読み込み
// addr(in): 読込開始アドレス (24ビット 0x00000 - 0xFFFFF)
// n(in):読込データ数
//
uint16_t W25Q64_read(uint32_t addr,uint8_t *buf,uint16_t n){ 
  W25Q64_select();
  SPI.transfer(CMD_READ_DATA);
  SPI.transfer(addr>>16);          // A23-A16
  SPI.transfer((addr>>8) & 0xFF);  // A15-A08
  SPI.transfer(addr & 0xFF);       // A07-A00
 
  uint16_t i;
  for(i = 0; i<n; i++ ) {
    buf[i] = SPI.transfer(0x00);
  }
  
  W25Q64_deselect();
  return i;
}

//
// 高速データの読み込み
// addr(in): 読込開始アドレス (24ビット 0x00000 - 0xFFFFF)
// n(in):読込データ数
//
uint16_t W25Q64_fastread(uint32_t addr,uint8_t *buf,uint16_t n) {
  W25Q64_select();
  SPI.transfer(CMD_FAST_READ);
  SPI.transfer(addr>>16);          // A23-A16
  SPI.transfer((addr>>8) & 0xFF);  // A15-A08
  SPI.transfer(addr & 0xFF);       // A07-A00
  SPI.transfer(0x00);              // ダミー
  
  uint16_t i;
  for(i = 0; i<n; i++)
    buf[i] = SPI.transfer(0x00);
  
  W25Q64_deselect();  
  return i;
}

//
// セクタ単位消去(4kb空間単位でデータの消去を行う)
// sect_no(in) セクタ番号(0 - 2048)
// flgwait(in) true:処理待ちを行う false:待ち無し
// 戻り値: true:正常終了 false:失敗
//  補足： データシートでは消去に通常 30ms 、最大400msかかると記載されている
//         アドレス23ビットのうち上位 11ビットがセクタ番号の相当する。下位12ビットはセクタ内アドレスとなる。
//
boolean W25Q64_eraseSector(uint16_t sect_no, boolean flgwait) {
 uint32_t addr = sect_no;
 addr<<=12;

 W25Q64_WriteEnable();
 W25Q64_select(); 
 SPI.transfer(CMD_SECTOR_ERASE);
 SPI.transfer((addr>>16) & 0xff);
 SPI.transfer((addr>>8) & 0xff);
 SPI.transfer(addr & 0xff);
 W25Q64_deselect();
 
 // 処理待ち
 while(W25Q64_IsBusy() & flgwait) {
    delay(10);
 }
 
 return true;
}

//
// 64KBブロック単位消去(64kb空間単位でデータの消去を行う)
// blk_no(in) ブロック番号(0 - 127)
// flgwait(in) true:処理待ちを行う false:待ち無し
// 戻り値: true:正常終了 false:失敗
//   補足: データシートでは消去に通常 150ms 、最大1000msかかると記載されている
//         アドレス23ビットのうち上位 7ビットがブロックの相当する。下位16ビットはブロック内アドレスとなる。
//
boolean W25Q64_erase64Block(uint16_t blk_no, boolean flgwait) {
 uint32_t addr = blk_no;
 addr<<=16;

 W25Q64_WriteEnable();
 W25Q64_select(); 
 SPI.transfer(CMD_BLOCK_ERASE64KB);
 SPI.transfer((addr>>16) & 0xff);
 SPI.transfer((addr>>8) & 0xff);
 SPI.transfer(addr & 0xff);
 W25Q64_deselect();
 
 // 処理待ち
 while(W25Q64_IsBusy() & flgwait) {
    delay(50);
 }
 
 return true;
}

//
// 32KBブロック単位消去(32kb空間単位でデータの消去を行う)
// blk_no(in) ブロック番号(0 - 255)
// flgwait(in) true:処理待ちを行う false:待ち無し
// 戻り値: true:正常終了 false:失敗
//   補足: データシートでは消去に通常 120ms 、最大800msかかると記載されている
//         アドレス23ビットのうち上位 8ビットがブロックの相当する。下位15ビットはブロック内アドレスとなる。
//
boolean W25Q64_erase32Block(uint16_t blk_no, boolean flgwait) {

 uint32_t addr = blk_no;
 addr<<=15;

 W25Q64_WriteEnable();  
 W25Q64_select(); 
 SPI.transfer(CMD_BLOCK_ERASE32KB);
 SPI.transfer((addr>>16) & 0xff);
 SPI.transfer((addr>>8) & 0xff);
 SPI.transfer(addr & 0xff);
 W25Q64_deselect();
 
 // 処理待ち
 while(W25Q64_IsBusy() & flgwait) {
    delay(50);
 }
 
 return true;
}

//
// 全領域の消去
// flgwait(in) true:処理待ちを行う false:待ち無し
// 戻り値: true:正常終了 false:失敗
//   補足: データシートでは消去に通常 15s 、最大30sかかると記載されている
//
boolean W25Q64_eraseAll(boolean flgwait) {
 W25Q64_WriteEnable();  
 W25Q64_select(); 
 SPI.transfer(CMD_CHIP_ERASE);
 W25Q64_deselect();

 // 処理待ち
 while(W25Q64_IsBusy() & flgwait) {
    delay(500);
 }
 
 W25Q64_deselect();
 return true;
}

//
// データの書き込み
// sect_no(in) : セクタ番号(0x00 - 0x7FF) 
// inaddr(in)  : セクタ内アドレス(0x00-0xFFF)
// data(in)    : 書込みデータ格納アドレス
// n(in)       : 書込みバイト数(0～256)
//
uint16_t W25Q64_pageWrite(uint16_t sect_no, uint16_t inaddr, byte* data, byte n) {

  uint32_t addr = sect_no;
  int i;
  addr<<=12;
  addr += inaddr;

  W25Q64_WriteEnable();  
  
  if (W25Q64_IsBusy()) {
    return 0;  
  }

  W25Q64_select();
  SPI.transfer(CMD_PAGE_PROGRAM);
  SPI.transfer((addr>>16) & 0xff);
  SPI.transfer((addr>>8) & 0xff);
  SPI.transfer(addr & 0xff);

  for (i=0; i < n; i++) {
    SPI.transfer(data[i]);
  }  
  W25Q64_deselect();
  while(W25Q64_IsBusy()) ;
  
  return i;
}
