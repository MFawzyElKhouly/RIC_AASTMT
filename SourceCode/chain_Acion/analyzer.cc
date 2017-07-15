/*
 * analyzer.cc
 *
 *  Created on: Jan 28, 2017
 *      Author: aastteam
 */

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
void Analyzer::resetCandidates() {

	for (int i = 0; i < (int) skillset.size(); i++)
		skillset[i].~skilldesc();
	skillset.clear();
}

bool off = false, of = true, od = true;
void Analyzer::generateDribble(int range) {
//	VecPosition tar = wm->getBall() - wm->getMyPosition();
//	tar*=1.2;
//	dribble skks = *(new dribble(wm,loader));
//	skks.setTarget(tar);
//	skillset.push_back(skks);
//	return;
	int thfas = wm->getOpponentClosestTo(wm->getBall()) + WO_OPPONENT1 - 1;

	double angle = -range;
	stringstream ss;
	int cltime = wm->getPlayerTimeTo(
			wm->getOpponentFastestTo(wm->getBall()) + WO_OPPONENT1 - 1,
			wm->getBall());

	for (int i = 0; angle <= range; i++) {
		double time = fabs(wm->getMyAngDeg() - angle) / 15;
		//if(cltime < time +0.5)
		//continue;
		dribble skill = *(new dribble(wm, loader));
		//skill.setAngle(angle);
		skill.angle = angle;
		VecPosition tar, ball = wm->getBall();
		VecPosition lball  = wm->g2l(ball);
			tar = lball+VecPosition(1,angle,0,POLAR);
			tar =wm->l2g(tar);
			skill.setTarget(tar);
			skill.calcCost();
		skillset.push_back(skill);
		//ss << angle << "_" << skill.getCost() << "|";
		angle += range/10;

	}
//	skilldesc skk = *(new skilldesc(SKILL_STAND));
//	skk.setCost(100);
//	skillset.push_back(skk);

	/*dribble skill = *(new dribble(wm, loader));
	 skill.setTarget(wm->getBall() + VecPosition(1, Deg2Rad(wm->getMyAngDeg()), 0, POLAR));
	 skill.calcCost();
	 skillset.push_back(skill);
	 */string mes;
	ss >> mes;
	//NaoBehavior::debug->debug(wm->getCycle(), mes);
	//cout<<mes<<endl;
}
void Analyzer::generateBallHolderSkills() {

	int threat = WO_OPPONENT1 + wm->getOpponentFastestTo(wm->getBall()) - 1; // a2rab opponent lel kora
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
		if(!wm->getWorldObject(i)->validPosition  || wm->getFallenOpponent(i))
			continue;
		double fact = 1;
		if ((wm->getBall() - wm->getMyPosition()).getAngleWithVector(
				wm->getBall() - wm->getOpponent(i)) < 90) {
			//fact = 2;
		}
		clos = min(clos,
				fact * (wm->getBall() - wm->getOpponent(i)).getMagnitude());
	}
	double threatTime = wm->getPlayerTimeTo(threat, wm->getBall());
	int range = 40;
	if (wm->distancetoBall(wm->getMyPosition()) < 1.5) {
		od = false;
			if (clos > 1.5
		//threatTime > 5 //TODO change this
				) {
			of = false;
			od = true;
			generatePassPoints(PLAYERS);
			generateKick();
			range  = 90;
			//cout << "FAR"<<clos<<"\n";
			//return ; //delete
		}
		if (clos > 0.2
		//threatTime >1
				) {
			generateDribble(range);
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
void Analyzer::generateAttackingSkills() {

	if(wm->getMyPosition().getDistanceTo(loader->getDuePosition(wm->getUNum())) >
	loader->getDueRadius(wm->getUNum())) {
		skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
			scill.setTarget(loader->getDuePosition(wm->getUNum()));
			scill.setCost(0.0);
	skillset.push_back(scill);
		return;
	}
	int theirh = wm->getOpponentFastestTo(wm->getBall());
	int mate = wm->getTeammateFastestTo(wm->getBall());
	if (theirh == -1
			|| wm->getPlayerTimeTo(theirh + WO_OPPONENT1, wm->getBall())
					> wm->getPlayerTimeTo(mate + WO_TEAMMATE1, wm->getBall())) {
		supportSkill scill2 = *(new supportSkill(this->wm, this->loader));
		skillset.push_back(scill2);
		of = true;
	}
}
void Analyzer::generateDefensiveSkills() {
	skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
	scill.setTarget(loader->getDuePosition(wm->getUNum()));
	scill.setCost(1);
	skillset.push_back(scill);

	for (int i = 0 + WO_OPPONENT2; i <= WO_OPPONENT11; i++) {
		if ((wm->getTeammateClosestTo(wm->getOpponent(i)) == wm->getUNum()
				&& wm->getOpponentClosestTo(wm->getBall())
						== i - WO_OPPONENT1 + 1)) {         //if the nearest is me ..
			continue;
		}
		mark scill = *(new mark(wm, loader));
		//scill.skill = ;

		scill.setTarget(wm->getOpponent(i));
		VecPosition targ;
		scill.calcCost();
		if (wm->getRole(wm->getUNum() - 1) < 2) {
			if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
					< wm->getOpponent(i).getDistanceTo(
							wm->getMyRightGoalPost()))
				targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
			else
				targ = wm->getOpponent(i) - wm->getMyRightGoalPost();
			targ *= 0.25;
			targ = wm->getOpponent(i)- targ;
		}
		if(wm->getRole(wm->getUNum() - 1) < 3){
			if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
								< wm->getOpponent(i).getDistanceTo(
										wm->getMyRightGoalPost()))
							targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
						else
							targ = wm->getOpponent(i) - wm->getMyRightGoalPost();
						targ *= 0.15;
						targ = wm->getOpponent(i) - targ;
		}
//		 else {
//			targ = wm->getBall() - wm->getOpponent(i);
//			targ *= 0.25;
//			targ += wm->getOpponent(i);
//		}

		scill.setTarget(targ);

		skillset.push_back(scill);

	}
	generateintersect();
}

void Analyzer::generateintersect() {
	int n = 0;
	//int Mark = 1;
	VecPosition me = wm->getMyPosition();
	VecPosition Half_Goal = (wm->getMyLeftGoalPost()+wm->getMyRightGoalPost())/2;
		//int NrstLeftTeammate,NrstRightTeammate;
	//	NrstLeftTeammate = wm->getTeammateClosestTo((wm->getMyLeftGoalPost()+Half_Goal)/2)+WO_TEAMMATE1-1;
	//	NrstRightTeammate = wm->getTeammateClosestTo((wm->getMyRightGoalPost()+Half_Goal)/2)+WO_TEAMMATE1-1;
		VecPosition ball = wm->getBall();
	//int cnt = 0;
	double d = wm->distancetoBall(wm->getMyPosition());
	if (wm->getMyPosition().getX() > wm->getBall().getX()
			&& wm->distancetoBall(wm->getMyPosition()) > 1.5)
		d += 50;
	for (int i = WO_TEAMMATE2; i <= WO_TEAMMATE11; i++) {// kam teammate a2rb ll kora

		if (i - WO_TEAMMATE1 + 1 == wm->getUNum())
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
		if (wm->getTeammate(i).getX() > wm->getBall().getX()// && wm->distancetoBall(wm->getTeammate(i)) > 1.5
				){
			di += 50;
		}

		if ((di < d || (di == d && i - (WO_TEAMMATE1 < wm->getUNum() - 1)))
				&& wm->getWorldObject(i)->validPosition
				//&& !wm->getFallenTeammate(i)
				) {
			n++;
		}
	}
	//cout << "Count = " << cnt << endl;

	int MyNum = wm->getUNum()+WO_TEAMMATE1-1;
	int ClosestTeammte = wm->getTeammateClosestTo(ball)+WO_TEAMMATE1-1;
	int i = WO_OPPONENT1 - 1 + wm->getOpponentClosestTo(wm->getBall()); //a2rb opp ll kora
	VecPosition v = wm->getMyRightGoalPost() + wm->getMyLeftGoalPost();
	v /= 2;
	double ang = wm->getWorldObject(i)->orien;
	VecPosition end = v;//wm->getOpponent(i) +(wm->getOpponent(i) - );// + VecPosition(20, ang, 0, POLAR);
	VecPosition tar = perpPoint(wm->getMyPosition(), wm->getBall(), end);
//	if ((tar - wm->getBall()).normalize()
//			!= (end - wm->getBall()).normalize() && (wm->getUNum()+WO_TEAMMATE1-1)==2) {
//		tar = wm->getBall() - VecPosition(1, 0, 0, CARTESIAN);
////		if ((tar - wm->getMyPosition()).getMagnitude() < (end - ball).getMagnitude())
////			tar = wm->getBall();
//	}
//	if(!Mark){
//		cout << MyNum << " Leave Opp " << endl;
//		skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
//				scill.setTarget((Half_Goal+VecPosition(0,0,0))/2);
//				scill.setCost(0);
//				skillset.push_back(scill);
//				return;
//		//cout << "Marking " << wm->getUNum() << endl;
////		for (int i = 0 + WO_OPPONENT2; i <= WO_OPPONENT11; i++) {
////				if ((wm->getTeammateClosestTo(wm->getOpponent(i)) == wm->getUNum()
////						&& wm->getOpponentClosestTo(wm->getBall())
////								== i - WO_OPPONENT1 + 1)) {         //if the nearest is me ..
////					continue;
////				}
////				mark scill = *(new mark(wm, loader));
////				//scill.skill = ;
////
////				scill.setTarget(wm->getOpponent(i));
////				VecPosition targ;
////				scill.calcCost();
////					if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
////										< wm->getOpponent(i).getDistanceTo(
////												wm->getMyRightGoalPost()))
////									targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
////								else
////									targ = wm->getOpponent(i) - wm->getMyRightGoalPost();
////								targ *= 0.15;
////					targ = wm->getOpponent(i) - targ;
////				scill.setTarget(targ);
////				skillset.push_back(scill);
////			}
//	}
	if(((MyNum==3 && n!=0 && n!=1) && ball.getX()>0)){
		skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
		//cout << "Nrst Left Is " << 3 << endl;
		scill.setTarget(tar);
		scill.setCost(0);
		skillset.push_back(scill);
		return;
	}
	else if(((MyNum == 4 && n!=0 && n!=1) && ball.getX()<0)){
		skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
		//cout << "Player NO 4 GOOOOOOO" << endl;
	//	cout << "Nrst Rtaright Isssss " << NrstRightTeammate << endl;
		scill.setTarget(tar);
		scill.setCost(0);
		skillset.push_back(scill);
		return;
	}
//	else if(cnt == 9 && wm->getUNum()==wm->getTeammateClosestTo(ball)){
//		mark scill = *(new mark(wm, loader));
//		VecPosition targ;
//		scill.setCost(0);
//		targ= perpPoint(wm->getMyPosition(), wm->getBall(), end);
//		scill.setTarget(targ);
//		scill.setCost(0);
//		skillset.push_back(scill);
//		return;
//	}
//	else if(n==0 && me.getX()>(wm->getOpponent(wm->getOpponentClosestTo(ball)+WO_OPPONENT1-1).getX()+0.5)){
//		mark scill = *(new mark(wm, loader));
//		VecPosition targ;
//		scill.setCost(0);
//		targ = wm->getOppLeftGoalPost();
//		scill.setTarget(targ);
//		skillset.push_borienack(scill);
//	}
		int Opp = wm->getOpponentClosestTo(ball)+WO_OPPONENT1-1;
	//	double OppTheta = wm->getOpponent(wm->getOpponentClosestTo(ball)).getTheta();
		VecPosition target;

		if(wm->getSide() == SIDE_RIGHT)
			target = ball
					+ *(new VecPosition(-1.7, Deg2Rad(wm->getWorldObject(Opp)->orien),
											0, POLAR));
		else
			target = ball
					+ *(new VecPosition(1.7, Deg2Rad(wm->getWorldObject(Opp)->orien),
											0, POLAR));
	if (n == 0 //|| ((wm->distanceToMyGoal(ball) < 10) && (wm->getUNum() < 6))
			) {
		skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
			scill.setTarget(target);
			scill.setCost(0);
			skillset.push_back(scill);
			return;
	}
	else if(n == 1 && wm->getFallenTeammate(wm->getTeammateClosestTo(ball)+WO_TEAMMATE1-1) == true){
		cout << "FALLEN TEAMMATE" << endl;
		skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
		scill.setTarget(target);
		scill.setCost(0);
		skillset.push_back(scill);
		return;
	}
	else if (n==1) // || ((wm->distanceToMyGoal(wm->getBall()) < 10) && (wm->getUNum() < 6))
			 {
		//cout << "Second nearest is " << wm->getUNum() << endl;
		mark scill = *(new mark(wm, loader));
		VecPosition targ;
		scill.setCost(0);
//		if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
//							< wm->getOpponent(i).getDistanceTo(
//									wm->getMyRightGoalPost()))
//						targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
//					else
//						targ = wm->getOpponent(i) - wm->getMyRightGoalPost();
//
//		targ*= 0.20;
//		targ = ball - targ;
		scill.setTarget(tar);
		skillset.push_back(scill);
	}
	else if(n == 2 && ball.getDistanceTo((wm->getMyRightGoalPost()+wm->getMyLeftGoalPost())/2)<10.0){
		skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
		scill.setCost(0);
//				if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
//									< wm->getOpponent(i).getDistanceTo(
//											wm->getMyRightGoalPost()))
//								targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
//							else
//								targ = wm->getOpponent(i) - wm->getMyRightGoalPost();
		if(ball.getX()<0){
			tar.setX(tar.getX()-1);
			cout << "BaLLX < 0 " << endl;
		}
			else
				tar.setX(tar.getX()+1);
				//targ*= 0.50;
				//targ = ball - targ;
				scill.setTarget(tar);
				skillset.push_back(scill);
	}

}
void Analyzer::generateCanditates() {
	if (wm->getPlayMode() == PM_PLAY_ON) {
		if (wm->isOut(wm->getMyPosition())) {
			resetCandidates();
			skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
			//scill.skill = ;
			scill.setTarget(loader->getDuePosition(wm->getUNum()));
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
	//cout<<"..x\n";
	int ballHolder = wm->getTeammateClosestTo(wm->getBall());

	if (ballHolder == wm->getUNum()
	//&&loader->getTeamState() == ATTACKING
			) {
		generateBallHolderSkills();
		return;
	}
	if ((loader->getTeamState() == ATTACKING)
			&& (wm->distanceToMyGoal(ball)) > 8) {

		generateAttackingSkills();
		return;
	}
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
	for (int i = 0 + WO_TEAMMATE1; i <= WO_TEAMMATE11; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| i - WO_TEAMMATE1 + 1 == wm->getUNum()
				|| wm->isOut(wm->getTeammate(i)))
			continue;
		if (wm->getBall().getDistanceTo(wm->getTeammate(i)) > maxShot)
			continue;
		passSkill scill = *(new passSkill(this->wm, this->loader));
		//scill.skill = ;

		VecPosition target = wm->getTeammate(i)
				+ *(new VecPosition(0.2, Deg2Rad(wm->getWorldObject(i)->orien),
						0, POLAR));
		if (target.getX() < wm->getBall().getX() && target.getX() < 0) {
			continue;
		}
		scill.setTarget(target);

		scill.calcCost();

		skillset.push_back(scill);
	}
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
	if (skillset.empty()) {

		skilldesc max = *(new skilldesc(SKILL_STAND));
		//max.getType() = SKILL_STAND;
		return max;
	}
	max = &skillset[0];
	for (int i = 0; i < (int) skillset.size(); i++) {
		if (skillset[i].getCost() < max->getCost())
			max = &skillset[i];
//		if(wm->getTeammateClosestTo(wm->getBall()) == wm->getUNum()
//				&& skillset[i].getType() == SKILL_PASS)
				//cout<<skillset[i].getCost()<<" Is my pass cost\n";

	}

	off = of;
	stringstream ss;
	ss << (max->getType());
	string message;
	ss >> message;
	if (t.getCost() - max->getCost() < 0.7 && t.getType() == max->getType()
			&& (t.getType() == SKILL_PASS || t.getType() == SKILL_DRIBBLE))
		max = &t;
//	if(wm->getTeammateClosestTo(wm->getBall()) == wm->getUNum() )
//		cout<<max->getType()<<" Is my skill and "<<max->getCost() <<"\n";



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
	for (int i = 0 + WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| wm->isOut(wm->getWorldObject(i)->pos)
				||wm->getWorldObject(i)->pos.getDistanceTo(target) > 4)
			continue;
		ret += exponential(wm->getPlayerTimeTo(i, target), Cnear);
		n++;
	}
	return n == 0 ? 0 : ret;
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

	return ret;
}

double passSkill::effectiveness(VecPosition passer, VecPosition target) {

	double myGoal = exponential(wm->distanceToMyGoal(target), MyGoalC);
	double theirGoal = 1
			- exponential(wm->distanceToOppGoal(target), TheirGoalC);
	double surrounding_Opponents = surroundingOpponents(target,
			wm->getPlayerTimeTo(wm->getTeammateFastestTo(target) + WO_TEAMMATE1,
					target));
//	double supporting_TeamMates = supportingTeamMates(target);

	double ret = (myGoal + theirGoal + surrounding_Opponents);
	return ret;
}

double passSkill::evaluatePass(VecPosition passer, VecPosition target) {
	passer.setZ(0);
	target.setZ(0);
	double e = effectiveness(passer, target);
	double s = passSafety(passer, target);
	cost = (5*e + s);
	//cout << ">>>>>>>>>>PE = " << e << " PS = " << s << " PC = " << cost << "\n";

	return cost;
}

double passSkill::calcCost() {
	//if (wm->getBall().getDistanceTo(wm->getMyPosition()) > 1.5)
	//return cost = INF;
	cost = evaluatePass((wm->getBall()), (target));

	return cost;
}

////
void Analyzer::generateKick() {
	shootSkill *scilll = new shootSkill(wm, loader);
	double cost = INF;
	VecPosition Pos_ToShot;
	VecPosition ball = wm->getBall();
	if (wm->distanceToOppGoal(ball) < this->maxShot - 1) {

		Pos_ToShot = VecPosition(15.2, 0, 0);
		scilll->setTarget(Pos_ToShot);
		scilll->calcCost();
		if (scilll->getCost() < cost) {

			skillset.push_back(*scilll);
		}
		Pos_ToShot = VecPosition(15.2, -0.3, 0);
		scilll->setTarget(Pos_ToShot);
		scilll->calcCost();

		if (scilll->getCost() < cost) {
			skillset.push_back(*scilll);
		}
		Pos_ToShot = VecPosition(15.2, 0.3, 0);
		scilll->setTarget(Pos_ToShot);
		scilll->calcCost();

		if (scilll->getCost() < cost) {
			skillset.push_back(*scilll);
		}

	}
}
double shootSkill::calcCost() {
	return cost = factor * this->passSafety(wm->getBall(), this->target);
}
