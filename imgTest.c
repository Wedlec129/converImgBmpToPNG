#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <zlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <libpng/include/libpng16/png.h>
#include <libpng/include/png.h>
#include <libpng/include/pngconf.h>

// gcc /Users/wedlec/Desktop/imgTest.c -o /Users/wedlec/Desktop/imgTest-app  -lpng
// ./imgTest-app /Users/wedlec/Desktop/dog.bmp /Users/wedlec/Desktop/dog.png

 
//структура цвета
typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
pixel_t;

 
 //структура фото с цветами   
typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;
    
 
//ф-я
static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}
    
//сохраняем png из "bitmap"
static int save_png_to_file (bitmap_t *bitmap, const char *path)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
   /* "status" содержит возвращаемое значение этой функции. Во-первых
        ему присвоено значение, которое означает «сбой». Когда рутина
        завершил свою работу, ему присвоено значение, которое означает
        'успех'. */
    int status = -1;
    // это обязательно делать так (документация libpng)
    
    int pixel_size = 3;
    int depth = 8;
    
    fp = fopen (path, "wb");
     

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
     
    
    info_ptr = png_create_info_struct (png_ptr);
     
     
    
    /* Установить атрибуты изображения.  типо heder*/
    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    

    /* Инициализировать строки PNG. */
    row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row =png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            pixel_t * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    
    /* Записываем данные изображения через "fp". */
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* Программа успешно записала файл, поэтому мы устанавливаем
        "status" значение, указывающее на успех. */

    status = 0;
    
    for (y = 0; y < bitmap->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}





void convertBmpToPng(char* filename,char *fileOut)
{
     
    FILE* f = fopen(filename, "rb");

    if(f == NULL)
        perror( "Argument Exception");

    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    //cout << endl;
    //cout << "  Name: " << filename << endl;
    //cout << " Width: " << width << endl;
    //cout << "Height: " << height << endl;

 

    int row_padded = (width*3 + 3) & (~3);
    //unsigned char* data = new unsigned char[row_padded];
    unsigned char* data=(unsigned char*)malloc (row_padded * sizeof (unsigned char));
    unsigned char tmp;




    bitmap_t fruit;
    int status;

    status = 0;

    /* Create an image. */

    fruit.width = width;
    fruit.height = height;

    fruit.pixels = calloc (fruit.width * fruit.height, sizeof (pixel_t));

    


    for (int y = 0; y < fruit.height; y++) {
        for (int x = 0; x < fruit.width; x++) {
            pixel_t * pixel = pixel_at (& fruit, x, y);
            pixel->red = 0;
            pixel->green = 0;
            pixel->blue=0;




        }
    }

 

    




    for(int i = 0; i < height; i++)
    {
        int k=0;
        fread(data, sizeof(unsigned char), row_padded, f);
        for(int j = 0; j < width*3; j += 3)
        {
            // Convert (B, G, R) to (R, G, B)
            tmp = data[j];
            data[j] = data[j+2];
            data[j+2] = tmp;




   
            pixel_t * pixel = pixel_at (& fruit, j-k, height-i-1);
            pixel->red = data[j];
            pixel->green =data[j+1];
            pixel->blue=data[j+2];
            k+=2;

  



           // printf("R: %d G: %d R: %d \n",data[j],data[j+1],data[j+2]);
        
        }
       
    }

     
    /* сохраняе картинку в png'. */
 
    if (save_png_to_file (& fruit, fileOut)) {
	fprintf (stderr, "Error writing file.\n");
	 
    }



    free (fruit.pixels);
     fclose(f);
     
}


int main (int argc,  char* argv[])
{

    if(argc!=3){
    perror("Error NO FIND arguments ");
    return 228;
    }

 
    //convertBmpToPng("/Users/wedlec/Desktop/dog.bmp","/Users/wedlec/Desktop/outFile.png");
    
    convertBmpToPng(argv[1],argv[2]);
 
 
  

    
    return 0;
}
