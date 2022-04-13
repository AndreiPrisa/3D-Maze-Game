#pragma once

class Timer {
public:
	float currentTime;
	float timeLimit;
	bool timedOut;

	Timer() {
		this->currentTime = 0;
		this->timeLimit = 0;
		this->timedOut = true;
	}

	Timer(float timeLimit) {
		this->currentTime = timeLimit;
		this->timeLimit = timeLimit;
		timedOut = false;
	}
	static Timer randomTimer(glm::vec2 interval) {
		return interval.x + (float)rand() / (RAND_MAX / (interval.y - interval.x));
	}
	void reset() {
		currentTime = this->timeLimit;
		timedOut = false;
	}

	void empty() {
		currentTime = 0;
		timedOut = true;
	}

	void update(float deltaTimeSeconds, float factor = 1) {
		if (this->isTimedOut()) {
			return;
		}

		this->currentTime -= deltaTimeSeconds * factor;
		if (this->currentTime < 0) {
			timedOut = true;
			this->currentTime = 0;
		}
	}

	void recharge(float deltaTimeSeconds, float factor = 1) {
		float newVal = this->currentTime + deltaTimeSeconds * factor;
		this->timedOut = false;
		if (newVal >= this->timeLimit) {
			this->currentTime = timeLimit;
		}
		else {
			this->currentTime = newVal;
		}
	}

	bool isTimedOut() {
		return timedOut;
	}

	bool isFull() {
		return currentTime == timeLimit;
	}

	//string convertToText() {

	//	int minutes = this->currentTime / 60;
	//	int seconds = fmod(this->currentTime, 60);
	//	char *result = new char[20];
	//	sprintf(result, "%02d:%02d", minutes, seconds);
	//	return result;
	//}

};