
#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"

#include "../chain_Acion/analyzer.h"
#include "../formations/formation.h"
#include <iostream>
#include <vector>
#include<cassert>
#include <fstream>
#define ATT 3
#define MID 2
#define DEF 1

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

	if((CBall - ball).getMagnitude() <0.3)
		return  kickBall(KICK_DRIBBLE,tar);
	VecPosition lball  = worldModel->g2l(ball);
	tar = lball+VecPosition(1,ang,0,POLAR);
	CBall = ball;
	tar =worldModel->l2g(tar);
	return  kickBall(KICK_DRIBBLE, tar);

}
bool OppInRegion(){
	return false;
}
SkillType NaoBehavior::selectSkill() {
//	VecPosition T_HalfG = (worldModel->getOppLeftGoalPost()+worldModel->getOppRightGoalPost())/2;
//	VecPosition O_HalfG = (worldModel->getMyLeftGoalPost()+worldModel->getMyRightGoalPost())/2;
//	cout << "State = ";
//	if(loader->getTeamState()==0)
//		cout << "Attacking ";
//		else if(loader->getTeamState()==1)
//		cout << "DEFENDING ";
//		else if(loader->getTeamState()==2)
//		cout << "ATTDEFEND ";
//		else if(loader->getTeamState()==3)
//		cout << "DEFATTACK ";
//
//	cout << loader->GetPrev() << endl;
//	if(worldModel->getUNum() == worldModel->getTeammateClosestTo(ball)){
//	if(ball.getX()>=0 && (loader->GetPrev()=="ATT"||loader->GetPrev()=="HALF"))
//		return kickBall(KICK_FORWARD, O_HalfG);
//	else
//		return kickBall(KICK_FORWARD, T_HalfG);}
//	else
//	return SKILL_STAND;

//	int Opp = worldModel->getOpponentClosestTo(ball)+WO_OPPONENT1-1;
//	VecPosition target;
//	target = ball
//						+ *(new VecPosition(-1.5,Deg2Rad(worldModel->getWorldObject(Opp)->orien)
//								,0, POLAR));
//	cout << "Ball Pos = " << ball.getX() << " " << ball.getY() << endl;
//			cout << "New POSSSSSS = " << target.getX() << " " << target.getY() << endl;
//return SKILL_STAND;
			//	if(worldModel->getUNum() == 5){
//		cout << "Prev X = " << me.getX() << "Prev Y = " << me.getY() << endl;
//		VecPosition newone = me+(*(new VecPosition(0.4,0,0,POLAR)));
//		cout << "New X = " << newone.getX() << "New Y = " << newone.getY() << endl;
//	}
//	return SKILL_STAND;
//	int i = WO_OPPONENT1 - 1 + worldModel->getOpponentClosestTo(ball);
//		VecPosition v = worldModel->getMyRightGoalPost() + worldModel->getMyLeftGoalPost();
//		v /= 2;
//		double ang = worldModel->getWorldObject(i)->orien;
//		cout << ang << endl;
//		return SKILL_STAND;
//	double AngleWithOpp = worldModel->getTeammate(5).getAngleWithVector(ball);
//	cout << Angle(InputServer) CreateDevice Keyboard

//	return SKILL_STAND;
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
//			+ *(new VecPosition(-1.0, -180//Deg2Rad(worldModel->getWorldObject(Opp)->orien)
//					,0, POLAR));
//			//VecPosition Intersection_pt = Point.getVecPositionFromPolar(Point.getX()*1.8,Point.getY(),Point.getZ());
//				cout << "Opp Pos " << OppPos.getX() << " " << OppPos.getY() << endl;
//				cout <<  "Estimated Position " << target.getX() << " " << target.getY() << endl;
//
//		return SKILL_STAND;

	//cout << "My Dist = " << MeDisToBall << endl;
	//cout << "oppDissssssssssssss = " << oppDis << endl;

	if (worldModel->getPlayMode() != PM_PLAY_ON) {
		return getPlayModeSkill();
	}
	bool ballHolder = false;

	int role = worldModel->getRole(worldModel->getUNum()-1);
	int GameState = loader->getTeamState();
	if(me.getDistanceTo(ball)<0.3)
		ballHolder = true;

	if (ballHolder)
		return getAttackSkill();

	if(role == ATT){
		switch (GameState)
		{
		case ATTACKING:
			getAttackSkill();
			break;
		case ATTDEFEND:
			getAttackSkill();
			break;
		case DEFENDING:
			if(OppInRegion())
				getDefensiveSkill();
			else
				getAttackSkill();
			break;
		case DEFATTACK:
			getAttackSkill();
			break;
		default : return SKILL_STAND;
		}
	}

	else if(role == MID){

		switch (GameState)
		{
		case ATTACKING:
			getAttackSkill();
			break;

		case ATTDEFEND:
			if(OppInRegion())
				getDefensiveSkill();
			else
				getAttackSkill();
			break;

		case DEFENDING:
				getDefensiveSkill();
			break;

		case DEFATTACK:
			if (OppInRegion())
				getDefensiveSkill();
			else
				getAttackSkill();
			break;
		default : return SKILL_STAND;

		}
	}

	else if (role == DEF) {

		switch (GameState) {
		case ATTACKING:
			if (OppInRegion())
				getDefensiveSkill();
			else
				getAttackSkill();
			break;

		case ATTDEFEND:
			getDefensiveSkill();
			break;

		case DEFENDING:
			getDefensiveSkill();
			break;

		case DEFATTACK:
			getDefensiveSkill();
			break;
		default:
			return SKILL_STAND;

		}

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
