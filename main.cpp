#include <iostream>
#include <cmath>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cstring>
#include <iomanip>
#ifndef SDL_main
#undef main
#endif
using namespace std;
const unsigned MAGIC_FILEID_NUMBER = 0x80120000u;
double scale = 1;
uint8_t quality = 10;
#include "BitSeq.h"
#include "algo1.h" //bitmaps
#include "algo2.h" //1D runs approximated by y = mx + b
#include "algo3.h" //Improved 1D runs
#include "algo4.h" //1D DCT (not implemented)
#include "generic.h"
int main(int argc, char **argv)
{
    ios::sync_with_stdio(false);
    SDL_Init(SDL_INIT_VIDEO);
    while(true)
    {
        string s;
        cout << ">";
        getline(cin, s);
        s += '*';
        string instruction;
        for(int i=0; i<s.size()-1; i++)
        {
            if(s[i] == ' ')
            {
                instruction = s.substr(0, i);
                s = s.substr(i+1);
                break;
            }
        }
        s.pop_back();
        if(instruction == "encode1")
            encode(s, algo1::encode1);
        else if(instruction == "encode2")
            encode(s, algo2::encode2);
        else if(instruction == "encode3")
            encode(s, algo3::encode3);
        else if(instruction == "encode4")
            encode(s, algo4::encode4);
        else if(instruction == "decode" || instruction == "display")
            display(s);
        else if(instruction == "scale")
        {
            if(s.empty())
                cout << scale << endl;
            else
            {
                scale = atof(s.c_str());
                if(scale > 5)
                    cout << "warning: large scales may cause lagg" << endl;
            }
        }
        else if(instruction == "quality")
        {
            if(s.empty())
                cout << quality << endl;
            else
            {
                quality = atoi(s.c_str());
            }
        }
        else cout << "error: unknown command\n";
    }
    return 0;
}
