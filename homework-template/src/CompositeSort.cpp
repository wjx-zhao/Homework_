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

// QuickSort
void quickSort(int* a, int left, int right) 
{
    if (left < right) 
    {
        int pivot = a[left];
        int i = left, j = right + 1;
        a[0] = pivot;           // 暫存 pivot
        a[right + 1] = pivot;   // 分割

        do 
        {
            do { ++i; } while (a[i] < pivot);
            do { --j; } while (a[j] > pivot);
            if (i < j) swap(a[i], a[j]);
        } while (i < j);

        swap(a[left], a[j]);
        quickSort(a, left, j - 1);
        quickSort(a, j + 1, right);
    }
}

// MergeSort
template<class T>
void Merge(T* initList, T* mergedList, const int l, const int m, const int n) 
{
    int i1 = l, i2 = m + 1, iResult = l;
    for (; i1 <= m && i2 <= n; ++iResult) 
    {
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

void mergeSort(int* a, int n) 
{
    int* tmp = new int[n + 1];
    mergeSortRec(a, tmp, 1, n);
    delete[] tmp;
}

// HeapSort
template<class T>
void Adjust(T* a, const int root, const int n) 
{
    T e = a[root];
    int j;
    for (j = 2 * root; j <= n; j *= 2) 
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
    for (int i = n / 2; i >= 1; --i)
        Adjust(a, i, n);
    for (int i = n - 1; i >= 1; --i) 
    {
        swap(a[1], a[i + 1]);
        Adjust(a, 1, i);
    }
}

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
    string line;
    getline(fin, line);
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

// 生成一組在多次隨機樣本中，執行 MergeSort 最慢的輸入
vector<int> genMergeWorst(int n) 
{
    long long maxT = -1;   // 記錄目前為止測得的最大排序耗時（微秒）
    vector<int> worst;   // 保存對應於 maxT 的「最慢」那組隨機資料
    int* tmp = new int[n + 2];

    // 保存對於 maxT 的「最慢」那組隨機資料
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

// 生成一組在多次隨機樣本中，執行 HeapSort 最慢的輸入
vector<int> genHeapWorst(int n) 
{
    long long maxT = -1;   // 記錄目前為止測得的最大排序耗時（微秒）
    vector<int> worst;   // 保存對應於 maxT 的「最慢」那組隨機資料
    int* buf = new int[n + 2];

    // 保存對於 maxT 的「最慢」那組隨機資料
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

// 測量傳入排序後在平均情況下的執行時間
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

long long measureWorst(int n,function<void(int*, int)> sorter,function<vector<int>(int)> genWorst) 
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


void compositeAverageCase(int n) 
{
    cout << "=== Composite Average Case (n=" << n << ") ===\n";
    double tIns = measureAvg(n, insertionSort);
    double tQck = measureAvg(n, [](int* a, int nn) { quickSort(a, 1, nn); });
    double tMrg = measureAvg(n, mergeSort);
    double tHep = measureAvg(n, heapSort<int>);
    double best = tIns; string name = "Insertion";
    if (tQck < best) { best = tQck; name = "Quick"; }
    if (tMrg < best) { best = tMrg; name = "Merge"; }
    if (tHep < best) { best = tHep; name = "Heap"; }
    cout << "最快算法：" << name << " Sort，平均 " << best << " μs\n\n";
}

void compositeWorstCase(int n) 
{
    cout << "=== Composite Worst Case (n=" << n << ") ===\n";
    long long tIns = measureWorst(n, insertionSort,[](int nn) 
    {
            vector<int> v(nn);
            for (int i = 0; i < nn; ++i) v[i] = nn - i;
            return v;
    });
    
    long long tQck = measureWorst(n,[](int* a, int nn) { quickSort(a, 1, nn); },[](int nn) 
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
    for (int i = 0; i < 6; ++i) cout << (i + 1) << ". " << sizes[i] << "\n";
    cout << "請選擇(1~6): ";
    int c; cin >> c;
    if (c < 1 || c > 6) return 1;
    int n = sizes[c - 1];

    cout << "選(1=Average Case, 2=Worst Case): ";
    int m; cin >> m;
    if (m == 1) compositeAverageCase(n);
    else         compositeWorstCase(n);

    system("pause");
    return 0;
}
