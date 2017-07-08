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
//	if(1-WO_TEAMMATE1+worldModel->getUNum() == 11)
//	return kickBall(KICK_FORWARD,
//					((worldModel->getOppLeftGoalPost()
//							+ worldModel->getOppRightGoalPost()) / 2));
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
	return SKILL_STAND;
}
