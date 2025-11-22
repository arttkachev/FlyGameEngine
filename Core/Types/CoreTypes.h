#pragma once

#include <assert.h>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <cstdlib>
#include <utility>

using int32 = int32_t;
using uint32 = uint32_t;
using int16 = int16_t;
using uint16 = uint16_t;

#define WinPlatform (_WIN32) || (_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__);

#define ENGINE_ERROR(error) \
    std::cerr <<error << " : " << __FUNCTION__ << " : " << __LINE__ <<  std::endl

template <typename... Args>
constexpr void ENGINE_LOG(Args&& ... args)
{
  ((std::cout << std::forward<Args>(args)), ...) << std::endl;
}

namespace Engine
{
  static bool IsAppRunning = false;
  static constexpr bool IsRunOnArchitectureLSB()
  {
    constexpr unsigned char LSB_Char = 1;
    return *(unsigned char*)(&LSB_Char);
  }

  namespace Window
  {
    GLFWwindow* AppWindow = nullptr;
    static const char* Title = "Flyeng v1.0";
    static uint16 Width = 1440;
    static uint16 Height = 1080;
    static bool IsFullScreen = true;
  }
}

namespace VulkanSpecs
{
  static HMODULE Vk_Lib = nullptr;
  static VkInstance Vk_Instance = nullptr;
  static PFN_vkGetInstanceProcAddr Vk_GetInstanceProcAddr = nullptr;
  static const char* AppTitle = "Vulkan Renderer";
  static uint32 AppVersion = 1;
  static uint32 ApiVersion = 0;
}

using namespace std;

template <typename T>
class FVector
{
public:
  FVector(size_t const MaxElementsNum = 100, size_t const GrowSize = 100) :
    ArrayBuffer(nullptr),
    MaxElementsNumber(MaxElementsNum),
    GrowSize(GrowSize),
    NumElements(0)
  {
  }

  template<typename... Args>
  FVector(Args&&... args) :
    ArrayBuffer(nullptr),
    MaxElementsNumber(100),
    GrowSize(100),
    NumElements(0)
  {

    ENGINE_LOG("First allocation happened");
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
    ENGINE_LOG("Memory is freed");
  }

  T& operator[](size_t const Index) const
  {
    assert(IsInRange(Index));
    return *(ArrayBuffer + Index);
  }

  template<typename... Args>
  inline size_t Add(Args&&... args)
  {
    if (NumElements == 0)
    {
      ENGINE_LOG("First allocation happened");
      ArrayBuffer = static_cast<T*>(malloc(sizeof(T) * MaxElementsNumber));

    }
    assert(ArrayBuffer != nullptr);
    if (NumElements >= MaxElementsNumber)
    {
      Expand(GrowSize);
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

  inline void Reserve(const size_t ElemNum)
  {
    assert(ElemNum > 0);
    if (NumElements == 0)
    {
      MaxElementsNumber = ElemNum;
      GrowSize = MaxElementsNumber;
    }
    else
    {
      Expand(ElemNum);
    }
  }

  template<typename T>
  inline void Assign(const size_t Count, T&& Val)
  {
    assert(Count <= NumElements);
    for (size_t i = 0; i < Count; ++i)
    {
      ArrayBuffer[i] = forward<T>(Val);
    }
  }

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

  void Expand(const size_t ExpandSize);
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

template<typename T>
inline void FVector<T>::Expand(const size_t ExpandSize)
{
  assert(GrowSize > 0);
  ENGINE_LOG("Expansion happened");
  MaxElementsNumber = (GrowSize == ExpandSize) ? MaxElementsNumber + GrowSize : ExpandSize;
  const size_t ElementsToCopy = (NumElements <= MaxElementsNumber) ? NumElements : MaxElementsNumber;
  T* TempBuffer = static_cast<T*>(malloc(sizeof(T) * MaxElementsNumber));
  assert(TempBuffer != nullptr);
  if constexpr (is_trivially_copyable_v<T> == true)
  {
    memcpy(TempBuffer, ArrayBuffer, sizeof(T) * ElementsToCopy);
  }
  else
  {
    for (size_t i = 0; i < ElementsToCopy; ++i)
    {
      new (TempBuffer + i) T(move_if_noexcept(ArrayBuffer[i]));
      destroy_at(&ArrayBuffer[i]);
    }
    if (ElementsToCopy < NumElements)
    {
      for (size_t i = ElementsToCopy; i < NumElements; ++i)
      {
        destroy_at(&ArrayBuffer[i]);
      }
    }
  }
  NumElements = ElementsToCopy;
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

static constexpr auto BITS_IN_BYTE{ 8 };
static constexpr auto MAX_UCHAR_VAL{ 0xff };
static constexpr auto BIT_IN_CHAR(const uint32 Bit)
{
  if constexpr (Engine::IsRunOnArchitectureLSB())
  {
    return 1 << Bit % BITS_IN_BYTE;
  }
  else
  {
    return (1 << (BITS_IN_BYTE - 1 - ((Bit) % BITS_IN_BYTE)));
  }
};
static constexpr auto BIT_TO_CHAR_INDEX(const uint32 Bit) { return Bit / BITS_IN_BYTE; }
static constexpr auto MIN_CHAR_NUM_FOR_BITS(const uint32 Bits) { return (((Bits - 1) / CHAR_BIT) + 1); }

class FBitArray
{
public:
  FBitArray(const uint32 BitsNum) :
    TotalBits(BitsNum),
    TotalBytes(MIN_CHAR_NUM_FOR_BITS(TotalBits))
  {
    assert(BitsNum > 0);
    BitBuffer.Reserve(TotalBytes);
    for (size_t i = 0; i < TotalBytes; ++i)
    {
      BitBuffer.Add(0);
    }
  }

  ~FBitArray() = default;

  size_t operator[](const size_t Index) const
  {
    assert(BitBuffer.IsInRange(Index));
    return BitBuffer[Index];
  }

  void ClearAllBits()
  {
    BitBuffer.Assign(BitBuffer.Size(), 0);
  }

  void SetAllBits()
  {
    BitBuffer.Assign(BitBuffer.Size(), MAX_UCHAR_VAL);
    unsigned char BitRemainder = TotalBits % BITS_IN_BYTE;
    if (BitRemainder != 0)
    {
      unsigned char mask = 0;
      if constexpr (Engine::IsRunOnArchitectureLSB())
      {
        mask = MAX_UCHAR_VAL >> (BITS_IN_BYTE - BitRemainder);
      }
      else
      {
        mask = MAX_UCHAR_VAL << (BITS_IN_BYTE - BitRemainder);
      }
      BitBuffer[BIT_TO_CHAR_INDEX(TotalBits - 1)] = mask;
    }
  }

  void SetByte(const size_t ByteIndex, const unsigned char Val)
  {
    assert(BitBuffer.IsInRange(ByteIndex));
    BitBuffer[ByteIndex] = Val;
  }

  void SetBit(const size_t BitIndex)
  {
    assert(TotalBits > BitIndex);
    BitBuffer[BIT_TO_CHAR_INDEX(BitIndex)] |= BIT_IN_CHAR(BitIndex);
  }

  void ClearBit(const size_t BitIndex)
  {
    assert(TotalBits > BitIndex);
    BitBuffer[BIT_TO_CHAR_INDEX(BitIndex)] &= ~BIT_IN_CHAR(BitIndex);
  }

private:
  FVector<unsigned char> BitBuffer{};
  uint32 TotalBits = 0;
  uint32 TotalBytes = 0;
};