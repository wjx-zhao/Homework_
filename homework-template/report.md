# 41243201、41243203

Insertion Sort

## 解題說明

本程式以「哨兵法插入排序」（Sentinel Insertion Sort）為核心，搭配檔案讀取與記憶體使用量偵測，實現以下兩種測試模式：
Average Case：從 <n>.txt 檔案讀入一列長度為 n 的整數，重複排序多次取平均時間。
Worst Case：在程式中自行生成反序排列 [n, n−1, …, 1]，並測量單次排序時間。
同時，於每個排序前、後（含釋放記憶體前）呼叫 Windows API 讀取並輸出目前進程的記憶體使用狀況。

### 解題策略

1. 插入排序主體
使用 a[0] 作為哨兵（Sentinel），將當前欲插入的元素暫存於 a[0]，以避免每次迴圈都要檢查邊界。
從第二個元素開始，將 key = a[i] 與前一個元素比較，若前一個元素較大則後移，直到找到適當位置插入 key。

2.Average Case
以 to_string(n) + ".txt" 作為檔名開檔，讀取單行 n 個整數。
重複 REPS 次：將原始資料複製至動態陣列 arr[1..n]，呼叫 insertionSort(arr, n)，並累加耗時。
最後計算並輸出平均執行時間。

3.Worst Case
動態配置陣列 arr[1..n]，並填入反序 [n, n−1, …, 1]。
單次呼叫 insertionSort(arr, n)，測量並輸出執行時間。

4.記憶體量測
使用 GetProcessMemoryInfo 取得並輸出「Working Set Size」、「Peak Working Set Size」以及「Pagefile Usage」。
在排序前、排序後（未釋放）和釋放記憶體後各呼叫一次，以觀察記憶體變化。

## 程式實作

以下為主要程式碼：

```cpp
這是使用到的標頭檔

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <chrono>
#include <Windows.h>
#include <Psapi.h>
using namespace std;
```
```cpp
//計算記憶體使用量(使用助教提供方式)
void printMemoryUsage() 
{
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    cout << "----------------------------------------------------------" << endl;
    cout << "Working Set Size: " << memInfo.WorkingSetSize / 1024 << " KB" << endl;
    cout << "Peak Working Set Size: " << memInfo.PeakWorkingSetSize / 1024 << " KB" << endl;
    cout << "Pagefile Usage: " << memInfo.PagefileUsage / 1024 << " KB" << endl;
    cout << "----------------------------------------------------------" << endl;
}
```
```cpp
// 哨兵法插入排序：對 a[1..n] 做排序，a[0] 作為哨兵
void insertionSort(int* a, int n) 
{
    for (int i = 2; i <= n; ++i) 
    {
        int key = a[i];
        a[0] = key;        // 設置哨兵
        int j = i - 1;
        while (a[j] > key) 
        {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}
```
```cpp
// Average Case：使用讀檔的方式(為了使用同一筆測資算出平均的準確度)
//讀取 "<n>.txt" n 個整數，重複排序 REPS 次並取平均
void averageCase(int n) 
{
    const int REPS = 2500;
    string filename = to_string(n) + ".txt";

    ifstream fin(filename);
    if (!fin) 
    {
        cerr << "無法打開 " << filename <<  endl;
        return;
    }
    string line;
    if (!getline(fin, line)) 
    {
        cerr << filename << " 讀取失敗" << endl;
        return;
    }
    fin.close();

    istringstream iss(line);
    vector<int> orig(n);
    for (int i = 0; i < n; ++i) 
    {
        if (!(iss >> orig[i])) 
        {
            cerr << filename << " 中第 " << (i + 1) << " 個數字解析失敗。" << endl;
            return;
        }
    }

    // 重複排序(2500次)取平均並計時
    long long totalTime = 0;

    cout << "===== Average Case ===== n = " << n << endl;

    printMemoryUsage();               // 排序前的記憶體使用量
    int* arr = new int[n + 1];        // a[0..n]

    for (int rep = 0; rep < REPS; ++rep) 
    {
        // 複製原始資料到 a[1..n]
        for (int j = 0; j < n; ++j)
            arr[j + 1] = orig[j];

        auto start = chrono::high_resolution_clock::now();
        insertionSort(arr, n);
        auto end = chrono::high_resolution_clock::now();
        totalTime += chrono::duration_cast<chrono::microseconds>(end - start).count();
    }

    printMemoryUsage();               // 排序後（尚未釋放記憶體)的記憶體使用量
    double avgTime = double(totalTime) / REPS;
    cout << "Average execution time: " << avgTime << " μs" << endl;

    delete[] arr;
    printMemoryUsage();               // 釋放後的記憶體使用量
}
```
```cpp

// Worst Case：生成反序 [n, n-1, ..., 1]，排序一次
void worstCase(int n) 
{
    cout << "===== Worst Case ===== n = " << n << " (反序排列)" << endl;

    printMemoryUsage();               // 排序前的記憶體使用量
    int* arr = new int[n + 1];
    for (int i = 1; i <= n; ++i)
        arr[i] = n - i + 1;

    auto start = chrono::high_resolution_clock::now();
    insertionSort(arr, n);
    auto end = chrono::high_resolution_clock::now();

    printMemoryUsage();               // 排序後（尚未釋放記憶體）的記憶體使用量
    cout << "Execution time: "
        << chrono::duration_cast<chrono::microseconds>(end - start).count()
        << " μs" << endl;

    delete[] arr;
    printMemoryUsage();               // 釋放後的記憶體使用量
}


int main() {
    int sizes[] = { 500,1000,2000,3000,4000,5000 };
    cout << "資料筆數: \n";
    for (int i = 0; i < 6; ++i)
        cout << (i + 1) << "." << sizes[i] << "  ";
    cout << "\n請選擇(1~6):";
    int c; cin >> c;
    if (c < 1 || c > 6) 
    {
        cerr << "輸入無效,請輸入1~6" << endl;
        return 1;
    }
    int n = sizes[c - 1]; //根據輸入的選項，決定接下來要排序的n筆資料筆數

    cout << "選(1 = Average Case, 2 = Worst Case): ";
    int m; cin >> m;
    if (m == 1)      averageCase(n);
    else if (m == 2) worstCase(n);
    else {
        cerr << "輸入錯誤,請輸入1或2決定模式" << endl;
        return 1;
    }

    system("pause");
    return 0;
}
```

## 效能分析

1.時間複雜度
Average Case：隨機資料下，勢必多次元素移動，平均約 $O(n^2)$。
Worst Case：反序排列下，每一輪都要將前 $i−1$ 個元素右移，時間複雜度為 $O(n^2)$。

2.空間複雜度
除了輸入本身使用的 $O(n)$，程式中動態配置 int[n+1] 也是 $O(n)$。
額外常數空間：哨兵、迴圈變數等，均為 $O(1)$。

## 測試與驗證

### 測試案例

| 測試案例 | Average Case(μs)| Worst Sort(μs) |
|-------- |-----------------|----------------|
| 500     |      143.928    | 372            |
| 1000    |      505.721    | 1626           |
| 2000    |      1932.18    | 6959           |
| 3000    |      6540.54    | 12289          |
| 4000    |      7542.73    | 14954          |
| 5000    |      19401.6    | 31262          |
### 編譯與執行指令

```shell
$ g++ -std=c++17 -O2 sentinel_sort.cpp -o sentinel_sort.exe -lpsapi
$ sentinel_sort.exe

```

### 結論

1. 程式能正確計算 $n$ 到 $1$ 的連加總和。  
2. 在 $n < 0$ 的情況下，程式會成功拋出異常，符合設計預期。  
3. 測試案例涵蓋了多種邊界情況（$n = 0$、$n = 1$、$n > 1$、$n < 0$），驗證程式的正確性。

## 申論及開發報告

### 選擇遞迴的原因

在本程式中，使用遞迴來計算連加總和的主要原因如下：

1. **程式邏輯簡單直觀**  
   遞迴的寫法能夠清楚表達「將問題拆解為更小的子問題」的核心概念。  
   例如，計算 $\Sigma(n)$ 的過程可分解為：  

   $$
   \Sigma(n) = n + \Sigma(n-1)
   $$

   當 $n$ 等於 1 或 0 時，直接返回結果，結束遞迴。

2. **易於理解與實現**  
   遞迴的程式碼更接近數學公式的表示方式，特別適合新手學習遞迴的基本概念。  
   以本程式為例：  

   ```cpp
   int sigma(int n) {
       if (n < 0)
           throw "n < 0";
       else if (n <= 1)
           return n;
       return n + sigma(n - 1);
   }
   ```

3. **遞迴的語意清楚**  
   在程式中，每次遞迴呼叫都代表一個「子問題的解」，而最終遞迴的返回結果會逐層相加，完成整體問題的求解。  
   這種設計簡化了邏輯，不需要額外變數來維護中間狀態。

透過遞迴實作 Sigma 計算，程式邏輯簡單且易於理解，特別適合展示遞迴的核心思想。然而，遞迴會因堆疊深度受到限制，當 $n$ 值過大時，應考慮使用迭代版本來避免 Stack Overflow 問題。
