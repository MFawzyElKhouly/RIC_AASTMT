/*
 * support.cc
 *
 *  Created on: Feb 8, 2017
 *      Author: aastteam
 */

#include "support.h"
#include "analyzer.h"
#include "../math/vecposition.h"
#include "../math/Geometry.h"

#include "../worldmodel/worldmodel.h"
using namespace std;
using namespace SIM;
double maxShot = 12;
double A=20, B=60.0, C = 20.0, Clen = 10.5,CNear=7.4;
supportSkill::supportSkill(WorldModel *wm,formationLoader *loader) :
			skilldesc(SKILL_SUPPORT) {
		this->skill = SKILL_WALK_OMNI;
		this->loader =loader;
		this->wm = wm;
		this->target = wm->getBall();
		map<string,double> factors;
		loadParameters(loader->getStrategy()+"/passing.io",factors);
		A = factors["length"];
		B = factors["support"];
		C = factors["pressure"];
		Clen = factors["lengthConstant"];
		CNear = factors["pressConstant"];
		factor  = factors["factor"];
	}
double supportSkill::lengthOfSupport() {
	double mylen = wm->distancetoBall(wm->getMyPosition());
	mylen -= maxShot;
	if (mylen < 0)
		return 1000000.0;
	double expo = exponential(mylen, Clen);
	return expo;

}
double supportSkill::currentSupport() {
	if (NUM_AGENTS == 1)
			return 0;
		double ret = 0;
		int n = 0;

		double mylen = wm->distancetoBall(wm->getMyPosition());
		for (int i = WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
			if (!wm->getWorldObject(i)->validPosition)
				continue;
			double distance = wm->getTeammate(i).getDistanceTo(wm->getBall());
			if (distance < mylen && distance < maxShot) {
				ret += 1 - exponential(distance, CNear);
				n++;
			}
		}
		return n == 0 ? 0 : ret/n;

}
double supportSkill::currentPressure() {
	double ret = 0;
		int n = 0;
		for (int i = 0 + WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++) {
			if (!wm->getWorldObject(i)->validPosition)
				continue;
			ret += exponential(
					wm->getOpponent(i).getDistanceTo(wm->getBall())
							, CNear);
		}
		return n == 0 ? 0 : ret / n;
}

double   supportSkill::calcCost() {
	double ret= A*lengthOfSupport() +
			B*currentSupport() +
			C*currentPressure();
	cost = factor * ret;
	return cost;
}
