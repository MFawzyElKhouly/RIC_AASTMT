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
#include "../chain_Acion/skillpass.h"
using namespace std;

bool drib = false;
SkillType NaoBehavior::getAttackSkill() {


	if (analyzer->getTopSkill().getType() == SKILL_PASS
			&& (worldModel->getTeammateClosestTo(worldModel->getBall())
					!= worldModel->getUNum()
					|| worldModel->getBall().getDistanceTo(
							worldModel->getMyPosition()) > 1)) {
		analyzer->resetCandidates();

	}
//	if (analyzer->getTopSkill().getType() == SKILL_DRIBBLE
//			&& worldModel->getBall().getDistanceTo(worldModel->getMyPosition())
//					>= worldModel->getBall().getDistanceTo(analyzer->getTopSkill().getTarget())
//					) {
	//analyzer->generateCanditates();
	if (analyzer->skillset.empty())
		if (worldModel->getTeammateClosestTo(worldModel->getBall()) == worldModel->getUNum()) {
			analyzer->generateBallHolderSkills();
		}
		//double MeDisToBall = wm->getBall().getDistanceTo(worldModel->getTeammate(worldModel->getTeammateClosestTo(ball)+WO_TEAMMATE1-1));

		//	if ((loader->getTeamState() == ATTACKING)
		//			&& (wm->distanceToMyGoal(ball)) > 0) {
		else {

			//cout << "ATTAAAAAAACK" << endl;
			analyzer->generateAttackingSkills();

		}
	skilldesc skilltarg = analyzer->getTopSkill();
 //cout<<"  "<<skilltarg.getType()<<endl;
//	if((drib == true && worldModel->getBall().getDistanceTo(worldModel->getMyPosition()) > 0.3)
//		||(skilltarg.getType() == SKILL_DRIBBLE && worldModel->getBall().getDistanceTo(worldModel->getMyPosition()) > 2)	)
//		{
//		analyzer->resetCandidates();
//	//	cout<<"Clear dribble\n";
//		drib = false;
//	}
//	else if(drib == false && skilltarg.getType() == SKILL_DRIBBLE){
//		drib = (worldModel->getBall().getDistanceTo(worldModel->getMyPosition()) <= 0.5);
//	//	cout<<"Start Dribble"<<worldModel->getBall().getDistanceTo(worldModel->getMyPosition())<<"\n";
//	}
//if(skilltarg.getType() != SKILL_DRIBBLE)
//	drib = false;

	SkillType ret;

	if (skilltarg.getType() == SKILL_PASS) {
		//cout<<"SKILL = SKILL_PASS"<<endl;
			ret = kickBall(KICK_IK, skilltarg.getTarget());
		//if (ret != SKILL_STAND && ret != SKILL_WALK_OMNI)
		//analyzer->resetCandidates();
		return ret;
	} else if (skilltarg.getType() == SKILL_WALK_OMNI
			|| skilltarg.getType() == SKILL_INTERCEPT
			|| skilltarg.getType() == SKILL_SUPPORT) {
		//cout<<"SKILL = SKILL_WALK_OMNI"<<endl;
		analyzer->resetCandidates();
		VecPosition target = skilltarg.getTarget();
		if (worldModel->getTeammateClosestTo(worldModel->getBall())
				!= worldModel->getUNum())
			target = collisionAvoidance(true /*Avoid teamate*/,
					true /*Avoid opponent*/, true /*Avoid ball*/, .5, .5,
					target, false /*fKeepDistance*/);
		double distance, angle, targ;
		getTargetDistanceAndAngle(target, distance, angle);

		if (distance > 0.2) {
			return goToTarget(skilltarg.getTarget());

		} else if (worldModel->getMyPosition().getDistanceTo(
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
		//dribble sktemp = (;

		ret = dribbleAng((skilltarg).angle);
		//cout<<"Dribb\n";
		analyzer->resetCandidates();
		return ret;
	} else
		analyzer->resetCandidates();

	return SKILL_STAND;
}

