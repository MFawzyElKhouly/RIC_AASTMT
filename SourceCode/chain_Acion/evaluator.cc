/*
 * evaluator.cc
 *
 *  Created on: Jun 26, 2017
 *      Author: Ahmed Hamdy
 */

#include "evaluator.h"
#include "../behaviors/naobehavior.h"

#include "../debugger/debugger.h"
#include "../math/vecposition.h"
#include "../math/Geometry.h"
#include "skillpass.h"
#include "support.h"
#include <cstdio>
#include <cstdlib>
#include <map>
#include<string>
#include <fstream>

#include <iostream>

#include <cstdio>
#define eps 0.01
#define INF 1<<12
using namespace std;
using namespace SIM;
Evaluator::Evaluator(WorldModel *wm) {
	//freopen("f.txt", "w", stdout);
	this->wm = wm;
	this->maxShot = 7; //TODO get this from wm
	loader = nullptr;
}
Evaluator::Evaluator(WorldModel *wm, formationLoader*loader) {
	this->wm = wm;
	this->maxShot = 10; //TODO get this from wm
	this->loader = loader;
}
	double Evaluator::positionValue(VecPosition target) {

	}
	double Evaluator::passSafety(VecPosition target) {

	}

