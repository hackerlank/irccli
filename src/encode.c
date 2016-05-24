#include "encode.h"

static char *encoding = "";
static char *encodings = "\
Available encodings:\n\
\n\
European languages\n\
       ASCII,  ISO-8859-{1,2,3,4,5,7,9,10,13,14,15,16},  KOI8-R, KOI8-U, KOI8-RU,\n\
       CP{1250,1251,1252,1253,1254,1257}, CP{850,866,1131}, Mac{Roman,CentralEurope,\n\
       Iceland,Croatian,Romania}, Mac{Cyrillic,Ukraine,Greek,Turkish}, Macintosh\n\
\n\
Semitic languages\n\
       ISO-8859-{6,8}, CP{1255,1256}, CP862, Mac{Hebrew,Arabic}\n\
\n\
Japanese\n\
       EUC-JP, SHIFT_JIS, CP932, ISO-2022-JP, ISO-2022-JP-2, ISO-2022-JP-1\n\
\n\
Chinese\n\
       EUC-CN, HZ, GBK, CP936, GB18030, EUC-TW, BIG5, CP950, BIG5-HKSCS,\n\
       BIG5-HKSCS:2004, BIG5-HKSCS:2001, BIG5-HKSCS:1999, ISO-2022-CN, ISO-2022-CN-EXT\n\
\n\
Korean\n\
       EUC-KR, CP949, ISO-2022-KR, JOHAB\n\
\n\
Armenian\n\
       ARMSCII-8\n\
\n\
Georgian\n\
       Georgian-Academy, Georgian-PS\n\
\n\
Tajik\n\
       KOI8-T\n\
\n\
Kazakh\n\
       PT154, RK1048\n\
\n\
Thai\n\
       TIS-620, CP874, MacThai\n\
\n\
Laotian\n\
       MuleLao-1, CP1133\n\
\n\
Vietnamese\n\
       VISCII, TCVN, CP1258\n\
\n\
Platform specifics\n\
       HP-ROMAN8, NEXTSTEP\n\
";

// Get encodings variable
char *genc() {
	return encodings;
}

void enc(char *_encoding) {
	encoding = _encoding;
}

void encode(char **str, int size) {
	if (*encoding) {
		////////  Convert from unicode to encoding  ////////
		iconv_t cd = iconv_open(encoding, "UTF-8");
		if (cd == (iconv_t)(-1))
			error("Error with iconv_open");

		char strcpy[size];
		char buffer[size];
		strncpy(strcpy, *str, size);
		char *inptr  = (char *) &strcpy[0];
		char *outptr = (char *) &buffer[0];
		size_t insize  = size;
		size_t outsize = size;

		iconv(cd, &inptr, &insize, &outptr, &outsize);

		memset(*str, 0, size);
		strncpy(*str, buffer, size);

		iconv_close(cd);
	}
}

void unencode(char **str, int size) {
	if (*encoding) {
		////////  Convert from encoding to unicode  ////////
		iconv_t cd = iconv_open("UTF-8", encoding);
		if (cd == (iconv_t)(-1))
			error("Error with iconv_open");

		char strcpy[size];
		char buffer[size];
		strncpy(strcpy, *str, size);
		char *inptr  = (char *) &strcpy[0];
		char *outptr = (char *) &buffer[0];
		size_t insize  = size;
		size_t outsize = size;

		iconv(cd, &inptr, &insize, &outptr, &outsize);

		*str = buffer; // Not sure how or why this works (isn't buffer a local variable?)

		iconv_close(cd);
	}
}
