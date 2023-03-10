#pragma once
#include <cstring>

#include "BaseStruct.h"

template<class T>
class UMyBuffer
{
private:
	T* BufferData;
	FSize Size;
	FSize MaxIndex;
	unsigned int ArrayMaxIndex;
	unsigned int* ValidDataIdxArray;
	unsigned int LastValidIdx;
	bool* ValidDataMap;
	T DefaultValue;

public:

	UMyBuffer(FSize InSize, T InDefaultValue)
	{
		Size = InSize;
		MaxIndex = Size - 1;
		size_t MSize = Size.X * Size.Y * sizeof(T);
		ArrayMaxIndex = Size.X * Size.Y - 1;
		BufferData = static_cast<T*>(malloc(MSize));
		ValidDataIdxArray = static_cast<unsigned int*>(malloc(Size.X * Size.Y * sizeof(unsigned int)));
		ValidDataMap = static_cast<bool*>(malloc(Size.X * Size.Y * sizeof(bool)));
		LastValidIdx = 0;
		DefaultValue = InDefaultValue;
		Clear();
	}

	~UMyBuffer()
	{
		free(BufferData);
		free(ValidDataIdxArray);
		free(ValidDataMap);
	}

	bool SetData(int X, int Y, const T Item)
	{
		if (X > MaxIndex.X || X < 0) return false;
		if (Y > MaxIndex.Y || Y < 0) return false;

		const int Index = Y * Size.X + X;
		
		return SetDataByIndex(Index, Item);
	}

	bool SetDataByIndex(int Index, const T Item)
	{
		if (Index > ArrayMaxIndex) return false;

		BufferData[Index] = Item;
		if (!ValidDataMap[Index])
		{
			ValidDataMap[Index] = true;
			ValidDataIdxArray[LastValidIdx] = Index;
			LastValidIdx++;
		}
		return true;
	}

	void GetValidDataIdxArray(const unsigned int*& OutValidDataIdxArray, unsigned int& OutSize) const
	{
		OutValidDataIdxArray = ValidDataIdxArray;
		OutSize = LastValidIdx;
	}


	bool GetDataByIndex(int Index, T*& OutData)
	{
		if (Index > ArrayMaxIndex) return false;

		if (ValidDataMap[Index] == false)
		{
			OutData = &DefaultValue;
			return false;
		}

		OutData = &(BufferData[Index]);
		return true;
	}

	bool GetData(int X, int Y, T*& OutData)
	{
		if (X > MaxIndex.X || X < 0) return false;
		if (Y > MaxIndex.Y || Y < 0) return false;

		const int Index = Y * Size.X + X;
		return GetDataByIndex(Index, OutData);
	}

	void Clear()
	{
		memset(ValidDataMap, 0, Size.X * Size.Y * sizeof(bool));
		LastValidIdx = 0;
	}
};

class UFrameBuffer
{

public:
	UMyBuffer<FVector4>* ColorBuffer;
	UMyBuffer<float>* DepthBuffer;

	UFrameBuffer(FSize InScreenSize)
	{
		ColorBuffer = new UMyBuffer<FVector4>(InScreenSize, { 0, 0, 0, 1 });
		DepthBuffer = new UMyBuffer<float>(InScreenSize, -1);
	}

	~UFrameBuffer()
	{
		delete ColorBuffer;
		delete DepthBuffer;
	}

	void ClearBuffer()
	{
		ColorBuffer->Clear();
		DepthBuffer->Clear();
	}
};

