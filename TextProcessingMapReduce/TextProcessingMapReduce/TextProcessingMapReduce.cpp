#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

using namespace std;

#define THREAD_COUNT 4
#define INPUT_FILE "input_400MB.txt"

struct Mapper
{
    string text;
    map<string, long long> counts;
};

struct Reducer
{
    map<string, vector<long long>> groups;
    map<string, long long> counts;
};

void mapWords(struct Mapper* mapper)
{
    char* context = NULL;
    char* word = strtok_s(mapper->text.data(), " ", &context);

    while (word != NULL)
    {
        mapper->counts[word]++;
        word = strtok_s(NULL, " ", &context);
    }
}

void reduceWords(struct Reducer* reducer)
{
    for (const auto& entry : reducer->groups)
    {
        long long count = 0;
        for (long long partialCount : entry.second)
        {
            count += partialCount;
        }
        reducer->counts[entry.first] = count;
    }
}

int main(int argc, char* argv[])
{
    auto start = chrono::steady_clock::now();

    filesystem::path path = filesystem::path(argv[0]).parent_path() / "input" / INPUT_FILE;
    FILE* file = NULL;
    if (fopen_s(&file, path.string().c_str(), "rb") != 0)
    {
        cout << "Cannot open " << INPUT_FILE << ". Build the project first.\n";
        return 1;
    }

    struct Mapper mappers[THREAD_COUNT];
    int part = 0;
    int character;

    while ((character = fgetc(file)) != EOF)
    {
        if (isalnum(character))
        {
            mappers[part].text += (char)tolower(character);
        }
        else
        {
            mappers[part].text += ' ';
            part = (part + 1) % THREAD_COUNT;
        }
    }

    int readError = ferror(file);
    fclose(file);
    if (readError)
    {
        cout << "An error occurred while reading " << INPUT_FILE << ".\n";
        return 1;
    }

    thread threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads[i] = thread(mapWords, &mappers[i]);
    }
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads[i].join();
    }

    struct Reducer reducers[THREAD_COUNT];
    hash<string> wordHash;
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        for (const auto& entry : mappers[i].counts)
        {
            int shard = wordHash(entry.first) % THREAD_COUNT;
            reducers[shard].groups[entry.first].push_back(entry.second);
        }
    }

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads[i] = thread(reduceWords, &reducers[i]);
    }

    map<string, long long> counts;
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads[i].join();
        counts.merge(reducers[i].counts);
    }

    long long totalWords = 0;
    for (const auto& entry : counts)
    {
        totalWords += entry.second;
    }

    double executionTime = chrono::duration<double, milli>(chrono::steady_clock::now() - start).count();

    cout << "Threads used: " << THREAD_COUNT << "\n";
    cout << "Total words: " << totalWords << "\n";
    cout << "Unique words: " << counts.size() << "\n";
    cout << "Execution time: " << executionTime << " ms\n\n";

    for (const auto& entry : counts)
    {
        cout << entry.first << ": " << entry.second << "\n";
    }

    return 0;
}
