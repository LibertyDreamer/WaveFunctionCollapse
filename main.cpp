#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <set>
#include <algorithm>
#include <iostream>
#include <random>
#include <iomanip>

int generate_random_int(int min, int max) {
    // Create a random device and seed the generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the distribution range
    std::uniform_int_distribution<> dist(min, max);

    // Generate and return the random number
    return dist(gen);
}

struct Color {
    uint8_t R, G, B;
};

class PPM {
    uint width, height;
    Color *color_array;

public:
    PPM(uint width, uint height) : width(width), height(height) {
        color_array = static_cast<Color*>(malloc(width * height * sizeof(Color)));
    }

    PPM(const char* name) {
        FILE *fp = fopen(name, "rb");
        if (!fp) {
            perror("Unable to open file");
            exit(EXIT_FAILURE);
        }

        // Read the PPM magic number ("P6")
        char header_has_to_be[3] = {0x50, 0x36 , 0x0A};
        char header[3];
        fread(header, 1, 3, fp);

        for(int i =0; i < 3; ++i)
        {
            if (header_has_to_be[i] != header[i])
            {
                perror("Invalid PPM file");
                fclose(fp);
                exit(EXIT_FAILURE);
            }
        }

        // Skip comments or non-standard lines
        int c;
        while ((c = fgetc(fp)) != EOF && c == '#') {
            while ((c = fgetc(fp)) != EOF && c != 0x0A);  // Skip the rest of the comment line
        }
        ungetc(c, fp);  // Put back the last read character if it's not a comment

        // Read the width, height, and maximum color value (255 expected)
        if (fscanf(fp, "%d %d", &width, &height) != 2) {
            perror("Invalid width/height");
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        int max_val;
        if (fscanf(fp, "%d", &max_val) != 1 || max_val != 255) {
            perror("Invalid max color value or not 255");
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        // Skip the newline character after max_val
        fgetc(fp);

        // Allocate memory for the color array
        color_array = static_cast<Color*>(malloc(width * height * sizeof(Color)));
        if (!color_array) {
            perror("Memory allocation failed");
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        // Read the pixel data
        size_t pixels_read = fread(color_array, sizeof(Color), width * height, fp);
        if (pixels_read != static_cast<size_t>(width * height)) {
            perror("Error reading pixel data");
            fclose(fp);
            free(color_array);
            exit(EXIT_FAILURE);
        }

        fclose(fp);
    }

    ~PPM() {
        free(color_array); // Free the allocated memory
    }

    void generate_something() {
        for (uint j = 0; j < height; ++j) {
            for (uint i = 0; i < width; ++i) {
                 color_array[j * width + i] = Color{static_cast<uint8_t>(i % 256), static_cast<uint8_t>(j % 256), static_cast<uint8_t>((i * j) % 256)};
            }
        }
    }

    void set_p(int x, int y, Color c)
    {
        color_array[y * width + x] = c;
    }

    Color get_p(int x, int y)
    {
        return color_array[y * width + x];
    }


    void write(const char* name) {
        FILE *fp = fopen(name, "wb");
        (void) fprintf(fp, "P6\n%d %d\n255\n", width, height);

        // Write the entire color array to the file
        for (uint j = 0; j < height; ++j) {
            fwrite(&color_array[j * width], sizeof(Color), width, fp);
        }

        (void) fclose(fp);
    }

};


struct Pattern{
    std::set<int> up;
    std::set<int> down;
    std::set<int> left;
    std::set<int> right;
}
p0{{0,4}, {0,1}, {0,2}, {0,3}},
p1{{0,4}, {2,3,4}, {1,3,4}, {1,2,4}},
p2{{1,2,3}, {2,3,4}, {1,3,4}, {0,3}},
p3{{2,3,1}, {4,2,3}, {0,2}, {1,2,4}},
p4{{1,2,3}, {0,1}, {1,3,4}, {1,2,4}};


Pattern patterns[5] = {p0,p1,p2,p3,p4};


int map[10][10];


bool check(int x, int y)
{
    int up, down, left, right;


    if (y + 1 > 10) up = -1;
    else up = map[x][y+1];

    if (y - 1 < 0) down = -1;
    else down = map[x][y-1];

    if (x + 1 > 10) right = -1;
    else right = map[x+1][y];

    if (x - 1 < 0) left = -1;
    else left = map[x-1][y];

    if(left == -1)
    if(right == -1)
    if(up == -1)
    if(down == -1) return true;


    std::set<int> intersect = {0,1,2,3,4};

    if (up != -1)
    {
        std::set<int> intersect1;
        std::set_intersection(patterns[up].down.begin(),patterns[up].down.end(), intersect.begin(), intersect.end(), std::inserter(intersect1, intersect1.begin()));
        intersect = intersect1;
    }

    if (down != -1)
    {
        std::set<int> intersect1;
        std::set_intersection(patterns[down].up.begin(),patterns[down].up.end(), intersect.begin(), intersect.end(), std::inserter(intersect1, intersect1.begin()));
        intersect = intersect1;
    }

    if (right != -1)
    {
        std::set<int> intersect1;
        std::set_intersection(patterns[right].left.begin(),patterns[right].left.end(), intersect.begin(), intersect.end(), std::inserter(intersect1, intersect1.begin()));
        intersect = intersect1;
    }

    if (left != -1)
    {
        std::set<int> intersect1;
        std::set_intersection(patterns[left].right.begin(),patterns[left].right.end(), intersect.begin(), intersect.end(), std::inserter(intersect1, intersect1.begin()));
        intersect = intersect1;
    }

    if(intersect.size() == 0) return false;


    map[x][y] = *std::next(intersect.begin(), generate_random_int(0,intersect.size()-1));

    return true;
}


void print_map()
{
    for(int i =0; i < 20; ++i)
    {
        for (int j = 0; j < 20; ++j){
            std::cout << std::setw(2) << map[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void map_init()
{
    for(int i =0; i < 20; ++i)
    {
        for (int j = 0; j < 20; ++j){
            map[i][j] = -1;
        }

    }
}

void pixel(int r, int g, int b)
{
    printf("\x1b[38;2;%d;%d;%dm█", r, g, b);
}

int main(void)
{
    map_init();
    map[5][5] = 2;

    while(true)
    {
        if(check(generate_random_int(0,19), generate_random_int(0,19)) == false)
        {
            //map_init();
            //map[5][5] = 2;
        }
        //print_map();

        int flag = 0;
        for(int i = 0; i < 20; ++i)
        {
            for(int j = 0; j < 20; ++j)
            {
                if (map[i][j] == -1)  flag = 1;
                 //std::cout << std::setw(2) << map[i][j] << " ";
            }
            //std::cout << std::endl;
        }
        //std::cout << std::endl;

        if(flag == 0) break;
    }

    print_map();

    PPM ppms[5] = {
        PPM ("0.ppm"),
        PPM ("1.ppm"),
        PPM ("2.ppm"),
        PPM ("3.ppm"),
        PPM ("4.ppm")};

    PPM result (1000,1000);

    for(int i = 0; i < 20; ++i)
    {
        for(int j = 0; j < 20; ++j)
        {
            int val = map[i][j];
            if (val != -1)
            {
                for(int k = 0; k < 50; ++k)
                {
                    for(int t = 0; t < 50; ++t)
                    {
                        result.set_p(k+(i*50),t+(j*50), ppms[val].get_p(k,t));
                    }
                }
            }

        }
    }

    result.write("result.ppm");



    return EXIT_SUCCESS;
}
