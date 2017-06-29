/*
 * evaluator.cc
 *
 *  Created on: Jun 29, 2017
 *      Author: rc9
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

Evaluator::Evaluator(WorldModel *wm,formationLoader *load) {
		this->wm = wm;
		this->maxShot = 10; //TODO get this from wm
		loader = NULL;
		this->maxPass = 5;
		max = (new skilldesc(SKILL_STAND));
}
double Evaluator::evalPass(VecPosition v) {
	double val = 0;

	return val;
}
