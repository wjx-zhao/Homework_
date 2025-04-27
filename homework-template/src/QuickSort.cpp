#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <chrono>
#include <Windows.h>
#include <Psapi.h>
using namespace std;


// 印出記憶體使用量
void printMemoryUsage() 
{
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    cout << "----------------------------------------------------------" << endl;
    cout << "Working Set Size:      " << memInfo.WorkingSetSize / 1024 << " KB" << endl;
    cout << "Peak Working Set Size: " << memInfo.PeakWorkingSetSize / 1024 << " KB" << endl;
    cout << "Pagefile Usage:        " << memInfo.PagefileUsage / 1024 << " KB" << endl;
    cout << "----------------------------------------------------------" << endl;
}


// 使用尾端遞迴消除，pivot = a[left]，
// a[0] 暫存 pivot，a[right+1] 作分割哨兵
void quickSort(int* a, int left, int right) 
{
    while (left < right) 
    {
        int pivot = a[left];
        int i = left, j = right + 1;
        a[0] = pivot;           // 暫存 pivot
        a[right + 1] = pivot;   // 分割哨兵

        // partition
        do 
        {
            do { ++i; } while (a[i] < pivot);
            do { --j; } while (a[j] > pivot);
            if (i < j) swap(a[i], a[j]);
        } while (i < j);

        swap(a[left], a[j]);    // 把 pivot 放到 j

        // 只對「較小那一半」做遞迴，其它用迴圈處理
        if (j - left < right - j) 
        {
            quickSort(a, left, j - 1);
            left = j + 1;
        }
        else 
        {
            quickSort(a, j + 1, right);
            right = j - 1;
        }
    }
}


// Average Case：從 "<n>.txt" 讀單行 n 個整數，重複排序 REPS 次取平均
void averageCase(int n) 
{
    const int REPS = 2500;
    string filename = to_string(n) + ".txt";

    ifstream fin(filename);
    if (!fin) 
    {
        cerr << "無法打開 " << filename << "！" << endl;
        return;
    }
    string line;
    getline(fin, line);
    fin.close();

    istringstream iss(line);
    vector<int> orig(n);
    for (int i = 0; i < n; ++i) 
    {
        if (!(iss >> orig[i])) 
        {
            cerr << filename << " 解析第 " << (i + 1) << " 個數失敗。" << endl;
            return;
        }
    }

    long long totalTime = 0;
    cout << "===== Average Case ===== n=" << n
        << "，重複排序 " << REPS << " 次" << endl;

    printMemoryUsage();               // 排序前的記憶體使用量
    int* arr = new int[n + 2];        // a[0..n+1]

    for (int rep = 0; rep < REPS; ++rep) 
    {
        // 複製原始資料
        for (int i = 0; i < n; ++i)
            arr[i + 1] = orig[i];

        auto start = chrono::high_resolution_clock::now();
        quickSort(arr, 1, n);
        auto end = chrono::high_resolution_clock::now();
        totalTime += chrono::duration_cast<chrono::microseconds>(end - start).count();
    }

    printMemoryUsage();               // 排序後（尚未釋放記憶體)的記憶體使用量
    cout << "Average execution time: " << (double(totalTime) / REPS) << " μs" << endl;

    delete[] arr;
    printMemoryUsage();               // 釋放後的記憶體使用量
}


// Worst Case：生成已升冪 [1,2,…,n]，排序一次（對 pivot=a[left] 即為 worst case）
void worstCase(int n) {
    cout << "===== Worst Case ===== n=" << n << endl;

    printMemoryUsage();               // 排序前的記憶體使用量
    int* arr = new int[n + 2];        // a[0..n+1]

    for (int i = 1; i <= n; ++i)
        arr[i] = i;

    auto start = chrono::high_resolution_clock::now();
    quickSort(arr, 1, n);
    auto end = chrono::high_resolution_clock::now();

    printMemoryUsage();               // 排序後（尚未釋放）排序後（尚未釋放記憶體)的記憶體使用量
    cout << "Execution time: "
        << chrono::duration_cast<chrono::microseconds>(end - start).count()
        << " μs" << endl;

    delete[] arr;
    printMemoryUsage();               // 釋放後的記憶體使用量
}


int main() 
{
    int sizes[] = { 500,1000,2000,3000,4000,5000 };
    cout << "資料筆數: \n ";
    for (int i = 0; i < 6; ++i)
        cout << (i + 1) << "." << sizes[i] << "  ";
    cout << "\n請選擇(1~6):";
    int c;
    cin >> c;
    if (c < 1 || c > 6) 
    {
        cerr << "輸入無效,請輸入1~6" << endl;
        return 1;
    }
    int n = sizes[c - 1]; //根據輸入的選項，決定接下來要排序的n筆資料筆數

    cout << "選(1 = Average Case, 2 = Worst Case): ";
    int m;
    cin >> m;
    if (m == 1)       averageCase(n);
    else if (m == 2)  worstCase(n);
    else 
    {
        cerr << "輸入錯誤,請輸入1或2決定模式" << endl;
        return 1;
    }

    system("pause");
    return 0;
}
