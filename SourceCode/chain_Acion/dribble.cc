/*
 * dribble.cc
 *
 *  Created on: Feb 27, 2017
 *      Author: aastteam
 */
#include "analyzer.h"
#include "../behaviors/naobehavior.h"

#include "../math/vecposition.h"
#include "../math/Geometry.h"
#include "skillpass.h"
#include "support.h"
#include <cstdio>
#include <cstdlib>
#include <map>
#include<string>
#include <fstream>
#define eps 0.01
#define INF 55555
using namespace std;
using namespace SIM;

bool bii = false;
dribble::dribble(WorldModel *wm, formationLoader *fm) :
		skilldesc(SKILL_DRIBBLE) {
	this->wm = wm;
	this->fm = fm;
	angle = 0;
	map<string, double> factors;
	loadParameters(fm->getStrategy() + "/dribble.io", factors);

	CTime = factors["CTime"];
	C_TheirGoal = factors["theirGoalConst"];
	TheirGoalP = factors["TheirGoalP"];
	surrP = factors["surrP"];
	Cnear = factors["Cnear"];
	effP = factors["effP"];
	safeP = factors["safeP"];
	factor = factors["factor"];
	if (!bii) {
		stringstream ss;
		ss << CTime << "_" << C_TheirGoal << "_" << TheirGoalP << "_" << surrP
				<< "_" << Cnear << "_" << effP << "_" << safeP << "_" << factor;
		string mess;
		ss >> mess;
		NaoBehavior::debug->debug(0, mess);
		bii = true;
	}
}
double dribble::surroundingOpponents() {
	double ret = 0;
	int n = 0;
	for (int i = 0 + WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| wm->isOut(wm->getWorldObject(i)->pos)
				|| wm->getWorldObject(i)->pos.getDistanceTo(target) > 4)
			continue;
		ret += exponential(wm->getPlayerTimeTo(i, target), Cnear);
		n++;
	}
	double width = HALF_FIELD_Y;
	width -= abs(target.getY());
	ret += exponential(width, Cnear);
	return n == 0 ? 0 : ret / 6;
}

double dribble::dribbleSafety() {

	double length = (target - wm->getMyPosition()).getMagnitude();

	double nearest = 1e9;

	int targetPlayer = wm->getTeammateClosestTo(target);

	for (int i = 0 + WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
		int curUNum = i - WO_TEAMMATE1 + 1;
		if (wm->getWorldObject(i)->validPosition && wm->getUNum() != curUNum
				&& targetPlayer != curUNum)

			nearest = min(nearest,
					perp(wm->getTeammate(i), wm->getMyPosition(), target));
	}

	for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++)
		if (wm->getWorldObject(i)->validPosition)
			nearest = min(nearest,
					perp(wm->getOpponent(i), wm->getMyPosition(), target));

	const double th = 0.2;

	if (nearest <= th || wm->isOut(target)) {
		return 1;
	}
	double ret = exponential(nearest, 0.5);

	return ret;

}
double dribble::effectiveness() {
	double myGoal = exponential(wm->distanceToMyGoal(target), 130);
	double theirGoal = 1
			- exponential(wm->distanceToOppGoal(target), TheirGoalP);
	double surrounding_Opponents = surroundingOpponents();
	//	double supporting_TeamMates = supportingTeamMates(target);

	double ret = (myGoal + theirGoal + surrounding_Opponents);
	return ret;
}

double dribble::calcCost() {
	double e = effectiveness();
	double s = dribbleSafety();
	double ret = 6 * e + 1*s; // + surrP*dribbleReliability();
	//cout<< "DE = "<<e<< " DS = "<<s <<" DC = "<<ret<<"\n";
	//ret /= (effP+safeP+surrP);
	//ret*=factor;
	calcTime();
	return cost = ret;
}
ndribble::ndribble(WorldModel *wm, formationLoader *fm) :
		skilldesc(SKILL_DRIBBLE) {
	this->wm = wm;
	this->fm = fm;
	angle = 0;
	shift = 0;
	this->wm = wm;
	this->fm = fm;
	map<string, double> factors;
	loadParameters(fm->getStrategy() + "/dribble.io", factors);

	factor = factors["factor"];
	if (!bii) {
		stringstream ss;

		string mess;
		ss >> mess;
		NaoBehavior::debug->debug(0, mess);
		bii = true;
	}
}
double ndribble::calcCost() {
	return cost = safety() + shift / 90;
}
void ndribble::setAngle(double angle) {
	VecPosition oppGoal = wm->getOppLeftGoalPost() + wm->getOppRightGoalPost();
	oppGoal /= 2;
	this->angle = angle
			+ (oppGoal - wm->getBall()).getAngleWithVector(
					VecPosition(1, wm->getMyAngRad(), 0, POLAR));
	shift = fabs(angle);
	target = VecPosition(1, this->angle, 0, POLAR) + wm->getBall();

}
double ndribble::getAngle() {
	return angle;
}
double ndribble::safety() {
	double time = angle / 15;
	time += 2;
	for (int i = WO_OPPONENT1; i <= WO_OPPONENT11; i++) {
		if (wm->getPlayerTimeTo(i, target) < time) {
			return INF;
		}

	}
	return 0;
}
