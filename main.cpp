#include "CImg.h"

#define NUM_TOTAL_IMAGE 3

using namespace cimg_library;
using namespace std;

typedef CImg<unsigned char> Image;

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

int main()
{
    const char *file_name[NUM_TOTAL_IMAGE] = {"Data/1.png", "Data/2.png", "Data/3.png"};
    Image src[NUM_TOTAL_IMAGE];

    read_data(file_name, src);

    // do something else ...

    show_result(file_name, src);

    return 0;
}