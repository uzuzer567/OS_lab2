#pragma once
class Point {
public:
	int function_result;
	bool destroy = false;
	std::string calculate_time;
	std::string record_time;

	Point(int result_value, std::string function_result_time, std::string record_result_time) {
		this->function_result = result_value;
		this->calculate_time = function_result_time;
		this->record_time = record_result_time;
	}

	~Point() {
		if (!this->destroy)
			std::cout << "The point didn't logged yet\n";
	}

	std::string log_value() {
		std::string record = "Function result: " + std::to_string(this->function_result) + ", calculated at " 
			+ this->calculate_time + " sec, recorded at " + this->record_time + " sec\n";
		destroy_point();
		return record;
	}

private:
	void destroy_point() {
		this->destroy = true;
	}
};