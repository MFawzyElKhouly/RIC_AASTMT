/*
 * evaluator.h
 *
 *  Created on: Jun 29, 2017
 *      Author: rc9
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
	skilldesc *max;
	VecPosition target;

	double maxShot;
	double maxPass;
public:
	Evaluator(WorldModel *wm);

	Evaluator(WorldModel *wm,formationLoader *loader);

	double evalPass(VecPosition v,int PlUnum);
	double posEffect();

};



#endif /* SOURCECODE_CHAIN_ACION_EVALUATOR_H_ */
