/* Header file that provides a few functions relating to file I/O:
 * (find file; read BMP file; read height field file)
 * See function prototypes at end for details...
 */

/* Ensure header is read once only... */
#ifndef _FILEOPERATIONS_
#define _FILEOPERATIONS_

#ifdef WIN32
/* Include necessary headers, available under Windows...  */
#include <windows.h>
#include <wingdi.h>
#endif

#define MAXWIDTH 257
#define MAXDEPTH 257

// Ensure function names are handled the same way whether included
// by C or C++ source code.  This prevents link errors when
// you mix both types of code...
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus  */

#ifndef WIN32
/* Define data structures for data in .BMP files.
 * These are defined in <wingdi.h> under Windows,
 * but need to be built from scratch on other platforms.
 *
 * Note that most Windows compilers will pack the following structures, so
 * when reading them under MacOS or UNIX we need to read individual fields
 * to avoid differences in alignment...
 */

typedef struct                       /**** BMP file header structure ****/
    {
    unsigned short bfType;           /* Magic number for file */
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
    } BITMAPFILEHEADER;

            /* "MB" */

typedef struct                       /**** BMP file info structure ****/
    {
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
    } BITMAPINFOHEADER;

/*
 * Constants for the biCompression field...
 */

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */

typedef struct                       /**** Colormap entry structure ****/
    {
    unsigned char  rgbBlue;          /* Blue value */
    unsigned char  rgbGreen;         /* Green value */
    unsigned char  rgbRed;           /* Red value */
    unsigned char  rgbReserved;      /* Reserved */
    } RGBQUAD;

typedef struct                       /**** Bitmap information structure ****/
    {
    BITMAPINFOHEADER bmiHeader;      /* Image header */
    RGBQUAD          bmiColors[256]; /* Image colormap */
    } BITMAPINFO;

#  endif       /* !WIN32 */

#define BF_TYPE 0x4D42 

/* Define struct for containing .BMP header & pixel data... */
typedef struct {
  BITMAPINFO *BitmapInfo; /* Bitmap information */
  char    *BitmapBits;    /* Bitmap data */
} UCO_BMP;


/* Function prototypes... */

// findFile:   Find a file, either in current directory, or in parent.
//     If we ask to find a file like "foo.bmp" the return value
//       will be "./foo.bmp"  or "../foo.bmp".
//       (or "" if file wasn't found).
char  *findFile        (char *fname);

// readHeightField:
//   Read a file containing a height field, returning
//     width / depth of the field through the parameter list,
//     along with the array of heights.
//     The caller should declare
//          int heights[MAXDEPTH][MAXDEPTH] 
//  Return TRUE if file was read, FALSE otherwise.
int readHeightField  (const char *filename, int *width, int *depth, 
                        int heights[][MAXDEPTH]);

// getBMPTexture: Read a 24-bit Windows BMP file and attach the
//  pixel data to a GL texture handle. Optionally build mipmaps.
//  Returns the texture handle as int, or -1 if failed.
int getBMPTexture(char *fname, GLboolean buildMipmaps);

// LoadBMP: Read a 24-bit Windows bitmap,
//     returning the BMP data via a struct in the parameter list.
//  Return value is TRUE if read was successful, FALSE otherwise.
int   LoadBMP          (char *fname, UCO_BMP *b, int transparency);

// LoadDIBitmap: Read a 24-bit Windows bitmap,
//     returning the BMP header stuff via the parameter list
//     and a pointer to the pixel data as return value.
//     (or  NULL if read failed).
char  *LoadDIBitmap    (const char *filename, BITMAPINFO **info);

// SaveDIBitmap: Write a 24-bit Windows bitmap,
//     char *filename   ... (in) Name of BMP file to written e.g. "foo.bmp"
//     BITMAPINFO *info ... (in) Struct containing BMP header.
//     char *bits       ... (in) Pointer to pixel data.
//  Return value is 0 if write was successful, -1 otherwise.
int   SaveDIBitmap     (const char *filename, BITMAPINFO *info, char *bits);

#ifdef __cplusplus
}       /* extern C */
#endif  /* __cplusplus */

#endif   /* _FILEOPERATIONS_ */

