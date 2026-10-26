#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <thread>

using namespace std;

#define THREAD_COUNT 4
#define INPUT_FILE "input_100MB.txt"

struct Worker
{
    string text;
    map<string, long long> counts;
};

void countWords(struct Worker* worker)
{
    char* context = NULL;
    char* word = strtok_s(worker->text.data(), " ", &context);

    while (word != NULL)
    {
        worker->counts[word]++;
        word = strtok_s(NULL, " ", &context);
    }
}

int main(int argc, char* argv[])
{
    auto start = chrono::steady_clock::now();

    filesystem::path path = filesystem::path(argv[0]).parent_path() / "Resource Files" / INPUT_FILE;
    FILE* file = NULL;
    if (fopen_s(&file, path.string().c_str(), "rb") != 0)
    {
        cout << "Cannot open " << INPUT_FILE << ". Build the project first.\n";
        return 1;
    }

    struct Worker workers[THREAD_COUNT];
    int part = 0;
    int character;

    while ((character = fgetc(file)) != EOF)
    {
        if (isalnum(character))
        {
            workers[part].text += (char)tolower(character);
        }
        else
        {
            workers[part].text += ' ';
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
        threads[i] = thread(countWords, &workers[i]);
    }

    map<string, long long> counts;
    long long totalWords = 0;
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads[i].join();
        for (auto entry = workers[i].counts.begin(); entry != workers[i].counts.end(); entry++)
        {
            counts[entry->first] += entry->second;
            totalWords += entry->second;
        }
    }

    double executionTime = chrono::duration<double, milli>(chrono::steady_clock::now() - start).count();

    cout << "Threads used: " << THREAD_COUNT << "\n";
    cout << "Total words: " << totalWords << "\n";
    cout << "Unique words: " << counts.size() << "\n";
    cout << "Execution time: " << executionTime << " ms\n\n";

    for (auto entry = counts.begin(); entry != counts.end(); entry++)
    {
        cout << entry->first << ": " << entry->second << "\n";
    }

    return 0;
}
