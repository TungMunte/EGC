#pragma once

namespace support
{
	int position(int x) {
		srand((unsigned)time(0));
		int test1 = (4 * x / 10);
		return 2 * x / 10 + (rand() % test1);
	}

	float angle() {
		srand((unsigned)time(0));
		float value = -M_PI_2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / M_PI));
		while (value == 0 || value <= -M_PI_2 || value >= M_PI_2)
		{
			value = -M_PI_2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / M_PI));
		}
		return value;
	}
}