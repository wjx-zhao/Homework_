#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <Windows.h>
#include <Psapi.h>
#include <random> // 確保包含此標頭檔案

using namespace std;

void printMemoryUsage() {
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    cout << "----------------------------------------------------------\n";
    cout << "Working Set Size:      " << memInfo.WorkingSetSize / 1024 << " KB\n";
    cout << "Peak Working Set Size: " << memInfo.PeakWorkingSetSize / 1024 << " KB\n";
    cout << "Pagefile Usage:        " << memInfo.PagefileUsage / 1024 << " KB\n";
    cout << "----------------------------------------------------------\n";
}

// Adjust是一個以 root 為根的子樹，使其滿足 max‐heap 性質
template<class T>
void Adjust(T* a, const int root, const int n) 
{
    T e = a[root];
    int j;
    for (j = 2 * root; j <= n; j *= 2) 
    {
        if (j < n && a[j] < a[j + 1])
            ++j;                // 選擇較大的子節點
        if (e >= a[j])
            break;              // e 可插入在 j/2
        a[j / 2] = a[j];          // 將較大子節點上移
    }
    a[j / 2] = e;
}


template<class T>
void heapSort(T* a, const int n) 
{
    // 建堆 (heapify)
    for (int i = n / 2; i >= 1; --i)
        Adjust(a, i, n);

    // 反覆取出最大元素到尾端，並重建堆
    for (int i = n - 1; i >= 1; --i) 
    {
        swap(a[1], a[i + 1]);   // 將堆頂與未排序區的最後一個交換
        Adjust(a, 1, i);      // 重新調整長度為 i 的堆
    }
}


static mt19937_64 rng(random_device{}());
vector<int> genRandom(int n) 
{
    uniform_int_distribution<int> dist(0, 1000000);
    vector<int> v(n);
    for (int& x : v) x = dist(rng);
    return v;
}

// 在多組隨機資料中，找出對 heapSort 最慢的那一組
vector<int> genWorstCaseSample(int n, int trials = 20) 
{
    long long maxT = -1;
    vector<int> worst;
    int* buf = new int[n + 1];
    for (int t = 0; t < trials; ++t) 
    {
        auto v = genRandom(n);
        for (int i = 0; i < n; ++i) buf[i + 1] = v[i];
        auto t0 = chrono::high_resolution_clock::now();
        heapSort(buf, n);
        auto t1 = chrono::high_resolution_clock::now();
        long long dt = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
        if (dt > maxT)
        {
            maxT = dt;
            worst = move(v);
        }
    }
    delete[] buf;
    return worst;
}

// Average Case：使用讀檔的方式(為了使用同一筆測資算出平均的準確度)
//讀取 "<n>.txt" n 個整數，重複排序 REPS 次並取平均
void averageCase(int n) {
    const int REPS = 2500;
    string filename = to_string(n) + ".txt";

    ifstream fin(filename);
    if (!fin) {
        cerr << "無法打開 " << filename << "\n";
        return;
    }
    string line;
    getline(fin, line);
    fin.close();

    istringstream iss(line);
    vector<int> orig(n);
    for (int i = 0; i < n; ++i)
        iss >> orig[i];

    cout << "===== Average Case (Heap Sort) ===== n = " << n
        << "，重複 " << REPS << " 次\n";
    printMemoryUsage();

    long long totalTime = 0;
    int* arr = new int[n + 1];   

    for (int rep = 0; rep < REPS; ++rep) {
        for (int i = 0; i < n; ++i)
            arr[i + 1] = orig[i];

        auto t0 = chrono::high_resolution_clock::now();
        heapSort(arr, n);
        auto t1 = chrono::high_resolution_clock::now();
        totalTime += chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    }

    printMemoryUsage();
    cout << "Average execution time: "
        << (double)totalTime / REPS << " μs\n";

    delete[] arr;
    printMemoryUsage();
}

// Worst Case：隨機產生多筆，保留排序最慢那筆，再做一次測量
void worstCase(int n) {
    cout << "===== Worst Case (random) Heap Sort ===== n = " << n << "\n";
    auto sample = genWorstCaseSample(n);

    printMemoryUsage();
    int* arr = new int[n + 1];
    for (int i = 0; i < n; ++i)
        arr[i + 1] = sample[i];

    auto t0 = chrono::high_resolution_clock::now();
    heapSort(arr, n);
    auto t1 = chrono::high_resolution_clock::now();

    printMemoryUsage();
    cout << "Worst-case execution time: "
        << chrono::duration_cast<chrono::microseconds>(t1 - t0).count()
        << " μs\n";

    delete[] arr;
    printMemoryUsage();
}

int main() 
{
    int sizes[] = { 500, 1000, 2000, 3000, 4000, 5000 };
    cout << "資料筆數: \n ";
    for (int i = 0; i < 6; ++i)
        cout << (i + 1) << "." << sizes[i] << "  ";
    int c;
    cout << "\n請選擇(1~6):";
    cin >> c;
    if (c < 1 || c > 6) 
    {
        cerr << "輸入無效,請輸入1~6";
        return 1;
    }
    int n = sizes[c - 1];

    cout << "選(1 = Average Case, 2 = Worst Case): ";
    int m;
    cin >> m;
    if (m == 1)
    {
        averageCase(n);
    }
    else if (m == 2)
    {
        worstCase(n);
    }
    else
    {
        cerr << "輸入錯誤,請輸入1或2決定模式" << endl;
        return 1;
    }

    system("pause");
    return 0;
}
