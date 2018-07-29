#include<stdio.h>
#include<sys/resource.h>
#include<string.h>
#include<sys/resource.h>
#include<sys/time.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdint.h>

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t LONG;
typedef uint8_t  BYTE;

// struct definition from Microsoft Windows,
// Keep same name with origin copy.
typedef struct BITMAPFILEHEADER {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER;

// struct definition from Microsoft Windows,
// Keep same name with origin copy.
typedef struct BITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER;

int main(int argc, char *argv[])
{

    BITMAPFILEHEADER bmp_header;
    BITMAPINFOHEADER bmp_info;
    
    memset((char *)&bmp_header, 0, sizeof(bmp_header));
    memset((char *)&bmp_info, 0, sizeof(bmp_info));
    if(argc != 3)
	{
        printf("usage %s input_file(.bmp) output_file(.dat)\n", argv[0]);
		return -1;
	}
    int input_fd = open(argv[1], O_RDWR | O_CREAT, 0777);
    if(0 > input_fd)
    {
        printf("fail to open %s\n", argv[1]);
        return -1;
    }
    read(input_fd, &bmp_header, sizeof(bmp_header));
    read(input_fd, &bmp_info, sizeof(bmp_info));
    
    printf("Bitmap header:");
    printf("\n  type: 0x%x", bmp_header.bfType);
    printf("\n  size: %d\n", bmp_header.bfSize);
    printf("\n  reserved1: 0x%x", bmp_header.bfReserved1);
    printf("\n  reserved2: 0x%x", bmp_header.bfReserved2);
    printf("\n  data offset: %d", bmp_header.bfOffBits);
    printf("\nBitmap info");
    printf("\n  struct size: %d", bmp_info.biSize);
    printf("\n  width: %d", bmp_info.biWidth);
    printf("\n  height: %d", bmp_info.biHeight);
    printf("\n  planes: %d", bmp_info.biPlanes);
    printf("\n  bits of color: %d", bmp_info.biBitCount);
    printf("\n  compression type: %d", bmp_info.biCompression);
    printf("\n  data sizes: %d", bmp_info.biSizeImage);
    printf("\n  X pixels per meter: %d", bmp_info.biXPelsPerMeter);
    printf("\n  Y pixels per meter: %d", bmp_info.biYPelsPerMeter);
    printf("\n  number of colors in file: %d", bmp_info.biClrUsed);
    printf("\n  number of import colors: %d\n", bmp_info.biClrImportant);
    
    close(input_fd);
    input_fd = open(argv[1], O_RDWR | O_CREAT, 0777);
    if(0 > input_fd)
    {
        printf("fail to open %s", argv[1]);
        return -1;
    }
    int output_fd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0777);
    if(0 > output_fd)
    {
        printf("%s:%d fail to open %s\n", __func__, __LINE__, argv[2]);
        return -1;
    }
    
    char buf[7] = {0};
    // if the first bit of the 8b data we read from bmp file is 1, it will be negative number after we cast this 8b to char/int/unsighed_int.
    // so we need to use uint8_t to save it
    uint8_t c = 0;
    int i = 0;
    int bmp_file_size = bmp_header.bfSize;
    int width = bmp_info.biWidth;
    int height = bmp_info.biHeight;
    bmp_file_size = 54 + (3 * width * height);
    for(i = 0; i < bmp_file_size; i++)
    {
        if(0 > read(input_fd, &c, sizeof(c)))
        {
            printf("fail to open %s", argv[1]);
            return -1;
        }
        if(0 == i)
        {
            snprintf(buf, sizeof(buf), "0x%02x", c);
        }
        else
        {
            snprintf(buf, sizeof(buf), ", 0x%02x", c);
        }
        unsigned int num = c;
        if(i < 6)
        {
            //printf("0x%02x %d %c buf:%s   0x%02x %d %c \n", (uint8_t)c, c, c, buf, num, num, num);
            printf("0x%02x buf:%s\n", (uint8_t)c, buf);
        }
        write(output_fd, buf, strlen(buf));
    }
    printf("read size:bmp_file_size %d bmp_header.bfSize %d i = %d output.data size: %d\n",
           i, bmp_file_size, bmp_header.bfSize, 6 * bmp_file_size - 2);
    
/*  
    snprintf(temp_buf, sizeof(temp_buf), "%s", buf);
    snprintf(temp_buf, sizeof(temp_buf), "%s\n", temp_buf);
    
    write(output_fd, temp_buf, strlen(temp_buf));
    printf("%s:%d temp_buf  %s strlen(temp_buf) %d\n", __func__, __LINE__, temp_buf, (int)strlen(temp_buf));
    
    printf("runing in %s %d %04d%02d%02d\n", __func__, __LINE__, 1, 2, 3);
*/  
    close(output_fd);
    close(input_fd);
    
    return 0;
}
