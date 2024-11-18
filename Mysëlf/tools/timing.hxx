#ifndef TIMER_HXX
#define TIMER_HXX

class performance_timer
{
public:
	performance_timer(int thread_id) : thread_id(thread_id)
	{
		QueryPerformanceFrequency(&frequency);
		inv_frequency = 1000.0 / static_cast<double>(frequency.QuadPart);
		QueryPerformanceCounter(&start_time);
	}

	__forceinline static auto update_tick(int thread_id) -> void
	{
		QueryPerformanceCounter(&current_time[thread_id]);
	}

	__forceinline auto time_exceeded(std::uint32_t time_delta) -> bool
	{
		LARGE_INTEGER now = current_time[this->thread_id];

		double elapsed_time = static_cast<double>(now.QuadPart - this->start_time.QuadPart) * this->inv_frequency;
		if (elapsed_time >= static_cast<double>(time_delta))
		{
			start_time = now;
			return true;
		}

		return false;
	}

private:
	int thread_id;
	LARGE_INTEGER start_time;
	LARGE_INTEGER frequency;
	double inv_frequency;

	static inline LARGE_INTEGER current_time[4];
};

class limiter
{
public:
	limiter(unsigned int target_tps)
		: target_frame_duration(std::chrono::nanoseconds(1000000000 / target_tps)) {
	}

	__forceinline auto start() -> void
	{
		start_time = std::chrono::high_resolution_clock::now();
	}

	__forceinline auto end() -> void
	{
		auto ent_time = start_time + target_frame_duration;
		while (std::chrono::high_resolution_clock::now() < ent_time) {}
	}

private:
	std::chrono::high_resolution_clock::time_point start_time;
	std::chrono::nanoseconds target_frame_duration;
};

#endif // !TIMER_HXX
