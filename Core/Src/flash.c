// 	@(#)flash.c		SSG100-0.01		2023/01/10		IOE

#include "global.h"
#include "flash.h"
#include "printf.h"


byte	BlockBuf[BLOCKSIZ];
byte	verifBuf[PAGESIZ];
byte 	sfT = 0;
dword 	FlashId;



BYTE SF_RW_Byte_FLASH(BYTE data);

#if 0

const char FlashHelp[] =
"[option]\n"
" -d [#]        flash debug level(#:0~5)\n"
" -p [#]        flash power down(#:0=power down, 1=resume)\n"
" -i            flash init\n"
" -c            flash id chk\n"
" -s            flash status chk\n"
" -r [#1 #2]    flash read(#1:adr, #2:siz)\n"
" -e [#1 #2]    flash erase(#1:adr, #2:siz)\n"
" -w [#1 #2 #3] flash write(#1:adr, #2:siz, #3:dat)\n";



int FlashSer(void)
{
	int 	opt, i, p, stat = 0;
	siz_t   siz, adr;
	clock_t ec, sc;

	getoptinit();
	while ((opt=getopt(argc,argv,"d:p:icsr:e:w:")) != -1) {
		switch(opt) {
			case 'd' :
				i = aton(optarg);
				if(i < 0 || i > 5) {
					SetError(INVALID_PARA);
					return (-1);
				}
				sfT = i;
				printf("Flash debug level : %d\n", sfT);
				break;

			case 'p' :
				i = aton(optarg);
				if(i==0) {
					printf("Flash power down\n");
					flash_pwr_down();
				}
				else {
					printf("Resume power down\n");
					flash_pwr_resume();
				}
				break;

			case 'i' :
				printf("Flash Init...\n");
				stat = FlashInit();
				if(stat) {
					SetError(FAILED_CMD);
					return(-1);
				}
				break;

			case 'c' :
				i = SpiFlash_ReadInfo();
				if(i != 1) {
					SetError(FAILED_CMD);
					return(-1);
				}
				printf("Flash ID : %#lx\n", FlashId);
				break;

			case 's' :
				i = flash_stat();
				printf("Flash stat : %#x\n", i);
				break;
			case 'e' :
			case 'r' :
			case 'w' :
				p = argc - optind;
				if(opt == 'w') {
					if(p != 2) {
						SetError(INVALID_PARA);
						return (-1);
					}
				}
				else {
					if(p != 1) {
						SetError(INVALID_PARA);
						return (-1);
					}
				}
				adr = aton(argv[optind-1]);
				siz = aton(argv[optind]);
				if((adr < 0) || ((adr + siz) >= BANKSIZ) || (siz > BLOCKSIZ)) {
					//#define	BLOCKSIZ		0x4000		/* 16384(16K)Bytes (PAGESIZ x PAGES_PER_BLOCK)	*/
					printf("parameter check(max size : %#x, max adr : %#lx, adr : %#x, size : %#x)\n",BLOCKSIZ ,BANKSIZ, adr, siz);
					SetError(INVALID_PARA);
					return	-1;
				}

				if(opt == 'w') {
					// flash write
					p = aton(argv[optind + 1]);
					memset(BlockBuf, p, siz);
					printf("Flash write start\n");
					sc = ticktime();
					stat = flash_write(adr, BlockBuf, siz);
					ec = ticktime();
					printf("Flash write end[err = %d, time = %d ms]\n", stat, difftick(sc, ec));
					if(stat) {
						SetError(FAILED_CMD);
						return(-1);
					}
				}
				else if(opt == 'e') {
					printf("Flash erase start\n");
					sc = ticktime();
					stat = SpiFlash_Erase(adr, siz);
					ec = ticktime();
					printf("Flash erase end[err = %d, time = %d ms]\n", stat, difftick(sc, ec));
					if(stat) {
						SetError(FAILED_CMD);
						return(-1);
					}
				}
				// flash read
				printf("flash buf clear\n");
				memclr(BlockBuf, siz);
				printf("Flash read start\n");
				sc = ticktime();
				printf("flash read start\n");
				stat = flash_read(adr, BlockBuf, siz);
				ec = ticktime();
				printf("Flash read end[err = %d, time = %d ms]\n", stat, difftick(sc, ec));
				if(stat) {
					SetError(FAILED_CMD);
					return(-1);
				}

				//if(sfT == 0 || sfT == 2)
				//	break;

				printf("Flash memory dump\n");
				hexdump(adr, (byte *)BlockBuf, siz);
				break;

			default :
				SetError(INVALID_PARA);
				return (-1);
		}
	}

	if(stat) {
		SetError(FAILED_CMD);
		return(-1);
	}
	return	0;
}
#endif

/*This is the function Definition for weather the 32 bit Flash Memory is busy (or) free*/
void flash_busy(void)
{
	byte temp;

	SF_EN();
	putch_SF(SF_RDSR);
	do
	{
		temp = putch_SF(0xFF);
	}while(temp & 0x01);
	SF_DE();
}

/*
 * Serial Falsh Program and Erase Characteristics
 * Page Program Time 256byte : Typ(1.0 ms), Max(3.0 ms), Ref(Nand Flash  200~500 us)
 * Byte Program Time 	     : Typ(7 us)
 * Block Erase Time 4-Kbyte  : Typ(50 ms) , Max(200 ms)
 * Block Erase Time 32-Kbyte : Typ(250 ms), Max(600 ms), Ref(Nand Flash 2~3 ms)
 * Block Erase Time 64-Kbyte : Typ(400 ms), Max(950 ms)
 * Chip Erase Time 	     : Typ(25 sec), Max(40 sec)
 * Write Status Register Time: Typ(200 ns)
 */
int flash_ready(int ms)
{
	clock_t mst;
	byte    temp;

	SF_EN();
	putch_SF(SF_RDSR);
	mstimeout(&mst, 0);
	do {
		temp = putch_SF(0xFF);
		if(!(temp & 0x01))
			break;
	}while(!mstimeout(&mst, ms));
	SF_DE();

	if(sfT) printf("flash ready %#x\n", temp);
	return(temp);
}

/*This is function Definition for Enable the write operatations on the Flash Memory*/
void write_enable(void)
{
	//SF_EN();
	//putch_SF(SF_WREN);    // enable write operations
	//SF_DE();
	SF_EN();
    putch_SF(AT25DF_WRITE_ENABLE);
    SF_DE();
}

/*This is the function Definition for Disableing the write operations on the Flash Memory*/
void write_disable(void)
{
	//SF_EN();
	//putch_SF(SF_WRDI);  //disable write operations
	//SF_DE();
	SF_EN();
    putch_SF(AT25DF_WRITE_DISABLE);
    SF_DE();
}

void global_unprotect_sector(void)
{
	SF_EN();
	putch_SF(SF_WRSR1); // Write Status Register instruction
	putch_SF(0x00);    // make sure Block Protect bits are disabled
	SF_DE();
//    BYTE data = 0x00;
//    SF_CommandAndWriteN(AT25DF_WRITE_STATUS_BYTE1, &data, 1);
}

void flash_pwr_down(void)
{
	SF_EN();
	putch_SF(SF_DPD); // Deep Power-Down
	SF_DE();
}

void flash_pwr_resume(void)
{
	SF_EN();
	putch_SF(SF_RDPD); // Resume from Deep Power-Down
	SF_DE();
}


/*this is function Definition for sending 3 bytes of Address to Flash Memory*/
void send_add_flash(dword add)
{
	byte i = 0, n = 16, op;

	do {
		op = (byte)(add >> n);
		putch_SF(op);
		n = n - 8;
		i++;
	}while(i < 3);
}

int FlashInit(void)
{
	int temp;

	write_enable();
	//SpiFlash_WriteEnable();
	global_unprotect_sector();
	//SpiFlash_GlobalUnprotectSector();
	flash_ready(6);
	//SpiFlash_Ready(6);
	temp = SpiFlash_ReadInfo();
	if(temp == 0) {
		printf("\n\nflash is not identified![%#x]\n", FlashId);
		return (-2);
	}
	//flash_stat();
	SpiFlash_ReadStatus();
	return 0;
}

int flash_is(void)
{
	dword temp;

	FlashId = 0;

	SF_EN();
	temp = putch_SF(SF_RDID);
	temp = putch_SF(0xff);
	FlashId = (temp << 16); 	// Manufacturer ID(0x1f)
	temp = putch_SF(0xff);
	FlashId |= (temp << 8); 	// Device ID1(0x48)
	temp = putch_SF(0xff);
	FlashId |= temp; 			// Device ID2(0x00)
	//temp = putch_SF(0xff);
	//FlashId |= temp; 			// Device Info(0x00)
	SF_DE();

	if(sfT) printf("flash id %#lx\n", FlashId);
	return(FlashId == ID_AT25DF641);

	//return(FlashId == ID_AT25DF321);       //pms_temp
}

/*
   bit 7 : SPRL(Sector Protection Registers Locked)
   bit 6 : RES(Reserved for future use)
   bit 5 : EPE(Erase/Program Error)
   bit 4 : WPP(Write Protect(#WP) Pin Status)
   bit 3~2 : SWP(Software Protection Status)
   bit 1 : WEL(Write Enable Latch Status)
   bit 0 : RDY/BSY(Ready/Busy Status)
 */
int flash_stat(void)
{
	int stat;

	SF_EN();
	putch_SF(SF_RDSR); // instruction
	stat = putch_SF(0xff);  // read status
	stat = putch_SF(0xff);  // read status
	SF_DE();

	if(sfT) printf("flash stat %#x\n", stat);
	return(stat);
}

siz_t flash_size(int force)
{
	static 	siz_t  flsize = 0;
	siz_t 	siz;

	if (flsize == 0 || force) {
		siz = 0L;
		if(SpiFlash_ReadInfo())
			siz = BANKSIZ;

		flsize = siz;
	}

	if(sfT == 1) printf("flash size %#lx\n", flsize);
	return(flsize);
}

/*
 *	Allow random access
 */
int flash_read(siz_t addr, byte *buf, siz_t size)
{
	siz_t	ad, siz;
	int	page, i, n;
	byte 	oadr;
	byte 	*pb = buf;

	if (addr < 0 || (addr+size) > flash_size(0)) { /* check range validity */
		//syserr("flash_read(a:%#x, s:%#x, m:%#x) - invalid range\n", addr, size, flash_size(0));
		printf("flash_read(a:%#lx, s:%#lx, m:%#lx) - invalid range\n", addr, size, flash_size(0));
		return(E_ADDR);
	}

	ad = addr;	// debug backup
	siz = size;	// debug backup
	while (size > 0) {
		page = addr / PAGESIZ;			/* page addr in the bank  */
		oadr = addr % PAGESIZ;			/* offset addr in the bank */
		SF_EN();
		putch_SF(SF_READ_LOW);			// Max Frq 50MHz
		//putch_SF(SF_READ);			// Max Frq 85MHz
		putch_SF((byte)(page >> 8));		// address 23-16
		putch_SF((byte)(page));			// address 15-8
		putch_SF(oadr);				// address 7-0
		//putch_SF(0xff);			// SF_READ dumy data
		n = PAGESIZ - oadr;
		n = min(n, size);
		for(i = 0 ; i < n ; i++, pb++)
			*pb = putch_SF(0xff);
		SF_DE();

		if(sfT > 1) printf("flash_read a23-16[%#x] a15-8[%#x] a7-0[%#x] size[%#x]\n",
				(byte)(page >> 8), (byte)(page), oadr, n);
		addr += n;
		size -= n;
	}
	if(sfT > 1) printf("flash_read(addr %#lx, size %#lx)\n", ad, siz);
	if(sfT > 2) hexdump(ad, (byte *)buf, siz);
	return(0);
}

/*
 *	Erase the corresponding blocks.
 */

int flash_erase(siz_t addr, siz_t size)
{
	siz_t	ad, siz;
	int	page, i, err = 0;

	if(addr == 0 && size == 0) {		// full domain erase
		size = flash_size(0);
		write_enable();
		SF_EN();
		putch_SF(SF_CHIP_ERASE);
		SF_DE();
		printf("Waiting... Chip erase time is 36 ~ 56 sec\n");
		flash_ready(57 * CLOCKS_PER_SEC); // Chip Erase Time : Typ(36 sec), Max(56 sec)
		ad = addr;
		siz = size;
		goto RESULT;
	}

	if(addr < 0 || (addr+size) > flash_size(0)) { /* check range validity */
		//syserr("flash_erase(a:%#x, s:%#x, m:%#x) - invalid range\n", addr, size, flash_size(0));
		printf("flash_erase(a:%#lx, s:%#lx, m:%#lx) - invalid range\n", addr, size, flash_size(0));
		return(E_ADDR);
	}

	/*
	 * adjust addr to the start of block.
	 */
	i = addr % BLOCKSIZ;
	addr -= i;
	size += i;

	ad = addr;
	siz = size;
	while (size > 0) {
		page = addr / PAGESIZ; 		/* page addr in device(0~) */
		write_enable();
		SF_EN();
		putch_SF(SF_BLK_ERASE_4K);	// Block Erase (4KBytes)
		putch_SF((byte)(page >> 8));	// address 23-16
		putch_SF((byte)(page));		// address 15-8
		putch_SF(0x00);			// address 7-0
		SF_DE();
		flash_ready(210);		// Block Erase Time 4-Kbyte : Typ(50 ms) , Max(200 ms)
		addr += BLOCKSIZ;
		size -= BLOCKSIZ;
	}
RESULT :
	if(sfT > 1) printf("flash_erase(addr %#lx, size %#lx) - errcode %#x\n", ad, siz, err);
	if(err)
		err = E_FAIL;
	return(err);
}


/*
 *	Assume sequential write.
 */
int flash_write(siz_t addr, byte *buf, siz_t size)
{
	siz_t	ad, siz;
	int	page, i, n, err, nerr;
	byte 	oadr;
	byte 	*pb = buf;

	if (addr < 0 || (addr+size) > flash_size(0)) {
		//syserr("flash_write(a:%#x, s:%#x, m:%#x) - invalid range\n", addr, size, flash_size(0));
		printf("flash_write(a:%#lx, s:%#lx, m:%#lx) - invalid range\n", addr, size, flash_size(0));
		return(E_ADDR);
	}

	ad = addr;
	siz = size;
	nerr = err = 0;

	while(size > 0 ) {
		if((addr%BLOCKSIZ) == 0) {
			err = flash_erase(addr, (siz_t)1);
			if(err)
				return(err|0x8);
		}
		page = addr / PAGESIZ;			/* page addr in the bank 	*/
		oadr = addr % PAGESIZ;			/* offset addr in the bank */
		write_enable();
		SF_EN();
		putch_SF(SF_PROGRAM);  		//write command
		putch_SF((byte)(page >> 8));	// address 23-16
		putch_SF((byte)(page));		// address 15-8
		putch_SF(oadr);			// address 7-0
		n = PAGESIZ - oadr;
		n = min(n, size);
		for(i = 0 ; i < n ; i++, pb++ )
			putch_SF(*pb);
		SF_DE();
		flash_ready(7);			// Page Program Time 256byte : Typ(1.5 ms), Max(5.0 ms)
		flash_read(addr, verifBuf, (siz_t)n);
		if(memcmp(verifBuf, pb-n, n) == 0)
			nerr=0;
		else {
			nerr++;
			if(sfT > 1) printf("write retry count[%d]\n", nerr);
			if (nerr < 3) {
				msleep(2);
				pb -= n;
				continue;
			}
			err = E_VERF;
		}

		if(sfT > 1) printf("flash_write a23-16[%#x] a15-8[%#x] a7-0[%#x] size[%#x]\n",
				(byte)(page >> 8), (byte)(page), oadr, n);

		if(err) {
			printf("flash write error\n");
			break;
		}

		addr += n;
		size -= n;
	}
	if(sfT > 1) printf("flash_write(addr %#lx, size %#lx) - errcode %#x\n", ad, siz, err);
	if(sfT > 2) hexdump(ad, (byte *)buf, siz);
	return(err);
}


BYTE SF_RW_Byte_FLASH(BYTE data)
{
   BYTE read, test = 0;
   HAL_SPI_TransmitReceive(SF_HANDLE, &data, &read, 1, 100);
   if(read != 0)
	   test = 1;
   return read;



}
// EOF
