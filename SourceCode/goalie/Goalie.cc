#include "Goalie.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cctype>
#include <exception>

#include "../skills/skillparser.h"
#include "../rvdraw/rvdraw.h"
#include <assert.h>

#include <common/InterfaceInfo.h>
#include <motion/MotionModule.h>

#include <common/InterfaceInfo.h>
#include <motion/MotionModule.h>

extern int agentBodyType;

Goalie::Goalie(const std::string teamName, int uNum,
		const map<string, string>& namedParams_, const string& rsg_) :
		NaoBehavior(teamName, uNum, namedParams_, rsg_) {
	target = VecPosition(-14.5, 0, 0);
}

Goalie::~Goalie() {

	delete parser;
	delete worldModel;
	delete bodyModel;
	delete particleFilter;
	delete core;
}

double Goalie::getMagnitude(double p1, double q1, double p2, double q2) {
	return sqrt((p2 - p1) * (p2 - p1) + (q2 - q1) * (q2 - q1));
}

bool Goalie::splitDecision(double distance) {
	if ((distance) <= 1) {
		return 1;
	}
	return 0;
}

bool Goalie::canStand() {
	//if (worldModel->getMyPosition().getZ() > 0.04)
	//	return false;
	//else if (worldModel->getLastBallSeenPosition()[0] != worldModel->getBall())
	return true;
	//return true;
}

SkillType Goalie::selectSkill() {
	VecPosition LeftPost = worldModel->getMyLeftGoalPost();
	VecPosition RightPost = worldModel->getMyRightGoalPost();
	int BallHolder = worldModel->getBallHolder();
	VecPosition BallHolderpos = worldModel->getWorldObject(BallHolder)->pos;
	VecPosition Middelgoal = (RightPost + LeftPost) / 2.0;
	vector<double> Points(7), Ranges(8);
	double pX = 3.0, pY = 0.75;
	double LimitY = 15 - abs(ball.getX()) * 10.0 / 15.0;
	double range = LimitY * 2.0 / 7.0;
	// for foul and out of range
	if ((worldModel->getMyPosition().getX() > -9)
			|| (worldModel->getMyPosition().getY() > 6)
			|| (worldModel->getMyPosition().getY() < -6)) {
		target = VecPosition(-14.5, 0, 0);
		target = collisionAvoidance(true, true, false, .5, .5, target, false);
		return goToTarget(target);
	}

// for playmode handling
	if ((worldModel->getPlayMode() == PM_GOAL_KICK_LEFT
			&& worldModel->getSide() == SIDE_LEFT)
			|| (worldModel->getPlayMode() == PM_GOAL_KICK_RIGHT
					&& worldModel->getSide() == SIDE_RIGHT)) {
		target = VecPosition(-14.5, 0, 0);
		if (me.getDistanceTo(target) > 1)
			return goToTarget(target);
		else
			return kickBall(KICK_FORWARD, VecPosition(0, 0, 0));
	}

	timeforGK = worldModel->getTime();
	// generating points and ranges for positioning
	for (int i = 0; i < 7; i++) {
		Points[i] = fabs(RightPost.getY() - LeftPost.getY()) / 6.0 * i
				+ RightPost.getY();
		Ranges[i] = -LimitY + range * i;
	}
	Ranges[7] = LimitY;
	cout << worldModel->getWorldObject(WO_BALL)
	if (BallHolder > 11)
		cout << WorldObjType2Str[BallHolder - 13 + WO_OPPONENT_FOOT_L1] << " = "
				<< worldModel->getWorldObject(
						BallHolder - 12 + WO_OPPONENT_FOOT_L1)->pos
				<< endl;
	cout << WorldObjType2Str[BallHolder - 13 + WO_OPPONENT_FOOT_R1] << " = "
			<< worldModel->getWorldObject(BallHolder - 12 + WO_OPPONENT_FOOT_R1)->pos
			<< endl;
		if (BallHolder > 11
	 && worldModel->getWorldObject(BallHolder - 12 + WO_OPPONENT_FOOT_L1)->pos.getZ()
	 > 0.5)
	 {
			cout << "byshoot ?? " << BallHolder - 12 + WO_OPPONENT_FOOT_L1 << endl;

	 }
	if (ball.getDistanceTo(Middelgoal) <= 4.5
			|| (BallHolder == worldModel->getUNum()
					&& ball.getDistanceTo(Middelgoal) <= 5)) {
		double newX = ball.getX() + pX, newY = ball.getY() + pY;
		if (abs(newX - oldX) < 0.2)
			newX = oldX;
		if (abs(newY - oldY) < 0.2)
			newY = oldY;
		if (me.getDistanceTo(ball) > 0.5) {
			target = ball;
			target.setZ(0);
			target.setX(target.getX()-0.5);
			target = collisionAvoidance(true, true, false, .5, .5, target,
					false);
			return goToTarget(target);
		}
		oldX = newX;
		oldY = newY;
		return kickBall(KICK_IK, VecPosition(newX, newY, 0));
	}
	/*	if ((BallHolderpos.getDistanceTo(Middelgoal) <
	 || BallHolderpos.getDistanceTo(me) < 3.5) && BallHolder > 11)
	 return goToTarget((BallHolderpos + Middelgoal) / 2.0);
	 */
	if (ball.getDistanceTo(Middelgoal) > 5) {
		if (ball.getY() > Ranges[0] && ball.getY() < Ranges[1]) {
			pX = 0.75;
			pY = -3.0;
			return goToTarget(VecPosition(-14.25, Points[0], 0));
		} else if (ball.getY() > Ranges[1] && ball.getY() < Ranges[2]) {
			pX = 1.5;
			pY = -2.25;
			return goToTarget(VecPosition(-14.25, Points[1], 0));
		} else if (ball.getY() > Ranges[2] && ball.getY() < Ranges[3]) {
			pX = 2.25;
			pY = -1.5;
			return goToTarget(VecPosition(-14.25, Points[2], 0));
		} else if (ball.getY() > Ranges[3] && ball.getY() < Ranges[4]) {
			pX = 3.0;
			pY = 0.75;
			return goToTarget(VecPosition(-14.25, Points[3], 0));
		} else if (ball.getY() > Ranges[4] && ball.getY() < Ranges[5]) {
			pX = 2.25;
			pY = 1.5;
			return goToTarget(VecPosition(-14.25, Points[4], 0));
		} else if (ball.getY() > Ranges[5] && ball.getY() < Ranges[6]) {
			pX = 1.5;
			pY = 2.25;
			return goToTarget(VecPosition(-14.25, Points[5], 0));
		} else if (ball.getY() > Ranges[6] && ball.getY() < Ranges[7]) {
			pX = 0.75;
			pY = 3.0;
			return goToTarget(VecPosition(-14.25, Points[6], 0));
		} else
			return SKILL_STAND;
	}
	return SKILL_STAND;

	// for foul and out of range
	/*
	 */
	/*
	 bodyModel->setTargetAngle(EFF_LA1,120);
	 bodyModel->setTargetAngle(EFF_RA1,120);

	 bodyModel->setTargetAngle(EFF_LA2,
	 atof(namedParams.find("getup_parms_stateDown3A2")->second.c_str()));
	 bodyModel->setTargetAngle(EFF_RA2,
	 atof(namedParams.find("getup_parms_stateDown3A2")->second.c_str()));
	 return SKILL_STAND;*/

	//if (ball.getDistanceTo(BallHolder) > 1.5 )
	//return SKILL_SPLIT;
	/*	//cout<<ballPos<<endl;
	 double ballangle, dis, r, t;
	 double x, y;
	 x = (ballPos.getX() - 14.5) / 2;
	 y = ballPos.getY() / 2;
	 getTargetDistanceAndAngle(ball, dis, ballangle);

	 double mag = getMagnitude(x, y, -14.5, 0);

	 if (mag > 2) {
	 x = -14.5 + (0.5 / mag) * (ballPos.getX() + 14.5);
	 y = (0.5 / mag) * (ballPos.getY());
	 //double slope = -ballPos.getY() /(-14.5-ballPos.getX());
	 // cerr<<ballPos.getY()<<" "<<ballPos.getY()<<" ";
	 //cerr<<slope<<endl;
	 //y=(-13.5-ballPos.getX())*slope+ballPos.getY();

	 }

	 double x1 = worldModel->getWorldObject(ballHolder)->pos.getX();
	 double y1 = worldModel->getWorldObject(ballHolder)->pos.getY();
	 double orien = worldModel->getWorldObject(ballHolder)->orien;
	 if (orien < 0)
	 orien += 360;
	 orien -= 90;
	 double slope = tan(orien);
	 double c = x1 - slope * y1;
	 double x2 = -14.5;
	 double y2 = 0.0;
	 if (slope == 0.0);
	 else
	 y2 = (x2 - c) / slope;

	 if (y2 > -1 && y2 < 1) {
	 mag = getMagnitude(x1, y1, x2, y2);
	 x = x1 + (0.5 / mag) * (x2 - x1);
	 y = y1 + (0.5 / mag) * (y2 - y1);
	 }

	 if (worldModel->getBallHolder()
	 == worldModel->getUNum() + WO_TEAMMATE1 - 1) {

	 return kickBall(KICK_FORWARD,
	 ((worldModel->getOppLeftGoalPost()
	 + worldModel->getOppRightGoalPost()) / 2));

	 }
	 if (counter == 0)

	 {
	 if (fabs(ballangle) > 5) {
	 return goToTargetRelative(VecPosition(), ballangle);
	 }
	 if (fabs(x - worldModel->getMyPosition().getX()) > 0.2
	 || fabs(y - worldModel->getMyPosition().getY()) > 0.2)
	 return goToTarget(VecPosition(x, y, 0));
	 }
	 if (dis < 3) {
	 if (counter == 0) {
	 counter++;
	 return goToTarget(
	 VecPosition(worldModel->getMyPosition().getX(),
	 worldModel->getMyPosition().getY(), 0));
	 }
	 int i = worldModel->getOpponentClosestTo(worldModel->getBall())
	 + WO_OPPONENT1 - 1;

	 if ((counter >= 12)
	 && fabs(
	 (worldModel->getOpponent(i) - worldModel->getBall()).getAngleWithVector(
	 worldModel->getOpponent(i)
	 - worldModel->getMyPosition())) > 90) {
	 counter = 0;
	 return SKILL_STAND;
	 }
	 counter++;

	 //return SKILL_SPLIT;
	 }
	 counter = 0;
	 return goToTarget(
	 VecPosition(worldModel->getMyPosition().getX(),
	 worldModel->getMyPosition().getY(), 0));*/

}

bool Goalie::checkingFall() {
	if (canStand()) {
		return NaoBehavior::checkingFall();
	}
	return false;
}
