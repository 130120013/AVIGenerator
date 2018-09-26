#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

using namespace std;


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

/*RIFF ('AVI '
	   LIST ('hdrl'
				   'avih'(<Main AVI Header>)
				   LIST ('strl'
				   'strh'(<Stream header>)
				   'strf'(<Stream format>)
				   'strd'(additional header data)
					   .
					   .
					   .
			 )
						  .
				   .
			 .
	   )

LIST ('movi'
				   {SubChunk | LIST('rec '
							  SubChunk1
							  SubChunk2
								.
							   .
								 .
						 )

				 .
				 .
				 .
			 }

						.
						.
						.
   )

	   ['idx1'<AVIIndex>]
)*/





typedef struct {                     //The Main AVI Header
	DWORD  dwMicroSecPerFrame;
	DWORD  dwMaxBytesPerSec;
	DWORD  dwReserved1;
	DWORD  dwFlags;
	DWORD  dwTotalFrames;
	DWORD  dwInitialFrames;
	DWORD  dwStreams;
	DWORD  dwSuggestedBufferSize;
	DWORD  dwWidth;
	DWORD  dwHeight;
	DWORD  dwScale;
	DWORD  dwRate;
	DWORD  dwStart;
	DWORD  dwLength;
} MainAVIHeader;


typedef struct {                          //AVI Stream Header
	char    fccType[4];
	char    fccHandler[4];
	DWORD   dwFlags;
	DWORD   dwReserved1;
	DWORD   dwInitialFrames;
	DWORD   dwScale;
	DWORD   dwRate;
	DWORD   dwStart;
	DWORD   dwLength;
	DWORD   dwSuggestedBufferSize;
	DWORD   dwQuality;
	DWORD   dwSampleSize;
} AVIStreamHeader;


/*WAVE  Bytes   '##wb'
				  BYTE    abBytes[];*/

				  /*DIB  Bits   '##db'
							   BYTE   abBits[];*/

typedef struct {
	BYTE          bFirstEntry;
	BYTE          bNumEntries;
	WORD          wFlags;
	BYTE                  *peNew;
} AVIPALCHANGE;

typedef struct {
	DWORD  ckid;
	DWORD  dwFlags;
	DWORD  dwChunkOffset;
	DWORD  dwChunkLength;
} AVIINDEXENTRY;

typedef struct BMPFILEHEADER {
	char type[2];
	DWORD size;
	DWORD reserved;
	DWORD offset;
}FileHeader;


typedef struct BMPINFOHEADER {
	DWORD headersize;
	DWORD width;
	DWORD height;
	WORD planes;
	WORD bitsperpixel;
	DWORD compression;
	DWORD sizeimage;
	DWORD xpixelspermeter;
	DWORD ypixelspermeter;
	DWORD colorsused;
	DWORD colorsimportant;
} BmpHeader;



typedef struct tagBITMAP {
	DWORD width;
	DWORD height;
	BYTE *data;
}BITMAP;




int main(void)
{
	ifstream infile;
	ofstream outfile;
	infile.open("filedisk.avi", ios::binary | ios::in);
	outfile.open("testnew.bmp", ios::binary | ios::out);
	if (!infile)
		cout << "Error opening file";
	//      DWORD x;

	FileHeader bmpf;
	bmpf.type[0] = 'B';
	bmpf.type[1] = 'M';
	bmpf.reserved = 0;


	// DWORD size;

	// DWORD offset;

	BmpHeader bmph;
	BITMAP bitmap;
	char palette[768];
	char ch, che[4];
	int i = 0, count;
	int imageno = 1;
	int skip = 0;                    //This is no. of fields delimited by 'db' and/or 'dc' to skip
	int x = 0, y = 261;
	MainAVIHeader avih;
	while (!infile.eof())
	{
		infile >> ch;
		if (ch == 'a')
		{
			infile >> che[0];
			infile >> che[1];
			infile >> che[2];
			che[3] = '\0';
			if (!strcmp(che, "vih"))
			{
				infile.ignore(4);
				infile.read((char *)&avih, 14 * 4);
				break;
			}
		}
	}
	while (!infile.eof())
	{
		infile >> ch;
		if (ch == 'v')
		{
			infile >> che[0];
			infile >> che[1];
			infile >> che[2];
			che[3] = '\0';
			if (!strcmp(che, "ids"))
			{
				while (1)
				{
					infile >> ch;
					if (ch == 's')
					{
						infile >> che[0];
						infile >> che[1];
						infile >> che[2];
						che[3] = '\0';
						if (!strcmp(che, "trf"))
						{
							infile.ignore(4);
							infile.read((char *)&bmph, 40);
							//                        cout << bmph.bitsperpixel;
							if (bmph.bitsperpixel != 24)
							{
								/***read palette information***/
								for (i = 0; i < 256; i++)
								{
									palette[i * 3 + 2] = infile.get() >> 2;
									palette[i * 3 + 1] = infile.get() >> 2;
									palette[i * 3 + 0] = infile.get() >> 2;
									infile.get();
								}
							}
							bitmap.width = bmph.width;
							bitmap.height = bmph.height;
							bitmap.data = new BYTE[bitmap.width*bitmap.height];
							count = bitmap.width*bitmap.height;
							while (1)
							{
								infile >> ch;
								if (ch == '0')
								{
									infile >> che[0];
									infile >> che[1];
									infile >> che[2];
									che[3] = '\0';
									if (!strcmp(che, "0db") || !strcmp(che, "0dc"))
									{
										imageno--;
										if (imageno == 0)
										{
											infile >> bitmap.data[i++];
											while (1)
											{
												infile >> ch;
												if (ch == '0')
												{
													infile >> che[0];
													infile >> che[1];
													infile >> che[2];
													che[3] = '\0';
													if (!strcmp(che, "0db") || !strcmp(che, "0dc"))
													{
														skip--;
														if (skip == -1)
														{
															count = i;
															bmpf.size = i;
															break;
														}
													}
													else
													{
														bitmap.data[i++] = ch;
														bitmap.data[i++] = che[0];
														bitmap.data[i++] = che[1];
														bitmap.data[i++] = che[2];
														if (i > bmph.sizeimage)
															break;
													}
												}
												else
												{
													bitmap.data[i++] = ch;
													if (i > bmph.sizeimage)
														break;
												}
											}
											break;
										}
									}
								}
							}
							break;
						}
					}
				}
			}
		}
		//            infile.read((char *)&x,4);
		//            outfile.write((char *)&x,4);
	}
	bmpf.offset = 54;
	bmpf.size += 54;
	if (bmph.bitsperpixel != 24)
	{
		bmpf.offset += 1024;
		bmpf.size += 1024;
	}
	outfile.write((char *)&bmpf, 14);
	outfile.write((char *)&bmph, 40);

	for (i = 0; i < 256; i++)
	{
		outfile.put(palette[i * 3 + 2] << 2);
		outfile.put(palette[i * 3 + 1] << 2);
		outfile.put(palette[i * 3 + 0] << 2);
		outfile.put(0);
	}



	outfile.write(bitmap.data, count);
	delete bitmap.data;

	outfile.close();
	infile.close();
	getch();
	closegraph();
	return 0;
}