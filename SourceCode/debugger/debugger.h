/*
 * debugger.h
 *
 *  Created on: Mar 3, 2017
 *      Author: aastteam
 */

#ifndef DEBUGGER_DEBUGGER_H_
#define DEBUGGER_DEBUGGER_H_

#include<fstream>
#include <string>
using namespace std;
class Debugger {
private :
	 string file;
public :
	 Debugger(string fileName);
	 void success(long time,string message);
	 void fail(long time,string message);
	 void debug(long time,string message);
	 ~Debugger() {

	 }
};


#endif /* DEBUGGER_DEBUGGER_H_ */
