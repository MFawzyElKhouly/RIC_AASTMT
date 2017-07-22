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
	goalLineStart.setX(-14.8);
	goalLineStart.setY(-1.05);
	goalLineEnd.setX(-14.8);
	goalLineEnd.setY(1.05);
	f = 0;
	t = 0;
	notYet = false;
	endIt = false;
	counter = 0;
	times = 0.0;
	t1 = 0.0;
	t2 = 0.0;
	p1 = VecPosition(0);
	ballv = 0.0;

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
	if ((distance / ballv) <= 1) {
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
	if ((worldModel->getMyPosition().getX() > -11)
			|| (worldModel->getMyPosition().getY() > 5)
			|| (worldModel->getMyPosition().getY() < -5))
		return goToTarget(VecPosition(-14.5, 0, 0));
	if ((worldModel->getPlayMode() == PM_GOAL_KICK_LEFT
			&& worldModel->getSide() == SIDE_LEFT)
			|| (worldModel->getPlayMode() == PM_GOAL_KICK_RIGHT
					&& worldModel->getSide() == SIDE_RIGHT))
		return kickBall(KICK_FORWARD, VecPosition(0, 0, 0));
	VecPosition LeftPost = worldModel->getMyLeftGoalPost();
	VecPosition RightPost = worldModel->getMyRightGoalPost();
	int BallHolder = worldModel->getBallHolder();
	VecPosition BallHolderpos = worldModel->getWorldObject(BallHolder)->pos;
	VecPosition Middelgoal = (RightPost + LeftPost) / 2.0;
	VecPosition ballpre =  worldModel->predictBall(0.7);
	vector<double> Points(7), Ranges(8);
	double LimitY = 15 - abs(ball.getX()) * 10.0 / 15.0;
	double range = LimitY * 2.0 / 7.0;
	for (int i = 0; i < 7; i++) {
		Points[i] = fabs(RightPost.getY() - LeftPost.getY()) / 6.0 * i
				+ RightPost.getY();
		Ranges[i] = -LimitY + range * i;
	cout  << Ranges[i] << endl;
	}
	Ranges[7] = LimitY;

	if (ball.getDistanceTo(Middelgoal) < 3)
	{
		//VecPosition totar =
		return kickBall(KICK_IK, VecPosition(-11, 4, 0.0));
	}
		//if (ball.getDistanceTo(BallHolder) > 1.5 )
		//return SKILL_SPLIT;
		if (ball.getY() > Ranges[0] && ball.getY() < Ranges[1])
		return goToTarget(VecPosition(-14.25, Points[0], 0));
		else if (ball.getY() > Ranges[1] && ball.getY() < Ranges[2])
		return goToTarget(VecPosition(-14.25, Points[1], 0));
		else if (ball.getY() > Ranges[2] && ball.getY() < Ranges[3])
		return goToTarget(VecPosition(-14.25, Points[2], 0));
		else if (ball.getY() > Ranges[3] && ball.getY() < Ranges[4])
		return goToTarget(VecPosition(-14.25, Points[3], 0));
		else if (ball.getY() > Ranges[4] && ball.getY() < Ranges[5])
		return goToTarget(VecPosition(-14.25, Points[4], 0));
		else if (ball.getY() > Ranges[5] && ball.getY() < Ranges[6])
		return goToTarget(VecPosition(-14.25, Points[5], 0));
		else if (ball.getY() > Ranges[6] && ball.getY() < Ranges[7])
		return goToTarget(VecPosition(-14.25, Points[6], 0));
		else
		return SKILL_STAND;
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
