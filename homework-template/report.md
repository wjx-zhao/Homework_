# 41243201 
# 41243203

作業一
空行
---

## **Insertion Sort**

## 解題說明

本程式以「哨兵法插入排序」（Sentinel Insertion Sort）為核心，搭配檔案讀取與記憶體使用量偵測，實現以下兩種測試模式：
Average Case：從 <n>.txt 檔案讀入一列長度為 n 的整數，重複排序多次取平均時間。
Worst Case：在程式中自行生成反序排列 [n, n−1, …, 1]，並測量單次排序時間。
同時，於每個排序前、後（含釋放記憶體前）呼叫 Windows API 讀取並輸出目前進程的記憶體使用狀況。

### 解題策略

1.插入排序主體
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
```
```cpp
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

## 計時方式探討說明
衡量排序耗時用的是 C++11 的 <chrono>，程式碼出現在 averageCase 和 worstCase 中
```cpp
auto start = chrono::high_resolution_clock::now();
insertionSort(arr, n);
auto end   = chrono::high_resolution_clock::now();
long long dt = chrono::duration_cast<chrono::microseconds>(end - start).count();

```
1. 時鐘選擇high_resolution_clock
在大多數實作（如 MSVC）上等同於 steady_clock，底層呼叫 Windows 的 QueryPerformanceCounter，提供納秒或微秒級解析度，且不會受系統時間調整影響（monotonic）。
duration_cast<chrono::microseconds>
把時間差轉為微秒（μs）為單位。由於單次排序可能很快（尤其是小 n），微秒精度足以反映耗時。

2. 精度與誤差解析度
Windows 下 high_resolution_clock 解析度通常在 100 ns – 1 μs 之間（視硬體與驅動而定），當實際耗時低於解析度時，dt 可能為 0。
重複測試以減少誤差，程式將同一筆輸入 重複排序 2500 次，再以 totalTime/REPS 取平均。

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

### 圖表
![Insertion Sort](https://raw.githubusercontent.com/Lin-3203/image/main/Insertion%20Sort.png)


### 編譯與執行指令

```shell
$ g++ -std=c++17 -O2 sentinel_sort.cpp -o sentinel_sort.exe -lpsapi
$ sentinel_sort.exe

```

### 結論

1.功能正確性
程式能夠正確讀取檔案並執行 Average Case（重複多次並取平均）及 Worst Case（反序一次）兩種測試模式，排序結果均符合預期。

2.時間複雜度驗證
實測結果隨著資料規模由 500 → 5000 增大，執行時間呈現近似二次函數的增長趨勢，與插入排序 $O(n^2)$ 的理論複雜度相符。

3.記憶體使用情況
動態配置的陣列大小隨 $n$ 緩步增加，排序前後與釋放後的 Working Set、Pagefile Usage 變化量有限，顯示程式在處理完畢後能正常回收記憶體，無明顯記憶體洩漏。

4.適用範圍
對於資料筆數在數千筆以內的中小型應用，哨兵法插入排序結合本程式架構，既簡潔又易於測量，適合作為教學示例或簡易需求。
當資料規模超過 10K 以上，建議改採 $O(n\log n)$ 的排序演算法（如 Quick Sort、Heap Sort 或 Merge Sort），以獲得更佳效能。

## 申論及開發報告

### 選擇插入排序的原因

1.演算法簡潔易懂
插入排序的核心邏輯與數學公式 $\Sigma(n)=n+\Sigma(n-1)$ 類似，能迅速理解「將新元素插入已排序子序列」的概念。

2.實作與除錯成本低
完全以原地（in-place）方式運作，不需額外大型資料結構。
結構扁平、分支少，方便在邏輯中插入計時與記憶體量測程式碼。

3.適用於中小規模或近乎已排序資料
對於筆數在數千筆以內的小型資料集，插入排序在常數因子上表現不錯。
資料若已部分排序（或幾乎有序），插入排序的效能可逼近 $O(n)$，非常高效。

4.穩定排序
插入排序會保留相等鍵值元素的原始相對順序，適合後續需要保持穩定性的應用場景。

5.可深度優化的邊界檢查
本範例採用哨兵（sentinel）技巧，將待插入值暫存於 a[0]，免去每次迴圈都要檢查 j > 0，不僅簡化程式，也稍微提升了迴圈效率。

-------------------------------------------------------------------------------------------------------------------------------------

## **Quick Sort**

## 解題說明

本程式是一個快速排序（Quick Sort），並結合檔案讀取與記憶體使用量量測，完成以下兩種測試模式：

1.Average Case：從 <n>.txt 讀入一列長度為 n 的整數，重複 REPS 次排序並計算平均耗時。
2.Worst Case：生成已排序升冪資料（對於選取 pivot = a[left] 而言即為最壞情況），執行一次排序並測量耗時。

### 解題策略

1.遞迴分治
對 left..j-1 與 j+1..right 子陣列遞迴呼叫 quickSort，直至子陣列長度≤1。

2.Average Case 測試
開啟 <n>.txt，讀取單行 n 個整數至 orig 向量。
重複 REPS 次：將 orig 複製至動態陣列 arr[1..n]，呼叫 quickSort(arr,1,n)，並累計單次耗時。
輸出平均耗時與各階段記憶體使用量（排序前、排序後、釋放後）。

3.Worst Case 測試
動態配置 arr[1..n] 為升冪序列 [1,2,…,n]。
單次呼叫 quickSort(arr,1,n)，測量並輸出耗時及記憶體使用量。

4.記憶體量測
透過 Windows PSAPI (GetProcessMemoryInfo) 取得並輸出「Working Set Size」、「Peak Working Set Size」、「Pagefile Usage」，分別在關鍵階段呼叫。

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
```
```cpp
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


```
```cpp
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

```
```cpp

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
```
```cpp
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
```

## 效能分析

1.時間複雜度
Average Case：期望為 $O(n\log n)$，但受 REPS 迴圈影響實測需乘以常數。
Worst Case：對於選擇首元素為 pivot，升冪或降冪輸入會觸發 $O(n^2)$。

2.空間複雜度
平均遞迴深度為 $O(\log n)$，最壞情況下遞迴深度為 $O(n)$。
除遞迴堆疊外，額外使用 $O(n)$ 動態陣列；無其他大型額外空間。

## 測試與驗證

### 測試案例

| 測試案例 | Average Case(μs)| Worst Sort(μs) |
|-------- |-----------------|----------------|
| 500     |      17.4244    | 364            |
| 1000    |      44.8904    | 897            |
| 2000    |      173.566    | 3842           |
| 3000    |      350.953    | 10034          |
| 4000    |      449.1      | 11241          |
| 5000    |      578.788    | 13795          |

### 圖表
![Quick Sort](https://raw.githubusercontent.com/Lin-3203/image/main/Quick%20Sort.png)


### 編譯與執行指令

```shell
$ g++ -std=c++17 -O2 quick_sort.cpp -o quick_sort.exe -lpsapi
$ quick_sort.exe

```

### 結論

1.功能正確
程式能順利執行 Quick Sort 的 Average Case（重複多次取平均）與 Worst Case（升冪輸入最壞情況）測試，並正確輸出時間與記憶體使用量。

2.時間複雜度驗證
Average Case：實測結果顯示平均耗時成長趨勢近似 $O(n\log n)$。
Worst Case：在已排序升冪輸入下，耗時呈現近似 $O(n^2)$，符合理論預期。

3.記憶體使用狀況
動態陣列在排序前後與釋放後的 Working Set、Pagefile Usage 變化合理，無明顯記憶體洩漏現象。

4.遞迴深度與穩定性
固定選擇首元素為 pivot 會在最壞情況下導致遞迴深度退化至 $O(n)$，容易觸發 Stack Overflow。

## 申論及開發報告

### 選擇快速排序的原因

1.平均效能卓越
Quick Sort 在隨機或平均情況下具有 $O(n\log n)$ 的時間複雜度，對中大型資料集的排序效能遠優於 $O(n^2)$ 的演算法。

2.原地（In-Place）排序
僅需常數級額外空間（遞迴堆疊除外），不會像 Merge Sort 那樣需要額外的暫存陣列，節省記憶體開銷。

3.分治結構易於優化
可以透過尾遞迴優化、隨機化 pivot、三數取中等技術，控制最壞情況遞迴深度並進一步提升穩定性與實務效能。

4.實作簡潔、教學示範性強
Partition 的原理直觀、程式碼扁平；結合哨兵技巧後，更能示範邊界檢查優化的常見做法。

5.高度可擴充
在現有架構下，只需替換 quickSort 核心即能比較其他排序演算法；也可將同一份測量程式用於 Heap Sort、Merge Sort 等，實現模組化效能評測。

6.優良的記憶體與快取表現
原地交換、局部性原則（Locality of Reference）使得 Quick Sort 在現代 CPU 快取架構下擁有良好效能，常被用於標準庫實作。

-------------------------------------------------------------------------------------------------------------------------------------

## **Merge Sort**

## 解題說明

1.Average Case 測試
從 <n>.txt 單行讀入長度為 n 的整數資料，重複 REPS 次排序並計算平均耗時。

2.Worst Case 測試
隨機產生多組長度為 n 的向量，選出排序時間最長的那一組，對其執行一次排序並測量最壞情況耗時。

3.記憶體監控
在排序前、排序後（尚未釋放）與釋放後，呼叫 Windows PSAPI API 讀取並輸出「Working Set Size」、「Peak Working Set Size」與「Pagefile Usage」，以觀察程式的記憶體使用與回收情況。

### 解題策略

1.分治遞迴 (Divide and Conquer)
將陣列區間 [l,r] 分成 [l,m] 與 [m+1,r] 兩半，遞迴排序後再合併。

2.Merge 函式
Merge(initList, mergedList, l, m, r)：利用兩個指標同時掃描已排序的左右子陣列，將較小值依序放入 mergedList[l..r]，最後複製剩餘元素。

3.暫存與複製
每層遞迴配置大小為 n+1 的暫存陣列 tmp，合併完成後再以 std::copy 將 tmp[l..r] 複製回原陣列 a[l..r]，確保穩定性與正確性。

4.平均情況測試
重複讀取並排序同一筆資料 REPS 次以平滑背景干擾，計算所有次數累加的平均值。

5.最壞情況樣本搜尋
genRandom(n) 隨機產生一筆向量，重複多次試驗後保留排序耗時最長者作為最壞案例，再對其進行一次排序測量。

6.記憶體使用量量測
利用 GetProcessMemoryInfo 於關鍵階段輸出記憶體指標，觀察動態配置與釋放是否正常，並作為性能報告的一部分。

## 程式實作

以下為主要程式碼：

```cpp
這是使用到的標頭檔

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <Windows.h>
#include <Psapi.h>
#include <random> 

using namespace std;

```
```cpp
// 生成隨機數生成器
//這行程式碼的作用是定義並初始化一個全域（或函式內）唯一的隨機數產生器（RNG）
static mt19937_64 rng(std::random_device{}());
```
```cpp
void printMemoryUsage() {
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    cout << "----------------------------------------------------------\n";
    cout << "Working Set Size: " << memInfo.WorkingSetSize / 1024 << " KB\n";
    cout << "Peak Working Set Size: " << memInfo.PeakWorkingSetSize / 1024 << " KB\n";
    cout << "Pagefile Usage: " << memInfo.PagefileUsage / 1024 << " KB\n";
    cout << "----------------------------------------------------------\n";
}
```
```cpp
// 將 initList[l..m] 與 initList[m+1..n] 的兩段已排序子陣列
// 合併到 mergedList[l..n]。需要額外 O(n) 空間。
template<class T>
void Merge(T* initList, T* mergedList, const int l, const int m, const int n)
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

```

```cpp
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
```
```cpp
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

```
```cpp

// Worst Case：先找出「最慢」的那組隨機向量，再排序一次
vector<int> genRandom(int n)
{
    static mt19937_64 rng(random_device{}());
    uniform_int_distribution<int> dist(0, 1000000);
    vector<int> v(n);
    for (int& x : v) x = dist(rng);
    return v;
}

//多次試驗找最慢樣本
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
    auto sample = genWorstCaseSample(n);//最壞樣本的最終測量
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
```
```cpp
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
```

## 效能分析

1.時間複雜度
平均：Merge Sort 每次都將陣列一分為二再合併，無論輸入分佈為何，皆需執行 $\Theta(n\log n)$ 次比較與移動，因此平均時間複雜度為 $O(n\log n)$。
最壞：同理，最壞情況（完全反序、已排序等）亦為 $O(n\log n)$。

2.空間複雜度
額外空間：每次呼叫 mergeSort 時會配置大小為 $n+1$ 的暫存陣列 tmp，故空間複雜度為 $O(n)$。
遞迴堆疊：分治遞迴深度為 $\log_2 n$，故堆疊空間為 $O(\log n)$。

## 計時方式探討說明
衡量排序耗時用的是 C++11 的 <chrono>，程式碼出現在 averageCase 和 worstCase 中
```cpp
auto t0 = chrono::high_resolution_clock::now();
mergeSort(arr, n);
auto t1 = chrono::high_resolution_clock::now();
long long dt = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();

```
1. 時鐘類型與選擇high_resolution_clock[解析度最高]，實作上通常等同於 steady_clock（保證單調、不受系統時間調整影響）或直接對應到最精細的硬體計時器。
   其底層在 Windows 上會呼叫 QueryPerformanceCounter，解析度可達 數百奈秒 至 微秒。
steady_clock
明確保證「單調遞增」（monotonic），也是測量時間間隔的推薦選擇，不會因系統時鐘校正導致向前或向後跳動。
單位與精度
2.解析度與單位(microseconds)
使用 duration_cast<chrono::microseconds> 來把時間差轉成微秒（μs），適合大多數排序測量（通常耗時在幾十到幾萬微秒之間）。

## 測試與驗證

### 測試案例

| 測試案例 | Average Case(μs)| Worst Sort(μs) |
|-------- |-----------------|----------------|
| 500     |      17.4244    | 364            |
| 1000    |      44.8904    | 897            |
| 2000    |      173.566    | 3842           |
| 3000    |      350.953    | 10034          |
| 4000    |      449.1      | 11241          |
| 5000    |      578.788    | 13795          |

### 圖表
![Merge Sort](https://raw.githubusercontent.com/Lin-3203/image/main/Merge%20Sort.png)


### 編譯與執行指令

```shell
$ g++ -std=c++17 -O2 merge_sort.cpp -o merge_sort.exe -lpsapi
$ merge_sort.exe

```

### 結論

1.正確性：程式能穩定完成 Merge Sort 的 Average Case（重複取平均）和 Worst Case（最慢樣本）測試，並正確輸出耗時與記憶體使用量。

2.效能：實測證明無論輸入分佈為何，時間複雜度皆維持 $O(n\log n)$，符合 Merge Sort 理論。

3.記憶體管理：動態配置的暫存陣列與遞迴堆疊皆於排序結束後正確釋放，無記憶體洩漏。

4.適用範圍：對中大型資料（數千到數十萬筆）皆有穩定且可預期的效能，適合作為關鍵排序需求或教學示範。

## 申論及開發報告

### 選擇Merge Sort的原因

選擇 Merge Sort 的原因

1.穩定性：能保留相等元素的原始順序，適用於需要穩定排序的場景。
(1)保證表現：無論輸入分佈如何，皆能提供 $O(n\log n)$ 的最壞與平均效能。
(2)可並行化：左右兩半可獨立排序，具備良好擴展性。

2.程式設計架構
模組化：
mergeSort／mergeSortRec 處理遞迴分割與合併。
Merge 函式專責合併兩段已排序子陣列。

3.效能量測：
printMemoryUsage 結合 Windows PSAPI 讀取「Working Set」、「Peak Working Set」、「Pagefile Usage」。
<chrono> 進行高解析度微秒級時間測量。

4.測試驅動：
Average Case 透過重複多次取平均降低系統抖動影響。
Worst Case 以隨機試驗選出最慢樣本，貼近真實最差表現。

---------------------------------------------------------------------------------------------------------------------------------------------------------

## **Heap Sort**

## 解題說明

1.Average Case：從檔案 <n>.txt 讀入長度為 n 的整數序列。
重複 REPS 次執行 Heap Sort，累計並計算平均排序時間。

2.Worst Case：隨機產生多組長度為 n 的整數向量，找出排序耗時最長者作為最壞樣本。
針對該最壞樣本執行一次 Heap Sort，測量最壞情況下的排序時間。

3.記憶體量測：
在排序前、排序後（未釋放）及釋放後，呼叫 Windows PSAPI 的 GetProcessMemoryInfo，輸出「Working Set Size」、「Peak Working Set Size」與「Pagefile Usage」，以觀察記憶體配置與回收情形。

### 解題策略

1.建堆 (Heapify)從最後一個非葉節點 ⌊n/2⌋ 開始，呼叫 Adjust(a, i, n) 將以節點 i 為根的子樹調整為 max‐heap。
完成後，整個陣列 a[1..n] 滿足每個父節點均不小於子節點的性質。

2.排序階段 (Heap Sort 主流程)反覆執行以下步驟，直到堆大小縮減為 1：
將堆頂（最大值）與未排序區的最後一個元素交換。
將剩餘 i 個元素重新調整為 max‐heap（呼叫 Adjust(a, 1, i)）。
交換與重建堆的過程，使得最大元素依序移至陣列尾端，完成升冪排序。

3.效能量測
Average Case：讀檔後，多次複製原始資料到動態陣列，呼叫 heapSort 並累計微秒級耗時，最後計算平均值。
Worst Case：透過 genRandom 與 genWorstCaseSample 隨機試驗，選出耗時最久的輸入，再對該輸入排序並測時。

## 程式實作

以下為主要程式碼：

```cpp
這是使用到的標頭檔

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <Windows.h>
#include <Psapi.h>
#include <random>

using namespace std;

```

```cpp
void printMemoryUsage() {
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    cout << "----------------------------------------------------------\n";
    cout << "Working Set Size:      " << memInfo.WorkingSetSize / 1024 << " KB\n";
    cout << "Peak Working Set Size: " << memInfo.PeakWorkingSetSize / 1024 << " KB\n";
    cout << "Pagefile Usage:        " << memInfo.PagefileUsage / 1024 << " KB\n";
    cout << "----------------------------------------------------------\n";
}
```
```cpp
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

```

```cpp
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
```
```cpp
// 使用 std::random_device 產生非決定性種子，並以此初始化 64 位元的 Mersenne Twister 引擎
static mt19937_64 rng(random_device{}());

// genRandom：產生長度為 n 的隨機整數向量，每個元素分佈於 [0, 1,000,000]
vector<int> genRandom(int n)
{
    // 定義均勻整數分佈範圍：最小 0，最大 1,000,000
    uniform_int_distribution<int> dist(0, 1000000);

    // 建立長度為 n 的向量 v
    vector<int> v(n);

    // 透過 dist(rng) 產生隨機值並填入向量
    for (int& x : v) {
        x = dist(rng);
    }

    // 回傳包含 n 個隨機整數的向量
    return v;
}
```
```cpp
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
```
```cpp
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

```
```cpp

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
```
```cpp
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
```

## 效能分析

1.時間複雜度:建堆階段：從最後一個非葉節點 ⌊n/2⌋ 開始往前呼叫 Adjust，每次調整最高需 O(logn)，但所有節點合計成本為 O(n)。
排序階段：將堆頂交換至尾端並呼叫 Adjust(a,1,i) 共執行 n−1 次，每次調整最壞需 O(logn)，故排序階段為 O(nlogn)。
整體：O(n)+O(nlogn)=O(nlogn)。
平均與最壞情況：Heap Sort 的時間複雜度在任何輸入分佈下均為 O(nlogn)。

2.空間複雜度

輔助空間：原地（in-place）演算法，僅使用常數級額外空間（用於暫存變數與交換），故為 O(1)。
輸入與測試結構：程式為了測試方便額外配置了大小為 n+1 的動態陣列與大小為 n 的 vector<int>，共佔 O(n) 輸入空間，但這非演算法本身之必要。

3.記憶體使用量觀察

Working Set Size 隨 n 緩增，大致等於 4 bytes×n（整數陣列）加上程式常數開銷。
Peak Working Set 較 Working Set 稍高，因為多次 new[]/delete[] 及 vector 內部重配置會暫時佔用更多記憶體。
Pagefile Usage 變化不大，表示記憶體釋放後能及時回收，無明顯洩漏。

## 測試與驗證

### 測試案例

| 測試案例 | Average Case(μs)| Worst Sort(μs) |
|-------- |-----------------|----------------|
| 500     |      29.6936    | 87             |
| 1000    |      84.5648    | 205            |
| 2000    |      279.611    | 2037           |
| 3000    |      463.245    | 867            |
| 4000    |      665.06     | 1133           |
| 5000    |      854.194    | 1482           |


### 圖表
![Heap Sort](https://raw.githubusercontent.com/Lin-3203/image/main/Heap%20Sort.png)

### 測試資料產生細節
1.隨機數產生器:一次性以 random_device 取得非決定性種子，初始化 64 位元 Mersenne Twister 引擎，保證每次程式執行都能產生不同的亂數序列。
```cpp
static mt19937_64 rng(random_device{}());
```
2.產生單一測試樣本:對長度為 n 的向量，每個元素在 [0,1 000 000] 區間均勻分佈。
```cpp
vector<int> genRandom(int n) {
    uniform_int_distribution<int> dist(0, 1000000);
    vector<int> v(n);
    for (int& x : v) x = dist(rng);
    return v;
}
```
3.尋找最壞樣本
(1)trials = 20：預設會隨機生成 20 組長度為 n 的輸入。
(2)每次測試都將剛產生的向量複製到 buf，呼叫 heapSort 並用 <chrono> 計時。
(3)最後保留那組排序耗時最久的向量作為「最壞測試樣本」。
```cpp
vector<int> genWorstCaseSample(int n, int trials = 20) {
    long long maxT = -1;
    vector<int> worst;
    int* buf = new int[n+1];
    for (int t = 0; t < trials; ++t) {
        auto v = genRandom(n);
        // 複製到 buf[1..n]，排序並計時
        ...
        if (dt > maxT) { maxT = dt; worst = move(v); }
    }
    delete[] buf;
    return worst;
}
```
4.最壞情況測量
```cpp
auto sample = genWorstCaseSample(n);
```

### 編譯與執行指令

```shell
$ g++ -std=c++17 -O2 heap_sort.cpp -o heap_sort.exe -lpsapi
$ heap_sort.exe

```

### 結論

1.時間複雜度
無論輸入分佈為何，Heap Sort 的時間複雜度均為 O(nlogn)。實測結果隨著資料規模增大，排序時間曲線與 nlogn 理論趨勢吻合。

2.空間複雜度
原地排序演算法，僅需 O(1) 額外空間進行元素交換。
測試過程中的動態陣列與 vector 為量測需求所配置，不屬於演算法本身必需空間。

3.記憶體管理
printMemoryUsage 輸出顯示動態配置與釋放後的 Working Set、Pagefile Usage 變化合理，未見記憶體洩漏。

4.適用範圍
對於中大型資料集（數千至數萬筆），Heap Sort 可提供穩定且可預期的效能。

## 申論及開發報告

### 選擇Heap Sort的原因

選擇 Heap Sort 的原因
最壞情況可預期不同於 Quick Sort 可能退化至 O(n2)，Heap Sort 無論輸入分佈皆維持 O(nlogn)。

1.原地排序
只使用常數級額外空間（除測試所需之動態陣列），不需要像 Merge Sort 那樣額外分配暫存陣列。

2.實作清晰
Adjust 與 heapSort 函式邏輯扁平，可清楚演示「建堆」與「反覆取頂」兩大步驟。

---------------------------------------------------------------------------------------------------------------------------------------------------------
## **Composite Sort**

## 程式實作

```cpp
//使用到的標頭
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <random>
#include <functional>
#include <climits>
using namespace std;
```
```cpp
// InsertionSort
void insertionSort(int* a, int n)
{
    for (int i = 2; i <= n; ++i)
    {
        int key = a[i];
        a[0] = key;        // 哨兵
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
// QuickSortDet / QuickSortRand
static mt19937_64 qs_rng(random_device{}());

// 固定中點 pivot 分割 (Hoare)
int partition_det(int* a, int left, int right)
{
    int mid = left + (right - left) / 2;
    swap(a[left], a[mid]);
    int pivot = a[left], i = left, j = right + 1;
    a[0] = pivot; a[right + 1] = pivot;
    do {
        do { ++i; } while (a[i] < pivot);
        do { --j; } while (a[j] > pivot);
        if (i < j) swap(a[i], a[j]);
    } while (i < j);
    swap(a[left], a[j]);
    return j;
}

// 固定中點 pivot + 尾端遞迴優化
void quickSortDet(int* a, int left, int right)
{
    while (left < right)
    {
        int p = partition_det(a, left, right);
        if (p - left < right - p) {
            quickSortDet(a, left, p - 1);
            left = p + 1;
        }
        else
        {
            quickSortDet(a, p + 1, right);
            right = p - 1;
        }
    }
}

// 隨機 pivot 分割 (Hoare)
int partition_rand(int* a, int left, int right)
{
    uniform_int_distribution<int> dist(left, right);
    swap(a[left], a[dist(qs_rng)]);
    int pivot = a[left], i = left, j = right + 1;
    a[0] = pivot; a[right + 1] = pivot;
    do
    {
        do { ++i; } while (a[i] < pivot);
        do { --j; } while (a[j] > pivot);
        if (i < j) swap(a[i], a[j]);
    } while (i < j);
    swap(a[left], a[j]);
    return j;
}

// 隨機 pivot + 尾端遞迴優化
void quickSortRand(int* a, int left, int right)
{
    while (left < right)
    {
        int p = partition_rand(a, left, right);
        if (p - left < right - p)
        {
            quickSortRand(a, left, p - 1);
            left = p + 1;
        }
        else
        {
            quickSortRand(a, p + 1, right);
            right = p - 1;
        }
    }
}

```
```cpp
// MergeSort
template<class T>
void Merge(T* initList, T* mergedList, const int l, const int m, const int n)
{
    int i1 = l, i2 = m + 1, iResult = l;
    for (; i1 <= m && i2 <= n; ++iResult) {
        if (initList[i1] <= initList[i2]) mergedList[iResult] = initList[i1++];
        else                               mergedList[iResult] = initList[i2++];
    }
    copy(initList + i1, initList + m + 1, mergedList + iResult);
    iResult += (m - i1 + 1);
    copy(initList + i2, initList + n + 1, mergedList + iResult);
}

void mergeSortRec(int* a, int* tmp, int l, int r)
{
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSortRec(a, tmp, l, m);
    mergeSortRec(a, tmp, m + 1, r);
    Merge(a, tmp, l, m, r);
    copy(tmp + l, tmp + r + 1, a + l);
}

// 方便呼叫：只分配一次靜態暫存區
void mergeSort(int* a, int n)
{
    static int* tmpBuf = nullptr;
    static int  bufSize = 0;
    if (bufSize < n + 2)
    {
        delete[] tmpBuf;
        bufSize = n + 2;
        tmpBuf = new int[bufSize];
    }
    mergeSortRec(a, tmpBuf, 1, n);
}
```
```cpp
// HeapSort
template<class T>
void Adjust(T* a, const int root, const int n)
{
    T e = a[root];
    int j = 2 * root;
    for (; j <= n; j *= 2)
    {
        if (j < n && a[j] < a[j + 1]) ++j;
        if (e >= a[j]) break;
        a[j / 2] = a[j];
    }
    a[j / 2] = e;
}

template<class T>
void heapSort(T* a, const int n)
{
    for (int i = n / 2; i >= 1; --i) Adjust(a, i, n);
    for (int i = n - 1; i >= 1; --i)
    {
        swap(a[1], a[i + 1]);
        Adjust(a, 1, i);
    }
}
```
```cpp
// Read_AverageCase
vector<int> readAverageData(int n)
{
    string filename = to_string(n) + ".txt";
    ifstream fin(filename);
    if (!fin)
    {
        cerr << "無法打開 " << filename << "\n";
        return {};
    }
    string line; getline(fin, line);
    istringstream iss(line);
    vector<int> v(n);
    for (int i = 0; i < n; ++i) iss >> v[i];
    return v;
}

// 隨機數與最壞情境產生 
static mt19937_64 rng(random_device{}());

vector<int> genRandom(int n)
{
    uniform_int_distribution<int> dist(0, 1000000);
    vector<int> v(n);
    for (int& x : v) x = dist(rng);
    return v;
}

```
```cpp
// 生成 MergeSort 最慢輸入
vector<int> genMergeWorst(int n)
{
    long long maxT = -1;
    vector<int> worst;
    int* tmp = new int[n + 2];
    for (int t = 0; t < 20; ++t)
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

// 生成 HeapSort 最慢輸入
vector<int> genHeapWorst(int n)
{
    long long maxT = -1;
    vector<int> worst;
    int* buf = new int[n + 2];
    for (int t = 0; t < 20; ++t)
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

// 測量平均情況耗時
double measureAvg(int n, function<void(int*, int)> sorter)
{
    auto data = readAverageData(n);
    if ((int)data.size() != n) return 1e18;
    const int REPS = 2500;
    long long total = 0;
    int* buf = new int[n + 2];
    for (int rep = 0; rep < REPS; ++rep)
    {
        for (int i = 0; i < n; ++i) buf[i + 1] = data[i];
        auto t0 = chrono::high_resolution_clock::now();
        sorter(buf, n);
        auto t1 = chrono::high_resolution_clock::now();
        total += chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    }
    delete[] buf;
    return double(total) / REPS;
}

// 測量最壞情況耗時
long long measureWorst(int n, function<void(int*, int)> sorter, function<vector<int>(int)> genWorst)
{
    auto data = genWorst(n);
    int* buf = new int[n + 2];
    for (int i = 0; i < n; ++i) buf[i + 1] = data[i];
    auto t0 = chrono::high_resolution_clock::now();
    sorter(buf, n);
    auto t1 = chrono::high_resolution_clock::now();
    delete[] buf;
    return chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
}
```
```cpp
// Composite 平均情況
void compositeAverageCase(int n)
{
    cout << "=== Composite Average Case (n=" << n << ") ===\n";
    double tIns = measureAvg(n, insertionSort);
    double tQck = measureAvg(n, [](int* a, int nn) { quickSortDet(a, 1, nn); });
    double tMrg = measureAvg(n, mergeSort);
    double tHep = measureAvg(n, heapSort<int>);
    double best = tIns; string name = "Insertion";
    if (tQck < best) { best = tQck; name = "Quick"; }
    if (tMrg < best) { best = tMrg; name = "Merge"; }
    if (tHep < best) { best = tHep; name = "Heap"; }
    cout << "最快算法：" << name << " Sort，平均 " << best << " μs\n\n";
}

// Composite 最壞情況
void compositeWorstCase(int n)
{
    cout << "=== Composite Worst Case (n=" << n << ") ===\n";
    long long tIns = measureWorst(n, insertionSort, [](int nn)
        {
            vector<int> v(nn);
            for (int i = 0; i < nn; ++i) v[i] = nn - i;
            return v;
        });
    long long tQck = measureWorst(n, [](int* a, int nn) { quickSortRand(a, 1, nn); }, [](int nn)
        {
            vector<int> v(nn);
            for (int i = 0; i < nn; ++i) v[i] = i + 1;
            return v;
        });
    long long tMrg = measureWorst(n, mergeSort, genMergeWorst);
    long long tHep = measureWorst(n, heapSort<int>, genHeapWorst);
    long long best = tIns; string name = "Insertion";
    if (tQck < best) { best = tQck; name = "Quick"; }
    if (tMrg < best) { best = tMrg; name = "Merge"; }
    if (tHep < best) { best = tHep; name = "Heap"; }
    cout << "最快算法：" << name << " Sort，耗時 " << best << " μs\n\n";
}

int main()
{
    int sizes[] = { 500,1000,2000,3000,4000,5000 };
    cout << "資料筆數:\n";
    for (int i = 0; i < 6; ++i) cout << (i + 1) << ". " << sizes[i] << "   ";
    cout << "\n請選擇(1~6): ";
    int c; cin >> c;
    if (c < 1 || c > 6) return 1;
    int n = sizes[c - 1];

    cout << "選(1=Average Case, 2=Worst Case): ";
    int m; cin >> m;
    if (m == 1) compositeAverageCase(n);
    else if (m == 2) compositeWorstCase(n);
    else {
        cerr << "輸入錯誤,請輸入1或2決定模式\n";
        return 1;
    }

    system("pause");
    return 0;
}
```
Composite Sort優化
依照目前的程式下去比較有點不公平，會有單演算法一直是最快的  
如果要公平的比較  
1.Merge Sort的暫存陣列只配置一次
在main()開一個int*tmp = new int[max_n+2]，把它傳進mergeSortRec，避免反覆new/delete  
2.把QuickSort的亂數抽樣移到genWorstCase之外
只花WorstCase資料產生時用亂數，在測平均時改用固定pivot，減少呼叫亂數的花費  
3.少跑幾次平均測資
雖然2500次能更平均更穩定，但次數太多時，動態配置開銷比排序演算法成本還高
