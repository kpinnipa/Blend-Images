#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//unsigned char get_red(unsigned char *imagedata,float x,float y,int imagewidth, int imageheight);

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
struct tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved;must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits}
};

struct tagBITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by th ebitmap
    DWORD biClrImportant;  //number of colors that are important
};

unsigned char get_color(unsigned char *data, float x, float y, int imagewidth, int color_offset)
{
    int edit;
    int bplo = imagewidth*3;
    int padding = 0;
    if (bplo%4 != 0)
    {
        padding = (4-bplo%4);
    }
    edit = x*3 + (y*imagewidth*3) + padding*y + color_offset;
    //data[x*3 + y*fih.biWidth*3 + 2] = 0;
    //data[x*3 + bplo + 2] = 0;
    return data[edit]; //do this to change all of the pixels of a certain color
    //return edit;
}

int set_color(float x, float y, int imagewidth, int color_offset)
{
    int edit;
    int bplo = imagewidth*3;
    int padding = 0;
    if (bplo%4 != 0)
    {
        padding = (4-bplo%4);
    }
    edit = x*3 + (y*imagewidth*3) + padding*y + color_offset;
    return edit; //do this to change all of the pixels of a certain color

}


unsigned char get_color_bilinear(unsigned char *imagedata, float x, float y, int imagewidth, int imageheight,int color_offset)
{
    float x1 = floorf(x);
    float x2 = ceilf(x);
    if(x2>=imagewidth)
    {
        x2 = x1;
    }
    float y1 = floorf(y);
    float y2 = ceilf(y);
    if(y2>=imageheight)
    {
        y2 = y1;
    }
    float dx = x - x1;
    float dy = y - y1;
    unsigned char left_upper = get_color(imagedata,x1,y2,imagewidth,color_offset);
    unsigned char right_upper = get_color(imagedata,x2,y2,imagewidth,color_offset);
    unsigned char left_lower= get_color(imagedata,x1,y1,imagewidth,color_offset);
    unsigned char right_lower= get_color(imagedata,x2,y1,imagewidth,color_offset);
    unsigned char left = left_upper* (1-dy)+ left_lower * dy;
    unsigned char right= right_upper* (1-dy)+right_lower * dy;
    unsigned char result = left* (1-dx)+right* dx;
    return result;
}

void main(int argc, char *argv[])
{
    // char input[1000];
    char message[80] = "Enter [programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n";
    char message2[80] = "ex.) ./blendimages blendimages flowers.bmp jar.bmp 0.5 result3.bmp";
    char message1[80] = "Enter ratio between 0 and 1!\n";
    if(argc < 6)
    {
        printf("%s",message);
        printf("%s\n",message2);
        return;
    }
    
    FILE * foo;
    FILE * file;
    FILE * fool;
    FILE * file2;
    struct tagBITMAPFILEHEADER fh;
    struct tagBITMAPINFOHEADER fih;
    struct tagBITMAPFILEHEADER fh2;
    struct tagBITMAPINFOHEADER fih2;
    float ratio;
    ratio = atof(argv[4]);
    if(ratio > 1 || ratio < 0)
    {
        printf("%s",message1);
        printf("%s",message);
        printf("%s\n",message2);
        return;
    }


    if ((foo = fopen(argv[2],"rb")) == NULL)
    {
        printf("imagefile1 is not a real file, please enter a valid file\n");
        printf("%s",message);
        printf("%s\n",message2);
        return;
    }
    fread(&fh.bfType,1,2,foo);
    fread(&fh.bfSize,1,4,foo);
    fread(&fh.bfReserved1,1,2,foo);
    fread(&fh.bfReserved2,1,2,foo);
    fread(&fh.bfOffBits,1,4,foo);
    fread(&fih,1,sizeof(struct tagBITMAPINFOHEADER),foo);
    unsigned char *data = (unsigned char*)malloc(fih.biSizeImage);
    fread(data,1,fih.biSizeImage,foo);

    
    //This one will be the bigger one.
    if ((fool = fopen(argv[3],"rb")) == NULL)
    {
        printf("imagefile2 is not a real file, please enter a valid file\n");
        printf("%s",message);
        printf("%s\n",message2);
        return;
    }
    fread(&fh2.bfType,1,2,fool);
    fread(&fh2.bfSize,1,4,fool);
    fread(&fh2.bfReserved1,1,2,fool);
    fread(&fh2.bfReserved2,1,2,fool);
    fread(&fh2.bfOffBits,1,4,fool);
    fread(&fih2,1,sizeof(struct tagBITMAPINFOHEADER),fool);
    unsigned char *data2 = (unsigned char*)malloc(fih2.biSizeImage);
    fread(data2,1,fih2.biSizeImage,fool);
    //fclose(fool);
    
    struct tagBITMAPINFOHEADER *ptr = &fih2;
    struct tagBITMAPINFOHEADER *smaller = &fih;
    unsigned char *smalldata = data;
    unsigned char *bigdata = data2;
    
    //have a comparison of the widths/heights
    if ((fih2.biWidth) < (fih.biWidth))
    {
        ptr = &fih;
        smaller = &fih2;
        smalldata = data2;
        bigdata = data;
    }
    

    unsigned char *data3 = (unsigned char*)malloc((*ptr).biSizeImage); //makes new picture the same resolution as the bigger image passed in
    
    for(int y = 0; y < (*ptr).biHeight; y++)
    {
        for (int x = 0; x < (*ptr).biWidth; x++)
        {
            unsigned char pix = get_color(bigdata, x, y, (*ptr).biWidth,0); //access blue for the larger one 
            unsigned char pix1 = get_color(bigdata, x, y, (*ptr).biWidth,1); //access green
            unsigned char pix2 = get_color(bigdata, x, y, (*ptr).biWidth,2); //access red

            float x_smaller = x*((float)(*smaller).biWidth/(float)(*ptr).biWidth);
            float y_smaller = y*((float)(*smaller).biHeight/(float)(*ptr).biHeight);

            unsigned char small_b = get_color_bilinear(smalldata,x_smaller,y_smaller,(*smaller).biWidth,(*smaller).biHeight,0);
            unsigned char small_g = get_color_bilinear(smalldata,x_smaller,y_smaller,(*smaller).biWidth,(*smaller).biHeight,1);
            unsigned char small_r = get_color_bilinear(smalldata,x_smaller,y_smaller,(*smaller).biWidth,(*smaller).biHeight,2);

            unsigned char new = small_b*ratio + pix*(1-ratio);
            unsigned char new1 = small_g*ratio + pix1*(1-ratio);
            unsigned char new2 = small_r*ratio + pix2*(1-ratio);

            if ((fih2.biWidth) < (fih.biWidth))
            {
                new = pix*ratio + small_b*(1-ratio);
                new1 = pix1*ratio + small_g*(1-ratio);
                new2 = pix2*ratio + small_r*(1-ratio);
                //printf("Hi");
            }

            int pos = set_color(x, y, (*ptr).biWidth,0);
            int pos1 = set_color(x, y, (*ptr).biWidth,1);
            int pos2 = set_color(x, y, (*ptr).biWidth,2);

            data3[pos] = new;
            data3[pos1] = new1;
            data3[pos2] = new2;            
            
        }
    }

    fclose(foo);
    fclose(fool);
    
    file2 = fopen(argv[5], "wb+");
    fwrite(&fh2.bfType,1,2,file2);
    fwrite(&fh2.bfSize,1,4,file2);
    fwrite(&fh2.bfReserved1,1,2,file2);
    fwrite(&fh2.bfReserved2,1,2,file2);
    fwrite(&fh2.bfOffBits,1,4,file2);
    fwrite(ptr,1,sizeof(struct tagBITMAPINFOHEADER),file2);
    fwrite(data3,1,(*ptr).biSizeImage,file2);
    fclose(file2);

}