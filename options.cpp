/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/Options.hpp"

u32 Options::SPACE_SIZE;
u32 Options::ATOMIC_POV_COUNT;
u32 Options::RAY_STEP;

void Options::process(int argc, char **argv)
{
    int i = 1;
    while(i < argc){    
        const std::string name = argv[i];
        if(name.find("space-size:") == 0)
        {
           Options::SPACE_SIZE = std::stoi(name.substr(11)); 
        } else if(name.find("atomic-pov-count:") == 0)
        {
            Options::ATOMIC_POV_COUNT = std::stoi(name.substr(17));
        } else if(name.find("ray-step:") == 0)
        {
            Options::RAY_STEP = std::stoi(name.substr(9));
        }
        i++;
    }
}
