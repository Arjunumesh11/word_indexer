#include <iostream>
#include <ftw.h>
#include <dirent.h>
#include <string.h>
#include <fnmatch.h>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <fstream>
#include <map>
std::queue<std::string> Queue;
struct dirent *drnt;
std::mutex Mutexobject; //queue
std::mutex Mutexmap;    //map;
bool isfinsih = false, isprocessing = true;

std::string GetFilefrmQ()
{
    std::string filename = "";
    Mutexobject.lock();
    if (!Queue.empty())
    {
        filename = Queue.front();
        std::cout << std::endl
                  << " queue " << filename;
        Queue.pop();
    }
    Mutexobject.unlock();
    return filename;
}
void AddWordtoMTable(std::string str)
{
    std::map<std::string, int> Map;

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
    while (!(Queue.empty() && isfinsih))
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
    std::thread T1(FillMTable);
    T1.detach();
    std::thread T2(FillMTable);
    T2.detach();
    std::thread T3(FillMTable);
    T3.detach();
    std::thread T4(FillMTable);
    T4.detach();

    i_dir = opendir("./textfiles");
    drnt = readdir(i_dir);
    closedir(i_dir);
    auto start = std::chrono::high_resolution_clock::now();
    ftw(
        path, [](const char *fpath, const struct stat *sb, int typeflag) { 
       if(typeflag==FTW_F)
        if(fnmatch("*.txt",fpath,FNM_CASEFOLD)==0)
            {
                Mutexobject.lock();
            //    std::cout <<"\n main"<< fpath;
                Queue.push(fpath);
                Mutexobject.unlock();
                 std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
       return 0; }, 16);
    isfinsih = true;
    while (!Queue.empty())
    {
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::multimap<int, std::string> multiMap;

    std::map<std::string, int>::iterator it;
    for (it = Map.begin(); it != Map.end(); it++)
    {
        multiMap.insert(make_pair(it->second, it->first));
    }
    int count = 0;
    for (auto c : multiMap)
    {
        // std::cout << c.first << " " << c.second << std::endl;
        count = count + c.first;
    }
    std::cout << "\n words : " << count;
    std::cout << " Time taken by function: " << duration.count() << " milliseconds" << std::endl;
}