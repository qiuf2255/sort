# 常见排序 · C++17

优先考虑编译兼容性，使用 C++17 的结构化绑定、泛型 lambda、类型推导以及移动语义，无第三方依赖。

## 项目解决什么问题

本项目把常见排序算法整理为统一接口的 C++17 头文件，方便学习算法实现、比较复杂度，并在自己的程序中对数组数据排序。它提供可直接运行的演示和正确性验证，减少从零编写示例与处理边界情况的工作。

项目优先考虑编译兼容性，不要求 C++20/23。本机已使用 Visual Studio 2019 编译并运行通过。

## 主要功能

- 实现冒泡、选择、插入、希尔、归并、快速、堆排序和计数排序，共八种算法。
- 七种比较排序支持 `std::vector<T>` 和自定义比较器，可用于整数、字符串或自定义记录的升降序排序。
- 支持空数组、重复值和负数；冒泡、插入与归并排序保持相等元素的原始顺序。
- 计数排序支持 32 位有符号整数，并提供计数桶数量上限，限制值域带来的内存开销。
- 自带示例、固定种子的随机测试和边界测试，可直接运行或通过 CTest 执行。

## 安装方法

### 环境要求

- 支持 C++17 的 C++ 编译器，例如 Visual Studio 2019 的 MSVC，或支持该标准的 GCC / Clang。
- 可选：CMake 3.20 或更高版本，仅使用 CMake 构建时需要。
- 无第三方库依赖，无需安装额外的软件包。

将项目源码下载或复制到本地，在项目根目录（包含 `main.cpp` 和 `sorts.hpp` 的目录）打开终端。Windows 使用 Visual Studio Installer 安装“使用 C++ 的桌面开发”工作负载，并在对应的开发者终端执行 MSVC 命令。

### 直接编译

GCC（Linux / macOS 终端）：

```sh
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic main.cpp -o sort_demo
```

使用 Clang 时，将 `g++` 替换为 `clang++`。Windows 上使用 GCC 时可将输出名称改为 `sort_demo.exe`。

MSVC（Visual Studio 的 Developer PowerShell / 开发者命令提示符）：

```powershell
cl /nologo /std:c++17 /EHsc /W4 /permissive- /utf-8 main.cpp /Fe:sort_demo.exe
```

### 使用 CMake 构建

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

如果只想在现有项目中使用算法，将 `sorts.hpp` 复制到项目的头文件目录，启用 C++17 并包含该头文件即可，无需单独编译或链接排序库。

## 使用方法

### 运行演示和测试

直接编译后，在 Linux / macOS 运行：

```sh
./sort_demo
```

在 Windows PowerShell 运行：

```powershell
.\sort_demo.exe
```

使用 CMake 时，Visual Studio 生成器通常将程序放在 `build/Release/sort_demo.exe`，单配置生成器通常放在 `build/sort_demo`（Windows 为 `.exe`）。也可以统一执行测试：

```sh
ctest --test-dir build -C Release --output-on-failure
```

程序先验证空数组、单元素、有序、逆序、重复值、负数、整数极值、300 组固定种子随机输入、升降序比较器、稳定性和值域限制，再输出八种算法的结果。成功返回退出码 `0`；验证失败时输出 `FAILED: ...` 并返回 `1`。

当前演示使用 `main.cpp` 中写好的数据，不读取标准输入，也不提供命令行参数。要更换数据，可修改演示循环中的 `Values values{...}` 后重新编译，或在自己的程序中调用算法。

### 在代码中调用

包含 `sorts.hpp` 即可使用。比较排序支持可复制、可移动的 `std::vector<T>` 元素，以及满足严格弱序的比较器，默认升序。

```cpp
#include "sorts.hpp"
#include <string>

int main() {
    std::vector<int> data{5, 2, 9, 1, 5};
    sorting::quick_sort(data);                     // 升序
    sorting::merge_sort(data, std::greater<>{});    // 降序

    std::vector<std::string> words{"pear", "apple", "banana"};
    sorting::heap_sort(words);
}
```

所有排序函数直接修改传入的 vector，返回类型为 `void`。自定义记录可通过 lambda 指定排序字段，例如：

```cpp
struct Student {
    std::string name;
    int score;
};

std::vector<Student> students{{"Alice", 90}, {"Bob", 75}, {"Carol", 90}};
sorting::merge_sort(students, [](const Student& a, const Student& b) {
    return a.score > b.score;
});
// 按成绩降序：Alice 90、Carol 90、Bob 75；同分记录保持原始顺序。
```

计数排序仅接受 `std::vector<std::int32_t>`，支持负数，按升序排列。默认最多分配 1,000,000 个计数桶；值域过大抛出 `std::length_error`，可使用第二个参数调整上限。该版本直接回填整数，不提供携带附属记录的稳定计数排序。

## 算法对比

| 算法 | 平均时间 | 最坏时间 | 辅助空间 | 稳定 |
|---|---|---|---|---|
| 冒泡 bubble_sort | O(n²) | O(n²) | O(1) | 是 |
| 选择 selection_sort | O(n²) | O(n²) | O(1) | 否 |
| 插入 insertion_sort | O(n²) | O(n²) | O(1) | 是 |
| 希尔 shell_sort | 取决于增量序列 | O(n²) | O(1) | 否 |
| 归并 merge_sort | O(n log n) | O(n log n) | O(n) | 是 |
| 快速 quick_sort | O(n log n) | O(n²) | O(log n) 栈 | 否 |
| 堆 heap_sort | O(n log n) | O(n log n) | O(1) | 否 |
| 计数 counting_sort | O(n + k) | O(n + k) | O(k) | 仅整数回填 |

`k` 为最大值减最小值加一。希尔排序使用逐次减半的间隔。快排使用中间元素为基准的三路划分，并只递归较短区间，限制递归栈深度；它仍可能遇到 O(n²) 的输入。已排序输入的冒泡和插入排序为 O(n)。复杂度按元素比较、复制和移动为常数时间计算。

## 输入输出示例

### 示例一：运行项目自带演示

输入数据（程序内置，无需在终端输入）：

```text
38 -4 27 0 12 27 9
```

终端输出（省略行尾空格）：

```text
All sorting tests passed.

bubble: -4 0 9 12 27 27 38
selection: -4 0 9 12 27 27 38
insertion: -4 0 9 12 27 27 38
shell: -4 0 9 12 27 27 38
merge: -4 0 9 12 27 27 38
quick: -4 0 9 12 27 27 38
heap: -4 0 9 12 27 27 38
counting: -4 0 9 12 27 27 38
```

### 示例二：自定义降序排序

下面是独立的完整示例，可保存为 `example.cpp`，与 `sorts.hpp` 放在同一目录，并将上述编译命令中的 `main.cpp` 替换为 `example.cpp`：

```cpp
#include "sorts.hpp"
#include <iostream>

int main() {
    std::vector<int> values{5, 2, 9, 1, 5};
    sorting::quick_sort(values, std::greater<>{});
    for (const auto value : values) std::cout << value << ' ';
    std::cout << '\n';
}
```

输出：

```text
9 5 5 2 1
```

这些实现便于学习算法；实际业务代码通常直接使用标准库的 `std::sort`，有稳定性需求时使用 `std::stable_sort`。
