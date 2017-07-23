
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
bool NaoBehavior::OppInRegion(){
	VecPosition me = worldModel->getMyPosition();
	for (int i = WO_OPPONENT1;i<= WO_OPPONENT11;i++) {
		if (fabs(worldModel->getOpponent(i).getX() - me.getX()) < loader->getDueLength(worldModel->getUNum())/2)
			if (fabs(worldModel->getOpponent(i).getY() - me.getY()) < loader->getDueWidth(worldModel->getUNum())/2)
				return true;
	}
	return false;
}
bool NaoBehavior::Nearest(){
	int n = 0;
		int Mark = 0;
		VecPosition me = worldModel->getMyPosition();
		VecPosition Half_Goal = (worldModel->getMyLeftGoalPost()+worldModel->getMyRightGoalPost())/2;
			//int NrstLeftTeammate,NrstRightTeammate;
		//	NrstLeftTeammate = worldModel->getTeammateClosestTo((worldModel->getMyLeftGoalPost()+Half_Goal)/2)+WO_TEAMMATE1-1;
		//	NrstRightTeammate = worldModel->getTeammateClosestTo((worldModel->getMyRightGoalPost()+Half_Goal)/2)+WO_TEAMMATE1-1;
			VecPosition ball = worldModel->getBall();
		//int cnt = 0;
		double d = worldModel->distancetoBall(worldModel->getMyPosition());

		if (worldModel->getMyPosition().getX() > worldModel->getBall().getX() || worldModel->getFallenTeammate(worldModel->getUNum()) == true)
			//	&& d > 1.5)
			d +=50;


		for (int i = WO_TEAMMATE2; i <= WO_TEAMMATE11; i++) {// kam teammate a2rb ll kora

				if ((i - WO_TEAMMATE1 + 1) == (worldModel->getUNum() - WO_TEAMMATE1+1))
					continue;

		//		if(me.getX()>ball.getX() && Mark &&
		//						worldModel->getTeammateClosestTo(ball)==worldModel->getUNum()+WO_TEAMMATE1-1){
		//		//			double DisToOpp = worldModel->getTeammate(i).getDistanceTo(ball);
		//		//			int AngleWithBall = worldModel->getTeammate(i).getAngleWithVector(ball);
		//		//			int AngleWithGoal = worldModel->getTeammate(i).getAngleWithVector((worldModel->getOppLeftGoalPost()+worldModel->getOppRightGoalPost())/2);
		//					if(worldModel->getTeammate(i).getX()+1.0 < worldModel->getOpponent((worldModel->getOpponentClosestTo(ball)+WO_OPPONENT1-1)).getX()){
		//						Mark =0;
		//						cout << "Teammate Succ " << i << endl;
		//					}
		//				}
				double di = worldModel->distancetoBall(worldModel->getTeammate(i));
				if (worldModel->getTeammate(i).getX() > worldModel->getBall().getX() || worldModel->getFallenTeammate(i - WO_TEAMMATE1 + 1)){// && worldModel->distancetoBall(worldModel->getTeammate(i)) > 1.5)
				di+=50;
				Mark++;
				}

				if ((di < d) /*(di == d && i - (WO_TEAMMATE1 < worldModel->getUNum() - 1))*/
						&& worldModel->getWorldObject(i)->validPosition && worldModel->getFallenTeammate(i) == false
						//&& !worldModel->getFallenTeammate(i)
						) {
					n++;
				}
			}
		if(n==0 || n==1){
		//cout << worldModel->getUNum() << " " << n << endl;
			return true;	}

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

//		return SKILL_STAND;

	//cout << "My Dist = " << MeDisToBall << endl;
	//cout << "oppDissssssssssssss = " << oppDis << endl;

	if (worldModel->getPlayMode() != PM_PLAY_ON) {
		return getPlayModeSkill();
	}

	int role = worldModel->getRole(worldModel->getUNum()-1);
	int GameState = loader->getTeamState();

	return getDefensiveSkill();
	if (worldModel->GetsBall())
		return getAttackSkill();
	else if(Nearest())
		return getDefensiveSkill();

	if(role == ATT){
		switch (GameState)
		{
		case ATTACKING:
			return getAttackSkill();
			break;
		case ATTDEFEND:
			return getAttackSkill();
			break;
		case DEFENDING:
			if(OppInRegion())
				return getDefensiveSkill();
			else
				return getAttackSkill();
			break;
		case DEFATTACK:
			return getAttackSkill();
			break;
		default : return SKILL_STAND;
		}
	}

	else if(role == MID){

		switch (GameState)
		{
		case ATTACKING:
			return getAttackSkill();
			break;

		case ATTDEFEND:
			if(OppInRegion())
				return getDefensiveSkill();
			else
				return getAttackSkill();
			break;

		case DEFENDING:
			return getDefensiveSkill();
			break;

		case DEFATTACK:
			if (OppInRegion())
				return getDefensiveSkill();
			else
				return getAttackSkill();
			break;
		default : return SKILL_STAND;

		}
	}

	else if (role == DEF) {

		switch (GameState) {
		case ATTACKING:
			if (OppInRegion())
				return getDefensiveSkill();
			else
				return getAttackSkill();
			break;

		case ATTDEFEND:
			return getDefensiveSkill();
			break;

		case DEFENDING:
			return getDefensiveSkill();
			break;

		case DEFATTACK:
			return getDefensiveSkill();
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
