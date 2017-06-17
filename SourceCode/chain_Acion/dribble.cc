/*
 * dribble.cc
 *
 *  Created on: Feb 27, 2017
 *      Author: aastteam
 */
#include "analyzer.h"
#include "../behaviors/naobehavior.h"

#include "../math/vecposition.h"
#include "../math/Geometry.h"
#include "skillpass.h"
#include "support.h"
#include <cstdio>
#include <cstdlib>
#include <map>
#include<string>
#include <fstream>
#define eps 0.01
#define INF 55555
using namespace std;
using namespace SIM;

bool bii = false;
dribble::dribble(WorldModel *wm,formationLoader *fm):skilldesc(SKILL_DRIBBLE) {
this->wm = wm;
this->fm = fm;
map<string, double> factors;
loadParameters(fm->getStrategy() + "/dribble.io", factors);

	CTime = factors["CTime"];
	C_TheirGoal = factors["theirGoalConst"];
	TheirGoalP = factors["TheirGoalP"]; surrP = factors["surrP"];
			Cnear =
			factors["Cnear"];
			effP = factors["effP"];
			safeP = factors["safeP"];
			factor = factors["factor"];
			if(!bii) {
				stringstream ss;
				ss<<CTime<<"_"<<C_TheirGoal<<"_"<<TheirGoalP<<"_"<<surrP
						<<"_"<<Cnear<<"_"<<effP<<"_"<<safeP<<"_"<<factor;
				string mess;
				ss>>mess;
				NaoBehavior::debug->debug(0,mess);
				bii = true;
			}
}
double dribble::surroundingOpponents(){
	double ret = 0;
		int n = 0;
		for (int i = 0 + WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++) {
			if (!wm->getWorldObject(i)->validPosition || target.getX() > wm->getOpponent(i).getX())
				continue;
			ret += exponential(
					wm->getPlayerTimeTo(i,target)
							, Cnear);
			n++;
		}
		return n == 0 ? 0 : ret / n;
}

double dribble::dribbleReliability() {
	double ret = 0;
		int n = 0;
		for (int i = 0 + WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++) {

			double t =wm->getPlayerTimeTo(i, target);
			if (!wm->getWorldObject(i)->validPosition
					|| wm->isOut(wm->getWorldObject(i)->pos) || t > 6)
				continue;
			ret += exponential(t ,
					Cnear);
			n++;
		}
		return n == 0 ? 0 : ret / n;
}
double dribble::dribbleSafety() {
	//double myTime =  wm->getPlayerTimeTo(WO_TEAMMATE1-1+wm->getUNum(),target);
	//double theirTime = wm->getPlayerTimeTo(wm->getOpponentFastestTo(target)
			//+WO_OPPONENT1-1,target);
	//double theirTime2 = wm->getPlayerTimeTo(wm->getOpponentFastestTo((target+wm->getBall())/2)
				//+WO_OPPONENT1-1,(target+wm->getBall())/2);
//	double myTime2 =  wm->getPlayerTimeTo(WO_TEAMMATE1-1+wm->getUNum(),(target+wm->getBall())/2);
	double theirTime3 = wm->getPlayerTimeTo(wm->getOpponentFastestTo(wm->getBall())
					+WO_OPPONENT1-1,wm->getBall());
	double sum_Opp_Per = 0;
	int n = 0;
	for(int i = WO_OPPONENT1 ; i <= WO_OPPONENT11 ; i++)
	{
		if(!wm->getWorldObject(i)->validPosition)
			continue;
		double opp_per= perp( wm->getOpponent(i), wm->getBall() , target );
		//Min_Opp_Per = min(Min_Opp_Per , opp_per);
		sum_Opp_Per += exponential(opp_per,0.25);
		n++;
	}
	//if(theirTime  < myTime )
	//	return INF;
//	if(theirTime2 < myTime2)
	//	return INF;
	double ang = (target - wm->getMyPosition()).getAngleWithVector(
			VecPosition(1,wm->getMyAngRad(),0,POLAR));

	//if( theirTime3 < ang / 35)
		//return INF;
/*
	if(Min_Opp_Per < 0.3) {
		return INF;
	}
*/
		/*if(theirTime < 1.5)
		return INF;*/
	if(wm->isOut(target+VecPosition(-0.5,0,0,POLAR))) {
		NaoBehavior::debug->success(wm->getCycle(),"OUTTTt");
		return INF;
	}
return (n==0)?0:sum_Opp_Per/n;
 }
double dribble::effectiveness() {
	VecPosition oppGoal = wm->getOppLeftGoalPost()+wm->getOppRightGoalPost();
	oppGoal/=2;
	double theirGoal =1 - exponential(target.getDistanceTo(oppGoal) ,C_TheirGoal );
	//double surroundingOpponent = surroundingOpponents();
	return (theirGoal);// + surroundingOpponent * surrP)/(TheirGoalP+surrP);
}

double dribble::calcCost() {
	double ret = effP * effectiveness()+safeP*dribbleSafety() + surrP*dribbleReliability();
	ret /= (effP+safeP+surrP);
	ret*=factor;
	if(wm->getPlayMode() != PM_PLAY_ON)
		return INF;
	return cost =ret;
}
ndribble::ndribble(WorldModel *wm,formationLoader *fm):skilldesc(SKILL_DRIBBLE){
	this->wm = wm;
	this->fm = fm;
	angle = 0;
	shift = 0;
	this->wm = wm;
	this->fm = fm;
	map<string, double> factors;
	loadParameters(fm->getStrategy() + "/dribble.io", factors);


				factor = factors["factor"];
				if(!bii) {
					stringstream ss;

					string mess;
					ss>>mess;
					NaoBehavior::debug->debug(0,mess);
					bii = true;
				}
}
double ndribble::calcCost() {
	return cost = safety()+shift/90;
}
void ndribble::setAngle(double angle){
	VecPosition oppGoal = wm->getOppLeftGoalPost()+wm->getOppRightGoalPost();
		oppGoal/=2;
	this->angle = angle + (oppGoal - wm->getBall()).getAngleWithVector(
			VecPosition(1,wm->getMyAngRad(),0,POLAR));
	shift = fabs(angle);
	target = VecPosition(1,this->angle,0,POLAR)+wm->getBall();

}
double ndribble::getAngle() {
	return angle;
}
double ndribble::safety(){
	double time = angle/15;
	time+=2;
	for(int i = WO_OPPONENT1;i<=WO_OPPONENT11;i++) {
		if(wm->getPlayerTimeTo(i,target) < time) {
			return INF;
		}

	}
	return 0;
}
