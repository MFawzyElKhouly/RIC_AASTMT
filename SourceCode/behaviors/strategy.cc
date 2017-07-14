#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"

#include "../chain_Acion/analyzer.h"

#include <iostream>
#include <vector>
#include<cassert>
#include <fstream>
using namespace std;

vector<VecPosition> sttaics;
bool start = 0;
bool printed = 0;
void add(double x, double y, double z = 0, vector<VecPosition> &v = sttaics) {
	v.push_back(*(new VecPosition(x, y, z)));
}

void load(string filename) {

	sttaics.clear();
	fstream of(filename.c_str(), ios_base::in);
	double x, y;
	while (of >> x >> y)
		add(x, y);
	of.close();
}
/*
 * Real game beaming.
 * Filling params x y angle
 */
void NaoBehavior::beam(double& beamX, double& beamY, double& beamAngle) {

	loader->setStrategy("second");
	VecPosition beamer = loader->getBeamingPosition(this->agentUNum);
	beamX = beamer.getX();
	beamY = beamer.getY();
	beamAngle = beamer.getZ();

}
int i = 0;
VecPosition CBall;
VecPosition tar;
SkillType NaoBehavior::dribbleAng(double ang) {
	if(ball.getDistanceTo(worldModel->getMyPosition()) > 0.6)
				return goToTarget(ball);

	if((CBall - ball).getMagnitude() <0.1)
		return  kickBall(KICK_DRIBBLE,tar);
	VecPosition lball  = worldModel->g2l(ball);
	tar = lball+VecPosition(1,ang,0,POLAR);
	CBall = ball;
	tar =worldModel->l2g(tar);
	return  kickBall(KICK_DRIBBLE, tar);

}
SkillType NaoBehavior::selectSkill() {

	if(worldModel->getUNum() == 5){
	double AngleWithOpp = me.getAngleWithVector(worldModel->getOpponent(worldModel->getOpponentClosestTo(ball)));
	cout << AngleWithOpp << endl;
	}
	return SKILL_STAND;
//	if(worldModel->getUNum() !=11)
//		return SKILL_STAND;
//	else {
//
//		return dribbleAng(z);
//
//	}
//	int Opp = worldModel->getOpponentClosestTo(ball)+WO_OPPONENT1-1;
//	VecPosition OppPos = worldModel->getOpponent(Opp);
//	VecPosition target = worldModel->getOpponent(Opp)
//			+ *(new VecPosition(-1.0, Deg2Rad(worldModel->getWorldObject(Opp)->orien),
//									0, POLAR));
//			//VecPosition Intersection_pt = Point.getVecPositionFromPolar(Point.getX()*1.8,Point.getY(),Point.getZ());
//				cout << "Opp Pos " << OppPos.getX() << " " << OppPos.getY() << endl;
//				cout <<  "Estimated Position " << target.getX() << " " << target.getY() << endl;

		//return SKILL_STAND;
	double oppDis = ball.getDistanceTo(worldModel->getOpponent(worldModel->getOpponentClosestTo(ball)+WO_OPPONENT1-1));
		//cout << "Player " << wm->getUNum() << endl;
	double MeDisToBall = ball.getDistanceTo(worldModel->getTeammate(worldModel->getTeammateClosestTo(ball)+WO_TEAMMATE1-1));
	if(worldModel->getTeammateClosestTo(ball) == worldModel->getUNum() && me.getDistanceTo(ball)<0.3){
	for(int i=WO_OPPONENT1;i<=WO_OPPONENT11;i++){
		VecPosition opp = worldModel->getOpponent(i);
		double DisToOpp = me.getDistanceTo(opp);
		int AngleWithOpp = me.getAngleWithVector(opp);
		//cout << "Me " << AngleWithOpp << endl;
		if(DisToOpp < 0.25 && fabs(AngleWithOpp) < 5){
			//cout << "Angle With Opponent = " << AngleWithOpp << endl;
			VecPosition vec = me.getVecPositionFromPolar(1,me.getTheta(),0);
			//cout << "Ball Position : " << ball.getX() << " " << ball.getY() << " " << ball.getZ() << endl;
			//cout << "Target Position : " << vec.getX() << " " << vec.getY() << " " << vec.getZ() << endl;
			return intercept();
			}
		}
	}
	if (worldModel->getPlayMode() != PM_PLAY_ON) {
		return getPlayModeSkill();
	}

	if (MeDisToBall < oppDis || worldModel->getTeammateClosestTo(ball) == worldModel->getUNum()) {

	//cout << "ATT STATEEEEEEEEEEEEEEE" << endl;
		return getAttackSkill();
	}
	else{
		//cout << "DEF" << endl;
		return getDefensiveSkill();
	}

	return SKILL_STAND;
}


/*
 * Demo behavior where players form a rotating circle and kick the ball
 * back and forth
 */
SkillType NaoBehavior::demoKickingCircle() {
	// Parameters for circle
	VecPosition center = VecPosition(-HALF_FIELD_X / 2.0, 0, 0);
	double circleRadius = 5.0;
	double rotateRate = 2.5;

	// Find closest player to ball
	int playerClosestToBall = -1;
	double closestDistanceToBall = 10000;
	for (int i = WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; ++i) {
		VecPosition temp;
		int playerNum = i - WO_TEAMMATE1 + 1;
		if (worldModel->getUNum() == playerNum) {
			// This is us
			temp = worldModel->getMyPosition();
		} else {
			WorldObject* teammate = worldModel->getWorldObject(i);
			if (teammate->validPosition) {
				temp = teammate->pos;
			} else {
				continue;
			}
		}
		temp.setZ(0);

		double distanceToBall = temp.getDistanceTo(ball);
		if (distanceToBall < closestDistanceToBall) {
			playerClosestToBall = playerNum;
			closestDistanceToBall = distanceToBall;
		}
	}

	if (playerClosestToBall == worldModel->getUNum()) {
		// Have closest player kick the ball toward the center
		analyzer->generateCanditates();
		skilldesc skilltarg = analyzer->getTopSkill();
		if (skilltarg.getType() == SKILL_PASS) {
			//cout<<"shooting";
			return kickBall(KICK_IK, skilltarg.getTarget());
		} else if (skilltarg.getType() == SKILL_WALK_OMNI) {
			return goToTarget(skilltarg.getTarget());
		} else
			//return goToTargetRelative(VecPosition(1,0,0), 0);
			return SKILL_STAND;
		return kickBall(KICK_FORWARD, center);
	} else {
		// Move to circle position around center and face the center
		VecPosition localCenter = worldModel->g2l(center);
		SIM::AngDeg localCenterAngle = atan2Deg(localCenter.getY(),
				localCenter.getX());

		// Our desired target position on the circle
		// Compute target based on uniform number, rotate rate, and time
		VecPosition target = center
				+ VecPosition(circleRadius, 0, 0).rotateAboutZ(
						360.0 / (NUM_AGENTS - 1)
								* (worldModel->getUNum()
										- (worldModel->getUNum()
												> playerClosestToBall ? 1 : 0))
								+ worldModel->getTime() * rotateRate);

		// Adjust target to not be too close to teammates or the ball
		target = collisionAvoidance(true /*teammate*/, false/*opponent*/,
				true/*ball*/, 1/*proximity thresh*/, .5/*collision thresh*/,
				target, true/*keepDistance*/);

		if (me.getDistanceTo(target) < .25 && abs(localCenterAngle) <= 10) {
			// Close enough to desired position and orientation so just stand
			return SKILL_STAND;
		} else if (me.getDistanceTo(target) < .5) {
			// Close to desired position so start turning to face center
			return goToTargetRelative(worldModel->g2l(target), localCenterAngle);
		} else {
			// Move toward target location
			return goToTarget(target);
		}
	}
}

