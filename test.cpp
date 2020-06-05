#include <iostream>
#include <ftw.h>
#include <dirent.h>
#include <string.h>
#include <fnmatch.h>
#include <queue>
#include <mutex>
#include <thread>
#include <fstream>
#include <map>
std::queue<std::string> Queue;
std::map<std::string, int> Map;
struct dirent *drnt;
std::mutex Mutexobject; //queue
std::mutex Mutexmap;    //map;
bool isfinsih = false;

std::string GetFilefrmQ()
{
    std::string filename = "";
    Mutexobject.lock();
    if (!Queue.empty())
    {
        filename = Queue.front();
        std::cout << " queue " << filename;
        Queue.pop();
    }
    Mutexobject.unlock();
    return filename;
}
void AddWordtoMTable(std::string str)
{

    Mutexmap.lock();
    std::map<std::string, int>::iterator it = Map.find(str);
    if (it != Map.end())
        it->second = it->second + 1;
    else
    {
        Map.insert(std::make_pair(str, 1));
    }
    Mutexmap.unlock();
}
void FillMTable()
{
    while (!isfinsih)
    {
        std::string filename = GetFilefrmQ();
        if (!filename.empty())
        {
            std::cout << filename;
            std::ifstream file(filename.c_str());
            std::string line;

            if (file.is_open())
            {
                while (getline(file, line))
                {
                    std::size_t prev = 0, pos;
                    while ((pos = line.find_first_of("~`=!@#$%^&*)/\?-_|[,. }](_-+{;':"
                                                     "></",
                                                     prev)) != std::string::npos)
                    {
                        if (pos > prev)
                        {
                            AddWordtoMTable(line.substr(prev, pos - prev));
                        }
                        prev = pos + 1;
                    }
                    if (prev < line.length())
                    {
                        AddWordtoMTable(line.substr(prev, std::string::npos));
                    }
                }

                file.close();
            }
            else
            {
                std::cout << "-Unable to open file-" << std::endl;
            }
        }
    }
}
int main()
{
    const char *path = strdup("./textfiles");
    DIR *i_dir;
    std::thread T(FillMTable);
    i_dir = opendir("./textfiles");
    drnt = readdir(i_dir);
    closedir(i_dir);
    ftw(path, [](const char *fpath, const struct stat *sb, int typeflag) { 
       if(typeflag==FTW_F)
        if(fnmatch("*.txt",fpath,FNM_CASEFOLD)==0)
            {
                Mutexobject.lock();
                std::cout <<"\n main"<< fpath;
                Queue.push(fpath);
                Mutexobject.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
       return 0; }, 16);
    isfinsih = true;
    T.join();
    for (auto c : Map)
    {
        std::cout << c.first << " " << c.second << std::endl;
    }
}