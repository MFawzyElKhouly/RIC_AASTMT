/*
 * playmodestrategy.cc
 *
 *  Created on: Jun 23, 2017
 *      Author: Ahmed Hamdy
 */


#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"

#include "../chain_Acion/analyzer.h"

#include <iostream>
#include <vector>
#include<cassert>
#include <fstream>
using namespace std;
SkillType NaoBehavior::getDefensiveSkill() {
		analyzer->generateDefensiveSkills();
		skilldesc skilltarg = analyzer->getTopSkill();
		SkillType ret;

		 if (skilltarg.getType() == SKILL_WALK_OMNI
				|| skilltarg.getType() == SKILL_INTERCEPT) {
			analyzer->resetCandidates();
			/*if(worldModel->getRole(worldModel->getUNum()) < 2) {
			 if(skilltarg.getTarget().getDistanceTo(loader->getDuePosition(worldModel->getUNum())) > 5
			 && skilltarg.getTarget().getX() > loader->getDuePosition(worldModel->getUNum()).getX()+3
			 )
			 return SKILL_STAND;
			 }*/
			double distance, angle, targ;
			getTargetDistanceAndAngle(skilltarg.getTarget(), distance, angle);
			/*	if (abs(angle) > 10) {
			 return goToTargetRelative(VecPosition(), angle);
			 } else */

			if (distance > 0.2) {
			return goToTarget(skilltarg.getTarget());
			}
//			} else if (skilltarg.getType() == SKILL_INTERCEPT){
//				cout << "IN CONDITION" << endl;
//					if (worldModel->getMyPosition().getX() > -12
//							|| fabs(worldModel->getMyAngDeg()) < 90) {
//				/*VecPosition target = (worldModel->getBall() - worldModel->getMyPosition())*1.5;
//				 target += worldModel->getMyPosition();
//				 ret = kickBall(KICK_DRIBBLE, skilltarg.getTarget());
//
//				 analyzer->resetCandidates();
//				 return ret;*/
//
//				return intercept();
//				}}
			else if (worldModel->getMyPosition().getDistanceTo(
					loader->getDuePosition(worldModel->getUNum())) < 0.5) {
				double angle;
				VecPosition ball = worldModel->getBall();
				double dis;
				getTargetDistanceAndAngle(ball, dis, angle);
				return goToTargetRelative(VecPosition(), angle);
			}

			return SKILL_STAND;

		 }
		 else
		 		analyzer->resetCandidates();

		 return SKILL_STAND;
}

