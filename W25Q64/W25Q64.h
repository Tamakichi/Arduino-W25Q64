//
// フラッシュメモリW25Q64 からのフォントデータ取得
// 作成 2014/10/14
// 修正 2016/05/21
//

#include <arduino.h>
#include <SPI.h>

void W25Q64_begin(uint8_t cs=10); 		// フラッシュメモリ W25Q64の利用開始
void W25Q64_end();						// フラッシュメモリ W25Q64の利用終了
void W25Q64_select();					// チップセレクト フラッシュメモリ操作を選択にする
void W25Q64_deselect(); 				// チップディセレクト フラッシュメモリ操作を有非選択にする
byte W25Q64_readStatusReg1();			// ステータスレジスタ1の値取得
byte W25Q64_readStatusReg2(); 			// ステータスレジスタ2の値取得
void W25Q64_readManufacturer(byte* d); 	// JEDEC ID(Manufacture, Memory Type,Capacity)の取得
void W25Q64_readUniqieID(byte* d); 		// Unique IDの取得
boolean W25Q64_IsBusy(); 				// 書込み等の処理中チェック
void W25Q64_powerDown(); 				// パワーダウン指定 
void W25Q64_WriteEnable(); 				// 書込み許可設定
void W25Q64_WriteDisable();				// 書込み禁止設定

uint16_t W25Q64_read(uint32_t addr,uint8_t *buf,uint16_t n);		// データの読み込み
uint16_t W25Q64_fastread(uint32_t addr,uint8_t *buf,uint16_t n); 	// 高速データの読み込み
boolean  W25Q64_eraseSector(uint16_t sect_no, boolean flgwait);		// セクタ単位消去
boolean  W25Q64_erase64Block(uint16_t blk_no, boolean flgwait);		// 64KBブロック単位消去
boolean  W25Q64_erase32Block(uint16_t blk_no, boolean flgwait);		// 32KBブロック単位消去
boolean  W25Q64_eraseAll(boolean flgwait);							// 全領域の消去
uint16_t W25Q64_pageWrite(uint16_t sect_no, uint16_t inaddr, byte* data, byte n); // データの書き込み
