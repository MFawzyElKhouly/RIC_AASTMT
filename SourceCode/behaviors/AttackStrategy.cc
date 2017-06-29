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

	/*if(worldModel->getUNum() == worldModel->getTeammateClosestTo(worldModel->getBall()))
		return kickBall(KICK_FORWARD,
				((worldModel->getOppLeftGoalPost()
						+ worldModel->getOppRightGoalPost()) / 2));*/
	analyzer->generateCanditates();
		skilldesc skilltarg = analyzer->getTopSkill();
		SkillType ret;

		if (analyzer->getTopSkill().getType() == SKILL_PASS
					&& worldModel->getBall().getDistanceTo(worldModel->getMyPosition())
							> 1) {
				analyzer->resetCandidates();

			}
			if (analyzer->getTopSkill().getType() == SKILL_DRIBBLE
					&& worldModel->getBall().getDistanceTo(worldModel->getMyPosition())
							> 0.8) {
				analyzer->resetCandidates();

			}

			if (skilltarg.getType() == SKILL_PASS) {
					//cout<<"SKILL = SKILL_PASS"<<endl;
					if (worldModel->getBall().getDistanceTo(skilltarg.getTarget()) > 5)
						ret = kickBall(KICK_FORWARD, skilltarg.getTarget());
					else
						ret = kickBall(KICK_IK, skilltarg.getTarget());
					//if (ret != SKILL_STAND && ret != SKILL_WALK_OMNI)
					//analyzer->resetCandidates();
					return ret;
				}
			else if (skilltarg.getType() == SKILL_SHOOT) {
					//cout<<"SKILL = SKILL_KICK_LEFT_LEG"<<endl;
					ret = kickBall(KICK_FORWARD, skilltarg.getTarget());
					analyzer->resetCandidates();
					return ret;
				} else if (skilltarg.getType() == SKILL_DRIBBLE) {
					ret = kickBall(KICK_DRIBBLE, skilltarg.getTarget());

					//analyzer->resetCandidates();
					return ret;
				}
				else
						analyzer->resetCandidates();

			return SKILL_STAND;

}

