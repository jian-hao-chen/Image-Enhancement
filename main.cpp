#include "CImg.h"

#define NUM_TOTAL_IMAGE 3

using namespace cimg_library;
using namespace std;

typedef CImg<unsigned char> Image;
typedef struct Pixel Pixel;

struct Pixel
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char Y;
};

void read_data(const char *file_name[], Image *dst)
{
    for (int i = 0; i < NUM_TOTAL_IMAGE; i++)
    {
        dst[i].load(file_name[i]);
    }
}

void show_result(const char *file_name[], Image *src)
{
    for (int i = 0; i < NUM_TOTAL_IMAGE; i++)
    {
        src[i].display(file_name[i]);
    }
}

void bubble_sort(unsigned char *src, int length)
{
    // sort function for median_filter, compare Y value then swap the whole Pixel variable.
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < length - i; j++)
        {
            if (src[j + 1] < src[j])
            {
                unsigned char temp = src[j + 1];
                src[j + 1] = src[j];
                src[j] = temp;
            }
        }
    }
}

void median_filter(Image *src)
{
    Image padding_src(src->width() + 2, src->height() + 2, 1, 3, 0);
    unsigned char filter[9];

    // padding source image to deal with noise at the edge.
    for (int h = 0; h < src->height(); h++)
    {
        for (int w = 0; w < src->width(); w++)
        {
            for (int channel = 0; channel < 3; channel++)
            {
                padding_src(w + 1, h + 1, 0, channel) = (*src)(w, h, 0, channel);
            }
        }
    }

    // process padding image
    for (int channel = 0; channel < 3; channel++)
    {
        for (int h = 1; h < padding_src.height() - 1; h++)
        {
            for (int w = 1; w < padding_src.width() - 1; w++)
            {
                for (int f_h = 0; f_h < 3; f_h++)
                {
                    for (int f_w = 0; f_w < 3; f_w++)
                    {
                        int addr_w = w - 1 + f_w;
                        int addr_h = h - 1 + f_h;
                        filter[f_w + f_h * 3] = padding_src(addr_w, addr_h, 0, channel);
                    }
                }

                bubble_sort(filter, 9);

                // the Median is the 5th value of array(filter[4]).
                (*src)(w - 1, h - 1, 0, channel) = filter[4];
            }
        }
    }

    // // rewrite to the source image
    // for (int h = 1; h < padding_src.height() - 1; h++)
    // {
    //     for (int w = 1; w < padding_src.width() - 1; w++)
    //     {
    //         for (int channel = 0; channel < 3; channel++)
    //         {
    //             (*src)(w - 1, h - 1, 0, channel) = padding_src(w, h, 0, channel);
    //         }
    //     }
    // }
}

int main()
{
    const char *file_name[NUM_TOTAL_IMAGE] = {"Data/1.png", "Data/2.png", "Data/3.png"};
    Image src[NUM_TOTAL_IMAGE];

    read_data(file_name, src);

    // do something else ...
    for (int idx = 0; idx < NUM_TOTAL_IMAGE; idx++)
    {
        median_filter(&(src[idx]));
    }

    show_result(file_name, src);

    return 0;
}