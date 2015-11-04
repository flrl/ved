#include <conio.h>
#include <dos.h>

void set_mode(unsigned char mode) {
	union REGS regs;
	regs.h.ah = 0x00;
	regs.h.al = mode;
	int86(0x10, &regs, &regs);
}

void set_cursor(unsigned char x, unsigned char y) {
	union REGS regs;
	regs.h.ah = 0x02;
	regs.h.bh = 0x00;
	regs.h.dh = y;
	regs.h.dl = x;
	int86(0x10, &regs, &regs);
}

void write_char(unsigned char c, unsigned char fg, unsigned char bg,
				int x, int y)
{
	unsigned short attrib = (bg << 4) | (fg & 0x0f);
	unsigned short *where;
	where = (unsigned short *) 0xB8000000L + (y * 80 + x);
	*where = c | (attrib << 8);
}

int main() {
	unsigned char c;
	int x, y=0;

	set_mode(0x03);

	for (x = 0; x < 26; x++) {
		write_char(x + 'a', 7, 1, x, y);
	}

	set_cursor(8,5);


	while(getch() != 'q')
		;

	return 0;
}