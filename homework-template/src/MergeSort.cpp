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

// 生成隨機數生成器
static mt19937_64 rng(std::random_device{}());

void printMemoryUsage() {
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    cout << "----------------------------------------------------------\n";
    cout << "Working Set Size: " << memInfo.WorkingSetSize / 1024 << " KB\n";
    cout << "Peak Working Set Size: " << memInfo.PeakWorkingSetSize / 1024 << " KB\n";
    cout << "Pagefile Usage: " << memInfo.PagefileUsage / 1024 << " KB\n";
    cout << "----------------------------------------------------------\n";
}

// 將 initList[l..m] 與 initList[m+1..n] 的兩段已排序子陣列
// 合併到 mergedList[l..n]。需要額外 O(n) 空間。
template<class T>
void Merge(T* initList, T* mergedList,const int l, const int m, const int n)
{
    int i1 = l, i2 = m + 1, iResult = l;
    // 只要兩段都有剩餘，就比大小、放進 mergedList
    for (; i1 <= m && i2 <= n; ++iResult) 
    {
        if (initList[i1] <= initList[i2]) 
        {
            mergedList[iResult] = initList[i1++];
        }
        else 
        {
            mergedList[iResult] = initList[i2++];
        }
    }
    // 複製第一段剩餘
    copy(initList + i1, initList + m + 1, mergedList + iResult);
    iResult += (m - i1 + 1);
    // 複製第二段剩餘
    copy(initList + i2, initList + n + 1, mergedList + iResult);
}

// 實際的 Merge Sort 遞迴
void mergeSortRec(int* a, int* tmp, int l, int r) 
{
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSortRec(a, tmp, l, m);
    mergeSortRec(a, tmp, m + 1, r);
    Merge(a, tmp, l, m, r);
    // 再把 tmp[l..r] 複製回 a[l..r]
    copy(tmp + l, tmp + r + 1, a + l);
}

// 方便呼叫
void mergeSort(int* a, int n) 
{
    int* tmp = new int[n + 1];      
    mergeSortRec(a, tmp, 1, n);
    delete[] tmp;
}

// Average Case：使用讀檔的方式(為了使用同一筆測資算出平均的準確度)
//讀取 "<n>.txt" n 個整數，重複排序 REPS 次並取平均
void averageCase(int n) {
    const int REPS = 2500;
    string filename = to_string(n) + ".txt";

    ifstream fin(filename);
    if (!fin) {
        cerr << "無法打開 " << filename << "，請確認檔案存在。" << endl;
        return;
    }
    string line;
    if (!getline(fin, line)) {
        cerr << filename << " 讀取失敗或為空檔案。" << endl;
        return;
    }
    fin.close();

    istringstream iss(line);
    vector<int> orig(n);
    for (int i = 0; i < n; ++i) {
        if (!(iss >> orig[i])) {
            cerr << filename << " 中第 " << (i + 1) << " 個數字解析失敗。" << endl;
            return;
        }
    }

    
    long long totalTime = 0;
    cout << "===== Average Case ===== n = " << n
        << "，重複排序 " << REPS << " 次" << endl;

    printMemoryUsage();               // 排序前的記憶體使用量
    int* arr = new int[n + 1];        // a[0..n]

    for (int rep = 0; rep < REPS; ++rep) {
        // 複製原始資料到 a[1..n]
        for (int j = 0; j < n; ++j)
            arr[j + 1] = orig[j];

        auto start = chrono::high_resolution_clock::now();
        mergeSort(arr, n);
        auto end = chrono::high_resolution_clock::now();
        totalTime += chrono::duration_cast<chrono::microseconds>(end - start).count();
    }

    printMemoryUsage();               // 排序後（尚未釋放記憶體)的記憶體使用量
    double avgTime = double(totalTime) / REPS;
    cout << "Average execution time: " << avgTime << " μs" << endl;

    delete[] arr;
    printMemoryUsage();               // 釋放後的記憶體使用量
}


// Worst Case：先找出「最慢」的那組隨機向量，再排序一次
vector<int> genRandom(int n) 
{
    static mt19937_64 rng(random_device{}());
    uniform_int_distribution<int> dist(0, 1000000);
    vector<int> v(n);
    for (int& x : v) x = dist(rng);
    return v;
}

vector<int> genWorstCaseSample(int n, int trials = 20) 
{
    long long maxT = -1;
    vector<int> worst;
    int* tmp = new int[n + 1];
    for (int t = 0; t < trials; ++t)
    {
        auto v = genRandom(n);
        for (int i = 0; i < n; ++i) tmp[i + 1] = v[i];
        auto t0 = chrono::high_resolution_clock::now();
        mergeSort(tmp, n);
        auto t1 = chrono::high_resolution_clock::now();
        long long dt = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
        if (dt > maxT) 
        {
            maxT = dt;
            worst = move(v);
        }
    }
    delete[] tmp;
    return worst;
}


void worstCase(int n) 
{
    cout << "===== Worst Case (random) Merge Sort n=" << n << " =====\n";
    auto sample = genWorstCaseSample(n);
    printMemoryUsage();      // 排序前的記憶體使用量

    int* arr = new int[n + 1];
    for (int i = 0; i < n; ++i) arr[i + 1] = sample[i];
    auto t0 = chrono::high_resolution_clock::now();
    mergeSort(arr, n);
    auto t1 = chrono::high_resolution_clock::now();

    printMemoryUsage();       // 排序後（尚未釋放記憶體)的記憶體使用量
    cout << "Worst-case Time: "
        << chrono::duration_cast<chrono::microseconds>(t1 - t0).count()
        << " μs\n";
    delete[] arr;
    printMemoryUsage();      // 釋放後的記憶體使用量
}

int main() 
{
    int sizes[] = { 500,1000,2000,3000,4000,5000 };
    cout << " 資料筆數: \n ";
    for (int i = 0; i < 6; ++i) cout << i + 1 << '.' << sizes[i] << ' ';
    cout << "\n請選擇(1~6):";
    int c; cin >> c; 
    if (c < 1 || c > 6)
    {
        cerr << "輸入無效,請輸入1~6" << endl;
        return 1;
    }

    int n = sizes[c - 1];       //根據輸入的選項，決定接下來要排序的n筆資料筆數

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
