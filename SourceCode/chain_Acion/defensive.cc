/*
 * defensive.cc
 *
 *  Created on: Mar 6, 2017
 *      Author: aastteam
 */

#include "support.h"
#include "analyzer.h"
#include "skillpass.h"
#include "../math/vecposition.h"
#include "../math/Geometry.h"

#include "../worldmodel/worldmodel.h"
using namespace std;
using namespace SIM;
//double maxShot = 8;


	mark::mark(WorldModel *wm,formationLoader *fm):
				skilldesc(SKILL_WALK_OMNI) {
		this->wm =wm;
		this->fm = fm;
		map<string,double> factors;
			loadParameters(fm->getStrategy() + "/marking.io", factors);

			//double C_TheirGoal = 0.0 , L_TheirGoal = -0.7 , R_TheirGoal = 0.7;
			Cpos = factors["Cpos"], Cthre = factors["Cthre"];
			Cbal = factors["Cbal"];
			pos = factors["pos"], thre = factors["thre"];
						bal = factors["bal"];


			factor = factors["factor"];
		}
	double mark::calcCost() {
		if(wm->getTeammateClosestTo(target) != wm->getUNum() && (wm->getTeammateClosestTo(wm->getBall()) != wm->getTeammateClosestTo(target))
						&& wm->getTeammateClosestTo(target)!=1)
					return cost = 1000.5;

//				else if((wm->getOpponentClosestTo(target)+WO_OPPONENT1-1) == (wm->getOpponentClosestTo(wm->getBall())+WO_OPPONENT1-1))
//					return cost = 1000.5;

//		if((wm->getTeammateClosestTo(wm->getBall()) !=
//					wm->getTeammateClosestTo(target)) &&
//				wm->getTeammateClosestTo(target) != wm->getUNum())
//			return cost = INF;

//else if((wm->getOpponentClosestTo(target)+WO_OPPONENT1-1) == (wm->getOpponentClosestTo(wm->getBall())+WO_OPPONENT1-1))
//			return cost = INF;
			
		if(target.getDistanceTo((wm->getMyLeftGoalPost()+wm->getMyRightGoalPost())/2)<5)
			cost = thre*threatDist()*10+bal*ballDist()+pos*transitionDist();
		else
			cost = thre*threatDist()*7+bal*ballDist()+pos*transitionDist();
		cost/=(thre+bal+pos);
		cost*=factor;
		return cost;
	}
double mark::threatDist() {
	if(wm->distanceToMyGoal(target) > 15) {
		return INF;
	}
	return exponential(wm->distanceToMyGoal(target),Cthre);
}
double mark::ballDist() {
	if(target.getDistanceTo(wm->getBall()) > 15) {
		//return INF;
	}
	return exponential(target.getDistanceTo(wm->getBall()),Cbal);
}
double mark::transitionDist() {
	if(target.getDistanceTo(wm->getMyPosition()) > 10) {
		//return INF;
	}
	return exponential(target.getDistanceTo(wm->getMyPosition()),Cpos);

}




