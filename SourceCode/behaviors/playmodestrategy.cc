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
SkillType NaoBehavior::getPlayModeSkill() {
	if ((((worldModel->getSide() == SIDE_LEFT)
			&& (worldModel->getPlayMode() == PM_KICK_OFF_LEFT))
			|| ((worldModel->getSide() == SIDE_RIGHT)
					&& (worldModel->getPlayMode() == PM_KICK_OFF_RIGHT))))
		{//	analyzer->resetCandidates();
		//	analyzer->generatePlayers();
		//	skilldesc top = analyzer->getTopSkill();
			if (11 == worldModel->getUNum()){
					return kickBall(KICK_FORWARD, *new VecPosition(16,0,0));

			}

			return SKILL_STAND;
		} else if(worldModel->getPlayMode() == PM_KICK_OFF_RIGHT || worldModel->getPlayMode() == PM_KICK_OFF_LEFT)
		{
			return SKILL_STAND;
		}else if (worldModel->getSide() == SIDE_RIGHT) {
			if (worldModel->getPlayMode() == PM_KICK_IN_LEFT
					|| worldModel->getPlayMode() == PM_CORNER_KICK_LEFT
					|| worldModel->getPlayMode() == PM_FREE_KICK_LEFT
					|| worldModel->getPlayMode() == PM_GOAL_KICK_LEFT
					||worldModel->getPlayMode() == PM_DIRECT_FREE_KICK_LEFT){
				if(worldModel->getBall().getDistanceTo(worldModel->getMyPosition()) < 3 )
					return SKILL_STAND;
				return getDefensiveSkill();
			}
			if(worldModel->getPlayMode() == PM_GOAL_KICK_RIGHT){
				if(worldModel->getUNum()!=1){
					return goToTarget(VecPosition(worldModel->getMyPosition().getX()+1,worldModel->getMyPosition().getY(),0));
				}
			}
			return getAttackSkill();

		} else if (worldModel->getSide() == SIDE_LEFT) {

			if (worldModel->getPlayMode() == PM_KICK_IN_LEFT
					|| worldModel->getPlayMode() == PM_CORNER_KICK_LEFT
					|| worldModel->getPlayMode() == PM_FREE_KICK_LEFT
						|| worldModel->getPlayMode() == PM_GOAL_KICK_LEFT
						||worldModel->getPlayMode() == PM_DIRECT_FREE_KICK_LEFT)
				return getAttackSkill();
			if(worldModel->getBall().getDistanceTo(worldModel->getMyPosition()) < 3 )
								return SKILL_STAND;

			return getDefensiveSkill();

		}
	return SKILL_STAND;
}
