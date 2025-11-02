#pragma once
#include <assert.h>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <cstdlib>
#include <utility>

using namespace std;

template <typename T>
class FVector
{
public:
  FVector(size_t const MaxElementsNum = 50, size_t const GrowSize = 50) :
    ArrayBuffer(nullptr),
    MaxElementsNumber(MaxElementsNum),
    GrowSize(GrowSize),
    NumElements(0)
  {
  }

  template<typename... Args>
  FVector(Args&&... args) :
    ArrayBuffer(nullptr),
    MaxElementsNumber(50),
    GrowSize(50),
    NumElements(0)
  {
    cout << "First allocation happened" << endl;
    ArrayBuffer = static_cast<T*>(malloc(sizeof(T) * MaxElementsNumber));
    assert(ArrayBuffer);
    ((new (ArrayBuffer + NumElements++) T(forward<Args>(args))), ...);
  }

  ~FVector()
  {
    if constexpr (is_trivially_copyable_v<T> == false)
    {
      for (size_t i = 0; i < NumElements; ++i)
      {
        destroy_at(&ArrayBuffer[i]);
      }
    }
    free(ArrayBuffer);
    ArrayBuffer = nullptr;
    cout << "Memory is freed" << endl;
  }

  T& operator[](size_t const Index)
  {
    assert(IsInRange(Index));
    return *(ArrayBuffer + Index);
  }

  template<typename... Args>
  inline size_t Add(Args&&... args)
  {
    if (NumElements <= 0)
    {
      cout << "First allocation happened" << endl;
      ArrayBuffer = static_cast<T*>(malloc(sizeof(T) * MaxElementsNumber));

    }
    assert(ArrayBuffer != nullptr);
    if (NumElements >= MaxElementsNumber)
    {
      Expand();
    }
    new (ArrayBuffer + NumElements) T(forward<Args>(args)...);
    return NumElements++;
  }

  template<typename... Args>
  inline ptrdiff_t AddUnique(Args&&... args)
  {
    if (Find(forward<Args>(args)...) == -1)
    {
      return Add(forward<Args>(args)...);
    }
    return -1;
  }

  template<typename...Args>
  inline size_t InsertAt(size_t const Index, Args&&... args)
  {
    assert(Index <= NumElements);
    if (NumElements >= MaxElementsNumber)
    {
      Expand();
    }
    cout << "InsertAt" << endl;
    if constexpr (is_trivially_copyable_v<T> == true)
    {
      for (size_t i = NumElements; i > Index; --i)
      {
        ArrayBuffer[i] = ArrayBuffer[i - 1];
      }
      ArrayBuffer[Index] = T(forward<Args>(args)...);
    }
    else
    {
      for (size_t i = NumElements; i > Index; --i)
      {
        new (ArrayBuffer + i) T(move(ArrayBuffer[i - 1]));
      }
      new (ArrayBuffer + Index) T(std::forward<Args>(args)...);
    }
    return ++NumElements;
  }

  template <typename T>
  inline bool RemoveElement(T&& Element)
  {
    for (size_t i = 0; i < NumElements; ++i)
    {
      if (ArrayBuffer[i] == forward<decltype(Element)>(Element))
      {
        RemoveAt(i);
        return true;
      }
    }
    return false;
  }
  size_t RemoveAt(size_t const Index);
  size_t Pop();
  void Clear();
  inline size_t Size() const { return NumElements; }
  inline size_t Capacity() const { return MaxElementsNumber - NumElements; }
  void SwapElements(size_t const lIndex, size_t const rIndex);
  bool IsInRange(size_t const Index) const { return  Index >= 0 && Index < NumElements; }
  
  template<typename T>
  inline ptrdiff_t Find(T&& Val) const
  {
    for (size_t i = 0; i < NumElements; ++i)
    {
      if (ArrayBuffer[i] == forward<decltype(Val)>(Val))
      {
        return i;
      }
    }
    return -1;
  }

  template<typename U = T>
  enable_if_t<is_integral_v<U>>
    Sort()
  {
    if (ArrayBuffer == nullptr || NumElements <= 1)
    {
      return;
    }
    QuickSort(0, NumElements - 1);
  }

private:
  T* ArrayBuffer = nullptr;
  size_t NumElements = 0;
  size_t MaxElementsNumber = 0;
  size_t GrowSize = 0;

  void Expand();
  size_t Partition(size_t const lIndex, size_t const rIndex, T const Pivot);
  void QuickSort(size_t const lIndex, size_t const rIndex);
};

template<typename T>
inline size_t FVector<T>::RemoveAt(size_t const Index)
{
  assert(IsInRange(Index));
  if constexpr (is_trivially_copyable_v<T> == true)
  {
    for (size_t i = Index; i < NumElements; ++i)
    {
      ArrayBuffer[i] = ArrayBuffer[i + 1];
    }
  }
  else
  {
    destroy_at(&ArrayBuffer[Index]);
    move(ArrayBuffer + Index + 1, ArrayBuffer + NumElements, ArrayBuffer + Index);
  }
  return --NumElements;
}

template<typename T>
inline size_t FVector<T>::Pop()
{
  if constexpr (is_trivially_copyable_v<T> == false)
  {
    assert(IsInRange(NumElements - 1));
    destroy_at(&ArrayBuffer[NumElements - 1]);
  }
  return --NumElements;
}

template<typename T>
inline void FVector<T>::Clear()
{
  if constexpr (is_trivially_copyable_v<T> == false)
  {
    for (size_t i = 0; i < NumElements; ++i)
    {
      destroy_at(&ArrayBuffer[i]);
    }
  }
  NumElements = 0;
}

//template<typename T>
//inline ptrdiff_t FVector<T>::Find(T&& Val) const
//{
//  for (size_t i = 0; i < NumElements; ++i)
//  {
//    if (ArrayBuffer[i] == Val)
//    {
//      return i;
//    }
//  }
//  return -1;
//}

template<typename T>
inline void FVector<T>::Expand()
{
  cout << "Expansion happened" << endl;
  assert(GrowSize > 0);
  MaxElementsNumber += GrowSize;
  T* TempBuffer = static_cast<T*>(malloc(sizeof(T) * MaxElementsNumber));
  if constexpr (is_trivially_copyable_v<T> == true)
  {
    memcpy(TempBuffer, ArrayBuffer, sizeof(T) * NumElements);
  }
  else
  {
    for (size_t i = 0; i < NumElements; ++i)
    {
      new (TempBuffer + i) T(move_if_noexcept(ArrayBuffer[i]));
      destroy_at(&ArrayBuffer[i]);
    }
  }
  free(ArrayBuffer);
  ArrayBuffer = TempBuffer;
}

template<typename T>
inline void FVector<T>::SwapElements(size_t const lIndex, size_t const rIndex)
{
  assert(IsInRange(lIndex) && IsInRange(rIndex));
  auto Temp = ArrayBuffer[lIndex];
  ArrayBuffer[lIndex] = ArrayBuffer[rIndex];
  ArrayBuffer[rIndex] = Temp;
}

template<typename T>
inline size_t FVector<T>::Partition(size_t const lIndex, size_t const rIndex, T const Pivot)
{
  assert(IsInRange(lIndex) && IsInRange(rIndex));
  size_t CurrentLeft = lIndex;
  size_t CurrentRight = rIndex - 1;

  while (true)
  {
    while (ArrayBuffer[CurrentLeft] < Pivot)
    {
      ++CurrentLeft;
    }

    while (CurrentRight > 0 && ArrayBuffer[CurrentRight] > Pivot)
    {
      --CurrentRight;
    }

    if (CurrentLeft >= CurrentRight)
    {
      break;
    }
    SwapElements(CurrentLeft, CurrentRight);
  }
  SwapElements(CurrentLeft, rIndex);
  return CurrentLeft;
}

template<typename T>
inline void FVector<T>::QuickSort(size_t const lIndex, size_t const rIndex)
{
  if (rIndex <= lIndex || (rIndex - lIndex) <= 0)
  {
    return;
  }
  size_t PivotIndex = Partition(lIndex, rIndex, ArrayBuffer[rIndex]);
  QuickSort(lIndex, PivotIndex == 0 ? PivotIndex : PivotIndex - 1);
  QuickSort(PivotIndex + 1, rIndex);
}