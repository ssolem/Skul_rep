#pragma once

struct ScopedTimer
{
	const char* name;
	LARGE_INTEGER start;

	ScopedTimer(const char* name)
	{
#ifdef _DEBUG
		this->name = name;
		QueryPerformanceCounter(&start);
#endif
	}

	~ScopedTimer()
	{
#ifdef _DEBUG
		LARGE_INTEGER end, freq;
		QueryPerformanceCounter(&end);
		QueryPerformanceFrequency(&freq);

		double ms = (end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
		printf("%s: %.2f ms\n", name, ms);
#endif
	}
};
