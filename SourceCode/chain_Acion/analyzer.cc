/*
 * analyzer.cc
 *
 *  Created on: Jan 28, 2017
 *      Author: aastteam
 */

#include "../rvdraw/rvdraw.h"

#include "../chain_Acion/analyzer.h"

#include "analyzer.h"
#include "../behaviors/naobehavior.h"

#include "../debugger/debugger.h"
#include "../math/vecposition.h"
#include "../math/Geometry.h"
#include "skillpass.h"
#include "support.h"
#include <cstdio>
#include <cstdlib>
#include <map>
#include<string>
#include <fstream>

#include <iostream>

#include <cstdio>
#define eps 0.01
#define INF 1<<12
using namespace std;
using namespace SIM;
string strategyFile;
map<string, double> factors;
double threatTime;

void loadParameters(string file, map<string, double> &factors) {
	factors.clear();
	fstream of(file.c_str(), ios_base::in);
	string str;
	double val;

	while (of >> str >> val) {
		factors[str] = val;
	}
	of.close();

}
bool Analyzer::canThreat(double time) {
	VecPosition ball = wm->getBall();
	double nst = wm->getBall().getDistanceTo(wm->getOpponent(wm->getOpponentClosestTo(ball)));
			if(time < 0.1)
				return true;
			else{
				if(time < 1)
					return nst>= 1;

				else if(time < 2)
					return nst>=2;

				else
					return nst>=4;
			}

//	if( wm->getPlayMode() != PM_PLAY_ON || time < 0.1)
//		return true;

//	//time = 1;
//	for (int i = WO_OPPONENT1;i<= WO_OPPONENT11;i++) {
//		VecPosition start = wm->getOpponent(i);
//		VecPosition end;
//		if(!wm->getWorldObject(i)->validPosition)
//			continue;
//		//for(int k=0.5;k<time;k++)
//		//{
//		   end = wm->predictPlayer(i-WO_TEAMMATE1,time);
//
//		  // cout<<start<<"   "<<end<<"   "<<start.getDistanceTo(ball)<<" "<<end.getDistanceTo(ball) << " "<<endl;
//			if (end.getDistanceTo(ball) < 1 ) {
//						return false;
//			}
//		//}
//		if (start.getDistanceTo(ball) < 1) {
//					return false;
//
//				}
////		if(perp(ball,start,end) < 0.5) {
////			return false;
////		}
//
//	}	//cout<<"no threat "<<time<<endl;
//	return true;


}
kickSkill::kickSkill(SkillType type, WorldModel *wm, formationLoader *fm) :
		skilldesc(type) {
	this->wm = wm;
	this->fm = fm;
}
passSkill::passSkill(WorldModel *wm, formationLoader *fm) :
		kickSkill(SKILL_PASS, wm, fm) {

	loadParameters(fm->getStrategy() + "/passing.io", factors);
	myGoalP = factors["myGoal"];
	theirGoalP = factors["theirGoal"];
	surrOppP = factors["surrounding"];
	passlenP = factors["length"], passCutP = factors["cut"], supportP =
			factors["supporting"], supportP = factors["cutConst"], Cnear =
			factors["pressConst"];
	factor = factors["factor"];
	MyGoalC = factors["myGoalConst"], TheirGoalC = factors["theirGoalConst"];

}

throughSkill::throughSkill(WorldModel *wm, formationLoader *fm) :
		passSkill(wm, fm) {
	this->wm = wm;
	this->fm = fm;
}
shootSkill::shootSkill(WorldModel *wm, formationLoader *fm) :
		kickSkill(SKILL_SHOOT, wm, fm) {
	loadParameters(fm->getStrategy() + "/shooting.io", factors);

	//double C_TheirGoal = 0.0 , L_TheirGoal = -0.7 , R_TheirGoal = 0.7;
	Ccut = factors["cutConst"], Cnear = factors["pressConst"];
	factor = factors["factor"];
}

Analyzer::Analyzer(WorldModel *wm) {
	//freopen("f.txt", "w", stdout);
	this->wm = wm;
	this->maxShot = 10; //TODO get this from wm
	loader = NULL;
	max = (new skilldesc(SKILL_STAND));
}
Analyzer::Analyzer(WorldModel *wm, formationLoader*loader) {
	this->wm = wm;
	this->maxShot = 10; //TODO get this from wm
	this->loader = loader;
	strategyFile = loader->getStrategy();

	max = (new skilldesc(SKILL_STAND));
}
double TIME = 0;
void Analyzer::resetCandidates() {

	for (int i = 0; i < (int) skillset.size(); i++)
		skillset[i].~skilldesc();
	skillset.clear();
}

bool off = false, of = true, od = true;
void Analyzer::generateDribble(int range) {
	int thfas = wm->getOpponentClosestTo(wm->getBall()) + WO_OPPONENT1 - 1;
	double angle = -range;
	stringstream ss;
	int cltime = wm->getOpponent(
			wm->getOpponentClosestTo(wm->getBall()) + WO_OPPONENT1 - 1).getDistanceTo(
			wm->getBall());
	for (double len = 0.5;len<=3;len+=0.5)
	for (int i = 0; angle <= range; i++) {
		double time = fabs(wm->getMyAngDeg() - angle) / 15;
		dribble skill = *(new dribble(wm, loader));
		skill.angle = angle;
		VecPosition tar, ball = wm->getBall();
		VecPosition lball = wm->g2l(ball);
		tar = lball + VecPosition(len, angle, 0, POLAR);
		tar = wm->l2g(tar);
		skill.setTarget(tar);
		skill.calcCost();
		skillset.push_back(skill);
		angle += range / 10;

	}
	string mes;
	ss >> mes;
}
VecPosition Analyzer::getDuePos() {
	VecPosition toret = loader->getDuePosition(wm->getUNum());
	if(abs(wm->getBall().getY() - toret.getY()) < loader->getDueWidth(wm->getUNum()))
		toret.setY(wm->getBall().getY());
	VecPosition min = loader->defending[wm->getUNum() - 1];

	VecPosition max = loader->attacking[wm->getUNum() - 1];
	double ratio = (wm->getBall().getX() + 15) / 30;
	VecPosition ret = min + (max - min) * ratio;
//if(wm->getUNum() == 2)
//	cout<< ret<<endl;
	return toret;
}
void Analyzer::generateBallHolderSkills() {

	int threat = WO_OPPONENT1 + wm->getOpponentClosestTo(wm->getBall()) - 1; // a2rab opponent lel kora
	double clos = 50; // wm->getBall().getDistanceTo(wm->getOpponent(WO_OPPONENT1-1+wm->getOpponentClosestTo(wm->getBall())));
	for (int i = WO_OPPONENT2; i <= WO_OPPONENT11; i++) {
//		cout<<	wm->getWorldObject(i)->orien
//				- (wm->getBall() - wm->getOpponent(i)).getAngleWithVector(
//						VecPosition(1, 0, 0, CARTESIAN))<<"\n";
//		if (!fabs(
//				wm->getWorldObject(i)->orien
//						- (wm->getBall() - wm->getOpponent(i)).getAngleWithVector(
//								VecPosition(1, 0, 0, CARTESIAN))) < 45) {
//			continue;
//		}
		if (!wm->getWorldObject(i)->validPosition) // || wm->getFallenOpponent(i))
			continue;
		double fact = 1;
		if ((wm->getBall() - wm->getMyPosition()).getAngleWithVector(
				wm->getBall() - wm->getOpponent(i)) < 90) {
			//fact = 2;
		}
		double add = 0;
		if (wm->getFallenOpponent(i - WO_OPPONENT1))
			add = 2;
		clos = min(clos,
				fact * (wm->getBall() - wm->getOpponent(i)).getMagnitude()
						+ add);
	}
	//threatTime = wm->getPlayerTimeTo(threat, wm->getBall());
	threatTime = clos * 1.25;
	//threatTime = clos;
	int range = 20;

	skillset.push_back(*(new skilldesc(SKILL_STAND)));
	if (wm->distancetoBall(wm->getMyPosition()) < 1.5) {
		od = false;
		if (clos > 0 //2.6
				//threatTime > 5 //TODO change this
				) {
			of = false;
			od = true;
			range = 80;
			generateDribble(range);
			generatePassPoints(PLAYERS);
			generateKick();

			//cout << "FAR"<<clos<<"\n";
			//return ; //delete
		}
		if (clos > 0			//0.2
				//threatTime >1
				) {
		} else {
			skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
			scill.setTarget(wm->getBall());
			scill.setCost(0);
			skillset.push_back(scill);
		}
		return;

	}
	skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
	//scill.skill = ;
	scill.setTarget(wm->getBall());
	scill.setCost(0);
	skillset.push_back(scill);

}
bool inZone(VecPosition ch, VecPosition center, double width, double top,
		double bottom) {

	return ((ch.getX()) <= top && (ch.getX()) >= bottom)
			&& (abs(ch.getY() - center.getY()) <= width / 2);
}
void Analyzer::generateAttackingSkills() {
	int unum = wm->getUNum();
	double top = getDuePos().getX() + loader->getDueLength(unum) / 2,
			bottom =
			getDuePos().getX() - loader->getDueLength(unum) / 2;
	if(wm->getRole(unum-1) < 2)
		bottom = -15;
	if(wm->getRole(unum-1) > 2)
		top = 15;
	for (int i = WO_OPPONENT1; i < WO_OPPONENT11; i++) {
		if (inZone(wm->getOpponent(i), getDuePos(), loader->getDueWidth(unum),
				top, bottom)) {

			top = wm->getOpponent(i).getX();
		}
	}
	double secondclos= 100;
	int secondclosId = 0;
	if (wm->getBall().getX() > 0) {
		VecPosition supporter = wm->getBall() - VecPosition(1.5, 0, 0);
//		supporter = collisionAvoidance(true /*Avoid teamate*/,
//				true /*Avoid opponent*/, true /*Avoid ball*/, 1, 1,
//				supporter, true /*fKeepDistance*/);
		for(int i = WO_TEAMMATE1;i<=WO_TEAMMATE11;i++)
		if (wm->getTeammateClosestTo(supporter) == i-WO_TEAMMATE1 +1)
			continue;
		else if(wm->getWorldObject(i)->pos.getDistanceTo(supporter) < secondclos) {
			secondclos = wm->getWorldObject(i)->pos.getDistanceTo(supporter);
			secondclosId = i-WO_TEAMMATE1+1;
		}
			if(secondclosId == wm->getUNum()) {
			skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
			scill.setTarget(supporter);
			scill.setCost(0.0);
			skillset.push_back(scill);
			return;}


	}

	if (!inZone(wm->getMyPosition(), getDuePos(), loader->getDueWidth(unum),
			top, bottom)) {

		skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
		scill.setTarget(getDuePos());
		scill.setCost(0.0);
		skillset.push_back(scill);
		//cout<<"GoingHome "<<wm->getUNum()<<" "<<wm->getMyPosition() << " "<<getDuePos()<<" "<<top<<" "<<endl;;
		return;
	}
	supportSkill scill2 = *(new supportSkill(this->wm, this->loader));
	skillset.push_back(scill2);
	//of = true;
	//cout << "I am supporting" <<wm->getUNum()<<endl;
	//}
}
VecPosition tempBack(int unum) {
	switch (unum){
	case 2: return VecPosition(-13,-2.5,0);

	case 3: return VecPosition(-13,-1,0);

	case 4: return VecPosition(-13,1,0);

	case 5: return VecPosition(-13,2.5,0);
	}
}
void Analyzer::generateDefensiveSkills() {
	if(wm->getRole(wm->getUNum()-1) == 1 && wm->getBall().getX() < wm->getMyPosition().getX()){

		skilldesc backo =*(new skilldesc(SKILL_WALK_OMNI));
		VecPosition tar = tempBack(wm->getUNum());
		if(wm->getBall().getX() > tar.getX())
		{
		backo.setTarget(tar);
		backo.setCost(0);
		skillset.push_back(backo);
		return ;
		}
	}
	mark scill = *(new mark(wm, loader));
	scill.setTarget(loader->getDuePosition(wm->getUNum()));
	scill.setCost(500);
	skillset.push_back(scill);
	double max = 1000;
	threatTime = 1;
	for (int i = 0 + WO_OPPONENT2; i <= WO_OPPONENT11; i++) {
		if (wm->getTeammateClosestTo(wm->getOpponent(i)) == wm->getUNum()
				&& wm->getOpponentClosestTo(wm->getBall())
						== (i - WO_OPPONENT1 + 1) ) {         //if the nearest is me ..
			continue;
		}
		VecPosition cent = getDuePos();
double top = cent.getX() + loader->getDueLength(wm->getUNum())/2;

double down = cent.getX()-+ loader->getDueLength(wm->getUNum())/2;
if(((wm->getRole(wm->getUNum()-1))!=3 && !inZone(wm->getOpponent(i),cent,loader->getDueWidth(wm->getUNum()),top,down))) {
	continue;
}
		mark scill = *(new mark(wm, loader));
		//scill.skill = ;
		scill.time=0;

		scill.setTarget(wm->getOpponent(i));
		VecPosition targ;
		scill.calcCost();
//		 ofstream myfile;
//		  myfile.open ("example.txt");
//		  myfile << "cost My Pos : " << wm->getMyPosition().getX() << " " <<
//				  wm->getMyPosition().getY() << " to " <<   wm->getOpponent(i).getX()
//				  << " " << wm->getOpponent(i).getY() << " is " << scill.getCost() << endl;
//		  myfile.close();
		if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
					< wm->getOpponent(i).getDistanceTo(
							wm->getMyRightGoalPost()))
				targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
			else
				targ = wm->getOpponent(i) - wm->getMyRightGoalPost();

			targ *= 0.10;
			targ = wm->getOpponent(i) - targ;
	}
	//cout << "Player " << wm->getUNum() << "Cost : " << max << "Targ : " << (opp-11) << endl;
	generateintersect();
}

void Analyzer::generateintersect() {
	int n = 0;
	int Mark = 0;
	VecPosition me = wm->getMyPosition();
	VecPosition Half_Goal = (wm->getMyLeftGoalPost() + wm->getMyRightGoalPost())
			/ 2;
	//int NrstLeftTeammate,NrstRightTeammate;
	//	NrstLeftTeammate = wm->getTeammateClosestTo((wm->getMyLeftGoalPost()+Half_Goal)/2)+WO_TEAMMATE1-1;
	//	NrstRightTeammate = wm->getTeammateClosestTo((wm->getMyRightGoalPost()+Half_Goal)/2)+WO_TEAMMATE1-1;
	VecPosition ball = wm->getBall();
	//int cnt = 0;
	double d = wm->distancetoBall(wm->getMyPosition());

	if (wm->getMyPosition().getX() > wm->getBall().getX()
			|| wm->getFallenTeammate(wm->getUNum()) == true)
		//	&& d > 1.5)
		d += 50;

	for (int i = WO_TEAMMATE2; i <= WO_TEAMMATE11; i++) {// kam teammate a2rb ll kora

		if ((i - WO_TEAMMATE1 + 1) == (wm->getUNum() - WO_TEAMMATE1 + 1))
			continue;

		//		if(me.getX()>ball.getX() && Mark &&
		//						wm->getTeammateClosestTo(ball)==wm->getUNum()+WO_TEAMMATE1-1){
		//		//			double DisToOpp = wm->getTeammate(i).getDistanceTo(ball);
		//		//			int AngleWithBall = wm->getTeammate(i).getAngleWithVector(ball);
		//		//			int AngleWithGoal = wm->getTeammate(i).getAngleWithVector((wm->getOppLeftGoalPost()+wm->getOppRightGoalPost())/2);
		//					if(wm->getTeammate(i).getX()+1.0 < wm->getOpponent((wm->getOpponentClosestTo(ball)+WO_OPPONENT1-1)).getX()){
		//						Mark =0;
		//						cout << "Teammate Succ " << i << endl;
		//					}
		//				}
		double di = wm->distancetoBall(wm->getTeammate(i));
		if (wm->getTeammate(i).getX() > wm->getBall().getX()) { //|| wm->getFallenTeammate(i - WO_TEAMMATE1 + 1)){// && wm->distancetoBall(wm->getTeammate(i)) > 1.5)
			di += 50;
			Mark++;
		}

		if ((di < d) && wm->getWorldObject(i)->validPosition)
			n++;
	}
	//cout << "Player no " << wm->getUNum() << " N = " << n << endl;
	int MyNum = wm->getUNum() + WO_TEAMMATE1 - 1;
	int ClosestTeammte = wm->getTeammateClosestTo(ball) + WO_TEAMMATE1 - 1;
	int i = WO_OPPONENT1 - 1 + wm->getOpponentClosestTo(wm->getBall()); //a2rb opp ll kora
	VecPosition v = wm->getMyRightGoalPost() + wm->getMyLeftGoalPost();
	v /= 2;
	double ang = wm->getWorldObject(i)->orien;
	VecPosition end = v; //wm->getOpponent(i) +(wm->getOpponent(i) - );// + VecPosition(20, ang, 0, POLAR);
	VecPosition tar = perpPoint(wm->getMyPosition(), wm->getBall(), end);
	VecPosition target, tempTarget;
	int Opp = wm->getOpponentClosestTo(ball) + WO_OPPONENT1 - 1;

	if (Mark == 9 && wm->getRole(wm->getUNum() - 1) < 2) {
		mark scill = *(new mark(wm, loader));

		//target = (H_Goal + wm->getOpponent(i)) / 2;
		target = wm->getMyPosition();
		//cout << "Pos = " << target << endl;
		//cout <<" New = " << target << endl;
		target = (H_Goal+ball)/2;
		if(wm->getTeammateClosestTo(ball) == wm->getUNum())
			target = ball;
		scill.setTarget(target);
		scill.setCost(0);
		skillset.push_back(scill);
		return;
	}
//	else if(ball.getX()<-10){
//		double oppDis = ball.getDistanceTo(worldModel->getOpponent(worldModel->getOpponentClosestTo(ball)+WO_OPPONENT1-1));
//				//cout << "Player " << wm->getUNum() << endl;
//			VecPosition HALF_GOAL = (worldModel->getMyLastPosition()+worldModel->getMyRightGoalPost())/2;
//			double MeDisToBall = ball.getDistanceTo(worldModel->getTeammate(worldModel->getTeammateClosestTo(ball)+WO_TEAMMATE1-1));
//			if((worldModel->getTeammateClosestTo(ball) == worldModel->getUNum()) && me.getDistanceTo(ball)<0.3){
//			for(int i=WO_OPPONENT1;i<=WO_OPPONENT11;i++){
//				VecPosition opp = worldModel->getOpponent(i);
//				double DisToOpp = me.getDistanceTo(opp);
//				int AngleWithOpp = me.getAngleWithVector(opp);
//				double MyAng = worldModel->getWorldObject(worldModel->getUNum()+WO_TEAMMATE1-1)->orien;
//				//cout << "Me " << AngleWithOpp << endl;
//				if(DisToOpp < 0.25 && fabs(AngleWithOpp) < 5 && me.getDistanceTo(HALF_GOAL)<5.0
//						&& fabs(MyAng)<60){
//					return intercept();
//					}
//				}
//			}
//	}

	else if (n == 0) {
		skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
//		if (wm->getRole(wm->getUNum() - 1) == 1) {
//			double pX = 3.0, pY = 0.75;
//			double newX = ball.getX() + pX, newY = ball.getY() + pY;
//			if (abs(newX - oldX) < 0.2)
//				newX = oldX;
//			if (abs(newY - oldY) < 0.2)
//				newY = oldY;
//			if (me.getDistanceTo(ball) > 0.2) {
//				target = ball;
//				target.setZ(0);
//				target.setX(target.getX() - 0.5);
//				}
//			oldX = newX;
//			oldY = newY;
//			else
//
//			//return kickBall(KICK_IK, VecPosition(newX, newY, 0));
//			cout << "Defenders defending" << endl;
//		}
//		//cout << "Nearest Players is : " << wm->getUNum() << endl;
//		else
		target = wm->predictBall(0.7);
		target = target + ((wm->predictBall(0.3)-ball)*2);
//						target = target+((wm->predictBall(0.3)-ball)*2);

//		VecPosition lball  = wm->g2l(wm->getOpponent(wm->getOpponentClosestTo(ball)));
//				target = lball+VecPosition(1.5,0,0,POLAR);
//				target =wm->l2g(target);

				//cout << "My Pos " << worldModel->getMyPosition() << endl;
				//cout << "New " << target << endl;

	//	target = tempTarget-((tempTarget-ball)/2);
//		if(wm->getOpponent(i).getX()-1 > wm->getMyPosition().getX())
//			scill.setTarget(ball);
//		else
		//cout << "Ball Pos = " << ball.getX() << " " << ball.getY() << endl;
		//cout << "New POSSSSSS = " << target.getX() << " " << target.getY() << endl;
		scill.setTarget(target);
			scill.setCost(0);
			skillset.push_back(scill);
			return;

	} else if (n == 1) // || ((wm->distanceToMyGoal(wm->getBall()) < 10) && (wm->getUNum() < 6))
			{
		//	cout << "Second Nst = " << wm->getUNum() << endl;
		//cout << "Second nearest is " << wm->getUNum() << endl;
		mark scill = *(new mark(wm, loader));
		VecPosition targ;
		scill.setCost(0);
		if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
				< wm->getOpponent(i).getDistanceTo(wm->getMyRightGoalPost()))
			targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
		else
			targ = wm->getOpponent(i) - wm->getMyRightGoalPost();

		targ *= 0.20;
		targ = ball - targ;
		scill.setTarget(target);
			scill.setCost(0);
			skillset.push_back(scill);
			return;
	}
	return;
}

void Analyzer::generateCanditates() {
	if (wm->getPlayMode() == PM_PLAY_ON) {
		if (wm->isOut(wm->getMyPosition())) {
			resetCandidates();
			skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
			//scill.skill = ;
			scill.setTarget(getDuePos());
			scill.setCost(0);
			skillset.push_back(scill);
			return;
		}
	}
	VecPosition ball = wm->getBall();
	if (wm->getUNum() == 1 && wm->distanceToMyGoal(ball) > 4) {
		return;
	}

	if (!skillset.empty())
		return;
	threatTime = 300; //big number
	//skillset.push_back(*(new skilldesc(SKILL_STAND)));
	//cout<<"..x\n";
	int ballHolder = wm->getTeammateClosestTo(wm->getBall());
	bool really = false;
	for (int i = WO_TEAMMATE1; i <= WO_TEAMMATE11; i++) {
		if (wm->getUNum() + WO_TEAMMATE1 - 1 == i)
			continue;
		double dist = wm->getTeammate(i).getDistanceTo(ball);
		if (dist < ball.getDistanceTo(wm->getMyPosition()) && dist < 1) {
			really = true;
		}
	}
	if (ballHolder == wm->getUNum() // wm->hasBall()
	//&&loader->getTeamState() == ATTACKING
			) {
		generateBallHolderSkills();
		return;
	}
	//double MeDisToBall = wm->getBall().getDistanceTo(worldModel->getTeammate(worldModel->getTeammateClosestTo(ball)+WO_TEAMMATE1-1));

//	if ((loader->getTeamState() == ATTACKING)
//			&& (wm->distanceToMyGoal(ball)) > 0) {
	if(wm->hasBall() || wm->getTeammateClosestTo(ball) == wm->getUNum()){
		//cout << "ATTAAAAAAACK" << endl;
		generateAttackingSkills();
		return;
	}
	//cout << "DEF" << endl;
	generateDefensiveSkills();
	return;

}
void Analyzer::generatePoints() {
	stringstream ss;
	ss << wm->getBall() << "_" << wm->getUNum();
	string out;
	ss >> out;
	NaoBehavior::debug->success(wm->getCycle(), out);
	for (int i = 0 + WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| i - WO_TEAMMATE1 + 1 == wm->getUNum())
			continue;
		VecPosition target = VecPosition(0, 0, 0); //wrong entry
		stringstream ss;
		ss << target << "_" << (i - WO_TEAMMATE1) << "_"
				<< wm->getWorldObject(i)->pos;
		string out;
		ss >> out;
		NaoBehavior::debug->debug(wm->getCycle(), out);
		if (wm->getBall().getDistanceTo(target) > maxShot)
			continue;
		passSkill scill = *(new passSkill(this->wm, this->loader));

		scill.setTarget(target);

		scill.calcCost();

		skillset.push_back(scill);
	}
}

void Analyzer::generatePlayers() {
	TIME = wm->getTime();
	VecPosition ball = wm->getBall();
	double ini = 100;
	VecPosition ftar,pla;
	for (int i = 0 + WO_TEAMMATE1; i <= WO_TEAMMATE11; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| i - WO_TEAMMATE1 + 1 == wm->getUNum()
				|| wm->isOut(wm->getTeammate(i)))
			continue;
		if (ball.getDistanceTo(wm->getTeammate(i)) > maxShot)
			continue;
		passSkill scill = *(new passSkill(this->wm, this->loader));
		//scill.skill = ;
double shift = 1;
if(wm->getTeammate(i).getDistanceTo(ball) > 4.5)
	shift = 1;
		VecPosition target = wm->getTeammate(i)
				//wm->predictPlayer(i-WO_TEAMMATE1,shift);
				+ *(new VecPosition(0.5, Deg2Rad(wm->getWorldObject(i)->orien),
						0, POLAR));

		if ((wm->getTeammate(i) - ball).getAngleWithVector(
				target - wm->getTeammate(i)) < 10) {
			double shift = 10;
			if (ball.getY() > 0)
				shift = -10;
			//target = VecPosition(target.getMagnitude(),
			//		target.getTheta() + shift, 0, POLAR);
		}
		if (target.getX() < wm->getBall().getX() && target.getX() < 0) {
			continue;
		}
		scill.setTarget(target);

		scill.calcCost();
		if(scill.getCost() < ini) {
			ini = scill.getCost();
			pla = wm->getTeammate(i);
			ftar = scill.getTarget();
		}


		skillset.push_back(scill);
	}
	//cout << "PASSING TO " << pla <<" AT "<<ftar<<endl;
}

void Analyzer::generatePassPoints(PassType type) {
	switch (type) {
	case POINTS:
		generatePoints();
		break;
	case PLAYERS:
		generatePlayers();
		break;
	}
}
skilldesc Analyzer::getTopSkill() {
	skilldesc t = *max;
	t.calcCost();
	double dmi = 100;
	double pmi = 100;
	if (skillset.empty()) {

		skilldesc max = *(new skilldesc(SKILL_STAND));
		//max.getType() = SKILL_STAND;
		return max;
	}
	max = &skillset[0];
	bool threatb = false;
	for (int i = 0; i < (int) skillset.size(); i++) {
		if ( (threatb = canThreat(skillset[i].getTime())) &&skillset[i].getCost() < max->getCost()
				)
		{
			max = &skillset[i];
	//	if(skillset[i].getType() == 7)
		//	cout << "PASS" << "\n";
		}
		//if(skillset[i].getType() == 7)
			//		cout << "PASS" <<threatb << "\n";

		if (skillset[i].getType() == SKILL_DRIBBLE)
			dmi = min(dmi, skillset[i].getCost());
		if (skillset[i].getType() == SKILL_PASS)
			pmi = min(pmi, skillset[i].getCost());
//		if(wm->getTeammateClosestTo(wm->getBall()) == wm->getUNum()
//				&& skillset[i].getType() == SKILL_PASS)
		//cout<<skillset[i].getCost()<<" Is my pass cost\n";

	}
	off = of;
	stringstream ss;
	ss << (max->getType());
	string message;
	ss >> message;
	//if ( canThreat(t.getTime()))
	if (t.getCost() - max->getCost() < 0.7 && t.getType() == max->getType()
			&& (t.getType() == SKILL_PASS || t.getType() == SKILL_DRIBBLE))
		max = &t;
//	if(wm->getBallHolder() == WO_TEAMMATE1+wm->getUNum()-1)
//			cout <<threatTime<<" "<<max->getType() << " "
//			<< max->getTime() <<endl;

//	if(wm->getTeammateClosestTo(wm->getBall()) == wm->getUNum() )
//		cout<<max->getType()<<" Is my skill and "<<max->getCost() <<"\n";

	if (max->getType() == SKILL_DRIBBLE) {
		VecPosition tar = max->getTarget();
		//cout<<"IAm dribbling at"<<wm->g2l(tar).getTheta()<<endl;
	}

	return *max;
}

VecPosition kickSkill::NearestTeammate(VecPosition target) {
	return wm->getTeammate(wm->getTeammateClosestTo(target) + WO_TEAMMATE1);
}

VecPosition kickSkill::NearestOpponent(VecPosition target) {
	return wm->getOpponent(wm->getOpponentClosestTo(target) + WO_OPPONENT1);
}
double kickSkill::surroundingOpponents(VecPosition target,
		double Distance_NearestOpp_To_Ball) {
	double ret = 0;
	int n = 0;
	for (int i = 0 + WO_OPPONENT2; i < WO_OPPONENT1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| wm->isOut(wm->getWorldObject(i)->pos)
				|| wm->getWorldObject(i)->pos.getDistanceTo(target) > 3)
			continue;
		ret += exponential(wm->predictOpponentTimeTo(i, target,this->time), Cnear);
		n++;
	}
	double width = HALF_FIELD_Y;
	width -= abs(target.getY());
	ret += 2*exponential(width, Cnear);
	if(target.getX() > 12)
			ret+=2*exponential(15 - target.getX(), Cnear);
	return ret / 6;
}

double kickSkill::supportingTeamMates(VecPosition target) {
	if (NUM_AGENTS == 1)
		return 0;
	double ret = 0;
	int n = 0;
	for (int i = WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| wm->isOut(wm->getWorldObject(i)->pos))
			continue;
		double distance = wm->getTeammate(i).getDistanceTo(target);
		if (distance > eps) {
			ret += 1 - exponential(distance, Cnear);
			n++;
		}
	}
	return n == 0 ? 0 : ret / n;
}
double kickSkill::passSafety(VecPosition passer, VecPosition target) {

	double length = (target - passer).getMagnitude();

	double nearest = 1e9;

	int targetPlayer = wm->getTeammateClosestTo(target);

	for (int i = 0 + WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
		int curUNum = i - WO_TEAMMATE1 + 1;
		if (wm->getWorldObject(i)->validPosition && wm->getUNum() != curUNum
				&& targetPlayer != curUNum)

			nearest = min(nearest, perp(wm->getTeammate(i), passer, target));
	}

	for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++)
		if (wm->getWorldObject(i)->validPosition)
			nearest = min(nearest, perp(wm->getOpponent(i), passer, target));

	const double th = 0.2;

	if (nearest <= th) {
		return 1;
	}
	double ret = exponential(nearest, Ccut);
	double surrounding_Opponents = surroundingOpponents(target,0);


			return ret +surrounding_Opponents;
}

double passSkill::effectiveness(VecPosition passer, VecPosition target) {

	double myGoal = exponential(wm->distanceToMyGoal(target), MyGoalC);
	double theirGoal = 1
			- exponential(wm->distanceToOppGoal(target), TheirGoalC);
//	double supporting_TeamMates = supportingTeamMates(target);
	double ret = (myGoal +  theirGoal );


		return ret;
}

double passSkill::evaluatePass(VecPosition passer, VecPosition target) {
	passer.setZ(0);
	target.setZ(0);
	double e = effectiveness(passer, target);
	double s = passSafety(passer, target);
	cost = (5 * e + 1 * s);//+ wm->getBall().getX() / 15; // dribble preferal
	//cout << ">>>>>>>>>>PE = " << e << " PS = " << s << " PC = " << cost << "\n";

	return cost;
}

double passSkill::calcCost() {
	//if (wm->getBall().getDistanceTo(wm->getMyPosition()) > 1.5)
	//return cost = INF;
	cost = evaluatePass((wm->getBall()), (target));
	this->calcTime();
	return cost;
}

////
void Analyzer::generateKick() {
	shootSkill *scilll = new shootSkill(wm, loader);
	double cost = INF;
	VecPosition Pos_ToShot;
	VecPosition ball = wm->getBall();
	if (wm->distanceToOppGoal(ball) < this->maxShot - 1) {

		Pos_ToShot = VecPosition(15.0, 0, 0);
		scilll->setTarget(Pos_ToShot);
		scilll->calcCost();
		if (scilll->getCost() < cost) {

			skillset.push_back(*scilll);
		}
		Pos_ToShot = VecPosition(15.0, 0, 0);
		scilll->setTarget(Pos_ToShot);
		scilll->calcCost();

		if (scilll->getCost() < cost) {
			skillset.push_back(*scilll);
		}
		Pos_ToShot = VecPosition(15.0, 0, 0);
		scilll->setTarget(Pos_ToShot);
		scilll->calcCost();

		if (scilll->getCost() < cost) {
			skillset.push_back(*scilll);
		}
//		double dis = maxShot - wm->distanceToOppGoal(ball);
//		dis = dis / maxShot;
//		dis *= fabs(wm->getOppRightGoalPost().getY());
//		Pos_ToShot = VecPosition(15.2, -dis, 0);
//		scilll->setTarget(Pos_ToShot);
//		scilll->calcCost();
//		skillset.push_back(*scilll);
//		Pos_ToShot = VecPosition(15.2, 0, 0);
//		scilll->setTarget(Pos_ToShot);
//		scilll->calcCost();
//		skillset.push_back(*scilll);
//		Pos_ToShot = VecPosition(15.2, dis, 0);
//		scilll->setTarget(Pos_ToShot);
//		scilll->calcCost();
//		skillset.push_back(*scilll);

	}
}
double shootSkill::calcCost() {
	calcTime();
	return cost = factor * this->passSafety(wm->getBall(), this->target);
}
