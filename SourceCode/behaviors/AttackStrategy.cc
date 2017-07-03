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
SkillType NaoBehavior::getAttackSkill() {

	if ((((worldModel->getSide() == SIDE_LEFT)
			&& (worldModel->getPlayMode() == PM_KICK_OFF_LEFT))
			|| ((worldModel->getSide() == SIDE_RIGHT)
					&& (worldModel->getPlayMode() == PM_KICK_OFF_RIGHT)))
			&& (worldModel->getTeammateClosestTo(worldModel->getBall())
					== worldModel->getUNum())) {
		return kickBall(KICK_FORWARD,
				((worldModel->getMyLeftGoalPost()
						+ worldModel->getMyRightGoalPost()) / 2));
	}
	if (analyzer->getTopSkill().getType() == SKILL_PASS
			&& worldModel->getBall().getDistanceTo(worldModel->getMyPosition())
					> 1) {
		analyzer->resetCandidates();

	}
//	if (analyzer->getTopSkill().getType() == SKILL_DRIBBLE
//			&& worldModel->getBall().getDistanceTo(worldModel->getMyPosition())
//					>= worldModel->getBall().getDistanceTo(analyzer->getTopSkill().getTarget())
//					) {
	analyzer->generateCanditates();
		skilldesc skilltarg = analyzer->getTopSkill();

	if((drib == true && worldModel->getBall().getDistanceTo(worldModel->getMyPosition()) > 0.3)
		||(skilltarg.getType() == SKILL_DRIBBLE && worldModel->getBall().getDistanceTo(worldModel->getMyPosition()) > 2)	)
		{
		analyzer->resetCandidates();
	//	cout<<"Clear dribble\n";
		drib = false;
	}
	else if(drib == false && skilltarg.getType() == SKILL_DRIBBLE){
		drib = (worldModel->getBall().getDistanceTo(worldModel->getMyPosition()) <= 0.5);
	//	cout<<"Start Dribble"<<worldModel->getBall().getDistanceTo(worldModel->getMyPosition())<<"\n";
	}
if(skilltarg.getType() != SKILL_DRIBBLE)
	drib = false;

	SkillType ret;

	if (skilltarg.getType() == SKILL_PASS) {
		//cout<<"SKILL = SKILL_PASS"<<endl;
		if (worldModel->getBall().getDistanceTo(skilltarg.getTarget()) > 5)
			ret = kickBall(KICK_FORWARD, skilltarg.getTarget());
		else
			ret = kickBall(KICK_IK, skilltarg.getTarget());
		//if (ret != SKILL_STAND && ret != SKILL_WALK_OMNI)
		analyzer->resetCandidates();
		return ret;
	} else if (skilltarg.getType() == SKILL_WALK_OMNI
			|| skilltarg.getType() == SKILL_INTERCEPT) {
		//cout<<"SKILL = SKILL_WALK_OMNI"<<endl;
		analyzer->resetCandidates();

		double distance, angle, targ;
		getTargetDistanceAndAngle(skilltarg.getTarget(), distance, angle);

		if (distance > 0.2) {
			return goToTarget(skilltarg.getTarget());

		}  else if (worldModel->getMyPosition().getDistanceTo(
				loader->getDuePosition(worldModel->getUNum())) < 0.5) {
			double angle;
			VecPosition ball = worldModel->getBall();
			double dis;
			getTargetDistanceAndAngle(ball, dis, angle);
			return goToTargetRelative(VecPosition(), angle);
		}
		return SKILL_STAND;

	} else if (skilltarg.getType() == SKILL_SHOOT) {
		//cout<<"SKILL = SKILL_KICK_LEFT_LEG"<<endl;
		ret = kickBall(KICK_FORWARD, skilltarg.getTarget());
		analyzer->resetCandidates();
		return ret;
	} else if (skilltarg.getType() == SKILL_DRIBBLE) {
		ret = kickBall(KICK_DRIBBLE, skilltarg.getTarget());

		//analyzer->resetCandidates();
		return ret;
	} else
		analyzer->resetCandidates();

	return SKILL_STAND;
}

