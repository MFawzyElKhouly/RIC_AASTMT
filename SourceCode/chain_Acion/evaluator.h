/*
 * evaluator.h
 *
 *  Created on: Jun 26, 2017
 *      Author: Ahmed Hamdy
 */

#ifndef SOURCECODE_CHAIN_ACION_EVALUATOR_H_
#define SOURCECODE_CHAIN_ACION_EVALUATOR_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>

#include <boost/shared_ptr.hpp>

#include "../skills/skill.h"
#include "skillval.h"
#include <map>
#include <fstream>
using namespace std;



class WorldModel;
class BodyModel;
class NaoBehavior;
class VecPosition;

class formationLoader;

using namespace std;

class Evaluator {
private:
	WorldModel *wm;
	formationLoader *loader;
	vector<skilldesc> skillset;
	//skilldesc *max;

	double maxShot;
protected:
public:
	Evaluator(WorldModel *wm);

	Evaluator(WorldModel *wm,formationLoader *loader);
	double positionValue(VecPosition target);
	double passSafety(VecPosition target);
	virtual ~Evaluator() {

	}

};




#endif /* SOURCECODE_CHAIN_ACION_EVALUATOR_H_ */
