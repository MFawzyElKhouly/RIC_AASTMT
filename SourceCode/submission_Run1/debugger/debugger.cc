/*
 * debugger.cc
 *
 *  Created on: Mar 3, 2017
 *      Author: aastteam
 */


#include "debugger.h"

Debugger::Debugger(string fileName) {
	file = fileName;
	fstream of(file.c_str(), ios_base::out);
		of << "start\n";
			of.close();
}
void Debugger::debug(long time , string message) {
	fstream of(file.c_str(), ios_base::out| ios_base::app);
	of << "[DEBUG] "<<time<<" "<<message<<"\n";
		of.close();
}
void Debugger::success(long time , string message) {
	fstream of(file.c_str(), ios_base::out| ios_base::app );
	of << "[SUCCESS] "<<time<<" "<<message<<"\n";
		of.close();
}
void Debugger::fail(long time , string message) {
	fstream of(file.c_str(), ios_base::out | ios_base::app);
	of << "[ERROR] "<<time<<" "<<message<<"\n";
		of.close();
}

