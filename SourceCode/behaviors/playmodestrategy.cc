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
		{if ((worldModel->getTeammateClosestTo(worldModel->getBall())
				== worldModel->getUNum()))
		//	analyzer->resetCandidates();
		//	analyzer->generatePlayers();
		//	skilldesc top = analyzer->getTopSkill();
			if (worldModel->getTeammateClosestTo(worldModel->getBall()) == worldModel->getUNum()){
				if(worldModel->getUNum() != 10){
				return kickBall(KICK_IK, loader->getBeamingPosition(10));
				}
			return kickBall(KICK_IK, loader->getBeamingPosition(9));
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
