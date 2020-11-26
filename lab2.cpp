#include "pch.h"
#include <iostream>
#include <condition_variable>
#include <thread>
#include <fstream>
#include <string>
#include "Point.h"

std::mutex mutex;
std::condition_variable condition_variable;
bool message_result = false;
bool message_log = false;
bool function_result_done = false;
int function_result = 1;
int function_value = 1;
clock_t calculate_time, record_time;
std::fstream result_file;

void init_file(char filename[]) {
	result_file.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
	result_file.close();
}

void record_to_file(char filename[], std::string result) {
	result_file.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
	result_file << result << "\n";
	result_file.close();
}

void result_writer() {
	const char filename[] = "../result.txt";

	init_file(const_cast<char*>(filename));

	std::unique_lock<std::mutex> lock(mutex);

	while (!function_result_done) {
		while (message_result) {
			record_to_file(const_cast<char*>(filename), std::to_string(function_result));
			record_time = clock();
			message_result = false;
			message_log = true;
			condition_variable.notify_all();
		}
		condition_variable.wait(lock);
	}
}

void log_writer() {
	const char filename[] = "../log.txt";
	init_file(const_cast<char*>(filename));

	std::unique_lock<std::mutex> lock(mutex);

	while (!function_result_done) {
		while (message_log) {
			message_log = false;
			std::shared_ptr<Point> current_point(new Point(function_result, std::to_string(((float)calculate_time)
				/ CLOCKS_PER_SEC), std::to_string(((float)record_time) / CLOCKS_PER_SEC)));
			record_to_file(const_cast<char*>(filename), current_point->log_value());
		}
		condition_variable.wait(lock);								 // ждать, пока не появится уведомление
	}
}

void function(int value) {
	for (function_value = 1; function_value <= value; function_value++) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		function_result = function_result * function_value;
		calculate_time = clock();
		message_result = true;
		condition_variable.notify_all();							// уведомить всех
	}
	function_result_done = true;
	condition_variable.notify_all();
}

int main() {
	std::thread function(function, 10);
	std::thread result_of_function(result_writer);
	std::thread log(log_writer);

	function.join();
	result_of_function.join();
	log.join();

	std::cout << "Function calculation is over.";
	return 0;
}