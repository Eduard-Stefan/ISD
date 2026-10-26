# ISD - Text Processing

A C++ console application that counts word frequencies in a text file using multiple worker threads. It reports the total number of words, the number of unique words, the processing time, and the frequency of each word.

## How it works

1. The main thread reads the input file, converts letters to lowercase, and treats non-alphanumeric characters as word separators.
2. Complete words are distributed across worker buffers.
3. Each worker counts its words in a separate frequency map.
4. The main thread waits for the workers, combines their maps, and prints the results in alphabetical order.

Counting is case-insensitive, and numbers are included as words. For example, `Hello, HELLO! 123.` contains three words and two unique words: `hello` appears twice and `123` appears once.

File reading and text preparation happen before the worker threads start. The reported execution time includes reading, preparation, counting, and merging; it excludes printing the results.

## Requirements

- Windows.
- Visual Studio with support for `.slnx` solution files and the **Desktop development with C++** workload.
- MSVC platform toolset **v145** and a Windows SDK, as configured in the project.
- C++20 support.

The current implementation uses Microsoft CRT functions (`fopen_s` and `strtok_s`) and is configured for MSVC.

## Build and run

1. Clone the repository:

   ```powershell
   git clone https://github.com/Eduard-Stefan/ISD.git
   cd ISD
   ```

2. Open `TextProcessing/TextProcessing.slnx` in Visual Studio.
3. Select **Release** and **x64** to build the optimized executable used in the command below.
4. Build the solution with **Ctrl+Shift+B**. The build copies the included input files into a `Resource Files` folder beside the executable.
5. Run the application with **Ctrl+F5**, or run it from the repository root in PowerShell:

   ```powershell
   .\TextProcessing\x64\Release\TextProcessing.exe
   ```

The application locates its input relative to the executable. Keep the `Resource Files` folder beside it if you move the executable.

## Input files

The repository includes three datasets in `TextProcessing/TextProcessing/Resource Files/`:

| File | Nominal size |
| --- | --- |
| `input_10MB.txt` | 10 MB |
| `input_50MB.txt` | 50 MB |
| `input_100MB.txt` | 100 MB |

The default input is `input_100MB.txt`. Text is processed byte by byte using C character classification; Unicode-aware word segmentation is not implemented.

## Execution times

The following execution times were reported from runs in Visual Studio:

| Input file | Execution time (ms) |
| --- | ---: |
| `input_10MB.txt` | 1,642.33 |
| `input_50MB.txt` | 9,006.19 |
| `input_100MB.txt` | 14,271.2 |

The application's internal timer includes file reading, text preparation, word counting, and merging, and excludes printing the results.

Use the same build configuration and debugger settings when comparing results. Debug and Release builds can have different execution times, and hardware, system load, and file caching also affect measurements.

## Configuration

Change these definitions near the top of `TextProcessing/TextProcessing/TextProcessing.cpp`, then rebuild:

```cpp
#define THREAD_COUNT 4
#define INPUT_FILE "input_100MB.txt"
```

- Set `THREAD_COUNT` to a positive integer to choose the number of worker threads.
- Set `INPUT_FILE` to one of the included filenames to choose the dataset.

These settings are selected at compile time; the application does not accept command-line options for them. To use a custom dataset, add it to the project's resource build items so it is copied during the build, or place it manually in the executable's `Resource Files` folder.

For timing comparisons, use the same dataset and build configuration, change the thread count, and repeat each measurement. The reported time covers the whole processing pipeline, including the sequential reading and preparation steps.

## Output

Each run prints:

- The number of worker threads used.
- The total word count.
- The unique word count.
- The execution time in milliseconds.
- Every word and its frequency, sorted alphabetically.

## Project structure

```text
ISD/
├── README.md
└── TextProcessing/
    ├── TextProcessing.slnx
    └── TextProcessing/
        ├── TextProcessing.cpp
        ├── TextProcessing.vcxproj
        ├── TextProcessing.vcxproj.filters
        └── Resource Files/
            ├── input_10MB.txt
            ├── input_50MB.txt
            └── input_100MB.txt
```
