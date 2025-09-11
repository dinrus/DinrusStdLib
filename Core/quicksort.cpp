#include <drx/Core/Core.h>
#ifdef USE_OMP
#include "omp.h"
#endif

/*!
* quickSort - параллельная реализация алгоритма быстрой сортировки
* @param a сортируемый массив
* @param n индекс последнего элемента массива (не размер массива!)

i = 0, 1, ..n-1
j = n-1, n-2, ..0
n - размер массива
pivot - опорный элемент

*/
template <class T>
void quickSort(T* a, i64k n) {
  i64 i = 0, j = n-1;
  T pivot = a[n / 2];
  do {
    while (a[i] < pivot) {
        i++;
    }
    while (a[j] > pivot) {
        j--;
    }
    if (i <= j) {
      std::swap(a[i], a[j]);
      i++; j--;
    }
  } while (i <= j);
#ifdef USE_OMP
  #pragma omp task shared(a)
  {
#endif
    if (j > 0) {
      quickSort(a, j+1);
    }
#ifdef USE_OMP
  }
  #pragma omp task shared(a)
  {
#endif
    if (n > i) {
      quickSort(a + i, n - i);
    }
#ifdef USE_OMP
  }
  #pragma omp taskwait
#endif
}