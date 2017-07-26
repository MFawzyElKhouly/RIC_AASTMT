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
double maxShot = 10;
double A = 20, B = 60.0, C = 20.0, Clen = 10.5, CNear = 7.4;
supportSkill::supportSkill(WorldModel *wm, formationLoader *loader) :
		skilldesc(SKILL_SUPPORT) {
	this->skill = SKILL_WALK_OMNI;
	this->loader = loader;
	this->wm = wm;
	this->target = wm->getBall();
	map<string, double> factors;
//	loadParameters(loader->getStrategy() + "/passing.io", factors);
//		A = factors["length"];
//		B = factors["support"];
//		C = factors["pressure"];
//		Clen = factors["lengthConstant"];
//		CNear = factors["pressConstant"];
//		factor  = factors["factor"];
	int bh = wm->getTeammateClosestTo(wm->getBall());
	int cl = 1; // second closest to ball
	double dd = 100;
	for (int i = WO_TEAMMATE2; i <= WO_TEAMMATE11; i++) {
		if (i - WO_TEAMMATE1 + 1 == bh)
			continue;
		if (wm->getBall().getDistanceTo(wm->getTeammate(i)) < dd) {
			dd = wm->getBall().getDistanceTo(wm->getTeammate(i));
			cl = i - WO_TEAMMATE1 + 1;
		}
	}
	VecPosition ball = wm->getBall();
	if (wm->getBall().getDistanceTo(wm->getMyPosition()) > maxShot - 2) {

		target = wm->getBall();
	}

	else if(ball.getDistanceTo(wm->getMyPosition()) > 5
		//&& wm->getRole(wm->getUNum()) > 2
			&&ball.getX() > 0
		&& ball.getDistanceTo((wm->getOppLeftGoalPost()+wm->getOppRightGoalPost())/2) < 7.5 && wm->getUNum() > 6) {

		target = wm->getBall();
	}
	else {
		cost = 1000;
		double mtheta = -180;
		for (double theta = -180; theta < 180; theta += 15) {
			double x = generateTarget(theta);
			if (x < cost) {
				cost = x;
				mtheta = theta;
			}
		}
		target = VecPosition(0.5, mtheta, 0, POLAR) + wm->getMyPosition();

	}

}
double supportSkill::generateTarget(double theta) {
	target = VecPosition(0.5, theta, 0, POLAR) + wm->getMyPosition();
	double myGoal =  + 1
			- exponential(wm->distanceToOppGoal(target), 22);
	double ret = 0;


	for (int i = 0 + WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| wm->isOut(wm->getWorldObject(i)->pos)
				|| target.getDistanceTo(wm->getOpponent(i)) > 4)
			continue;
		ret += exponential(wm->getOpponent(i).getDistanceTo( target), 8.5);
	}
	double supported  = 0;
	for (int i = 0 + WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
			if (!wm->getWorldObject(i)->validPosition
					|| wm->isOut(wm->getWorldObject(i)->pos)
					|| target.getDistanceTo(wm->getTeammate(i)) > 4)
				continue;
			supported=  max(supported,exponential(wm->getPlayerTimeTo(i, target), 8.5));
		}
//	for (int i = 0 + WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++) {
//
//	}
	double nearest = 1e9;
	for (int i = 0 + WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
		int curUNum = i - WO_TEAMMATE1 + 1;
		if (wm->getWorldObject(i)->validPosition && wm->getUNum() != curUNum
				&& target != curUNum)

			nearest = min(nearest,
					perp(wm->getTeammate(i), wm->getBall(), target));
	}

	for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++)
		if (wm->getWorldObject(i)->validPosition)
			nearest = min(nearest,
					perp(wm->getOpponent(i), wm->getBall(), target));

	const double th = 0.2;

//	if (nearest <= th) {
//		return 100;
//	}
//	if(wm->getUNum() == 3 || wm->getUNum() == 4){
//		VecPosition bigOpp = wm->getOpponentClosestTo(
//				(wm->getMyLeftGoalPost()+ wm->getMyRightGoalPost())/2);
//
//		if(target.getX() > bigOpp.getX())
//			return target.getX();
//	}
	return ret + 5*myGoal +supported;
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
	return n == 0 ? 0 : ret / n;

}
double supportSkill::currentPressure() {
	double ret = 0;
	int n = 0;
	for (int i = 0 + WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition)
			continue;
		ret += exponential(wm->getOpponent(i).getDistanceTo(wm->getBall()),
				CNear);
	}
	return n == 0 ? 0 : ret;
}

double supportSkill::calcCost() {
	return cost;
}
