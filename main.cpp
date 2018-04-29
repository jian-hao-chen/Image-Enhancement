#include "CImg.h"
#include <math.h>

#define NUM_TOTAL_IMAGE 3

using namespace cimg_library;
using namespace std;

typedef CImg<int> Image;
typedef struct Pixel Pixel;

struct Pixel
{
    // color[0] = R, color[1] = G, color[2] = B
    unsigned char color[3];
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

void save_result(const char *file_name[], Image *src)
{
    for (int i = 0; i < NUM_TOTAL_IMAGE; i++)
    {
        src[i].save(file_name[i]);
    }
}

void inverse(Image *src)
{
    for (int h = 0; h < src->height(); h++)
    {
        for (int w = 0; w < src->width(); w++)
        {
            for (int channel = 0; channel < 3; channel++)
            {
                (*src)(w, h, 0, channel) = 255 - (*src)(w, h, 0, channel);
            }
        }
    }
}

void bubble_sort(unsigned char *src, int length)
{
    // sort function for median_filter, compare Y value then swap the whole Pixel variable.
    unsigned char temp;
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < length - i; j++)
        {
            if (src[j + 1] < src[j])
            {
                temp = src[j + 1];
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
}

void histogram_equalization(Image *src)
{
    Image temp, histogram, result;
    int cdf[256] = {0}, equalized_table[256] = {0};
    int cdf_min, idx_min;
    int width = src->width();
    int height = src->height();
    int num_pixel = width * height;

    // copy to temp variable and get histogram.
    temp = src->get_RGBtoYCbCr();
    histogram = temp.get_channel(0).get_histogram(256, 0, 255);

    // compute cumulative distribution function
    cdf[0] = histogram(0, 0, 0, 0);
    for (int scale = 1; scale < 256; scale++)
    {
        cdf[scale] = cdf[scale - 1] + histogram(scale, 0, 0, 0);
    }

    // find cdf minimun value
    for (int scale = 0; scale < 256; scale++)
    {
        if (cdf[scale] > 0)
        {
            cdf_min = cdf[scale];
            idx_min = scale;
            break;
        }
    }

    // equalization and mapping
    for (int scale = idx_min; scale < 256; scale++)
    {
        equalized_table[scale] = (int)((cdf[scale] - cdf_min) * 255.0 / (num_pixel - cdf_min));
    }

    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            temp(w, h, 0, 0) = equalized_table[(int)temp(w, h, 0, 0)];
        }
    }

    // convert to RGB and rewrite to source.
    result = temp.get_YCbCrtoRGB();
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            for (int channel = 0; channel < 3; channel++)
            {
                (*src)(w, h, 0, channel) = result(w, h, 0, channel);
            }
        }
    }
}

void log_transform(Image *src)
{
    // formula : y = ln(x + 1) * 255 / ln(256)
    for (int h = 0; h < src->height(); h++)
    {
        for (int w = 0; w < src->width(); w++)
        {
            for (int channel = 0; channel < 3; channel++)
            {
                (*src)(w, h, 0, channel) = (int)(log((*src)(w, h, 0, channel) + 1) * 255 / log(256));
            }
        }
    }
}

int main()
{
    const char *file_name[NUM_TOTAL_IMAGE] = {"Data/1.png", "Data/2.png", "Data/3.png"};
    const char *output_name[NUM_TOTAL_IMAGE] = {"result_1.png", "result_2.png", "result_3.png"};
    Image src[NUM_TOTAL_IMAGE];

    read_data(file_name, src);

    // first image
    log_transform(&src[0]);
    
    // second image
    median_filter(&src[1]);

    // third image
    inverse(&src[2]);
    

    save_result(output_name, src);
    show_result(output_name, src);

    return 0;
}