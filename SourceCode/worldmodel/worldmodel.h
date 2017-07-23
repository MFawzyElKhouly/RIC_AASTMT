#ifndef WORLDMODEL_H
#define WORLDMODEL_H

#include "../headers/headers.h"
#include "../math/Geometry.h"
#include "../math/hctmatrix.h"
#include "WorldObject.h"
#include "../headers/Field.h"
#include "../rvdraw/rvdraw.h"

class BallKF;
class PlayerKF;

using namespace std;

class WorldModel {

private:

	unsigned long cycle;
	int scoreLeft;
	int scoreRight;
	double time;
	double gameTime;
	int playMode;
	int lastPlayMode;
	int lastDifferentPlayMode;
	int uNum;
	int side;

	bool uNumSet;
	bool sideSet;

	WorldObject worldObjects[NUM_WORLD_OBJS];
	VecPosition myPosition;
	SIM::AngDeg myAngDegrees;
	bool confident;

	VecPosition myLastPosition;
	SIM::AngDeg myLastAngDegrees;
	RVSender *rvsend;
	bool fUseGroundTruthDataForLocalization;

    int roles[11] ={0,1,1,1,1,2,2,2,2,3,3};
	// TODO: comment it out if we don't want ground truth
#define GROUND_TRUTH_SERVER
#ifdef GROUND_TRUTH_SERVER
	VecPosition myPositionGroundTruth;
	SIM::AngDeg myAngGroundTruth;
	VecPosition ballGroundTruth;
#endif

	double lastBallSightingTime;
	double lastLineSightingTime;


	// This is where we actually saw the ball ourself
	vector<VecPosition> lastBallSeenPosition;
	vector<double> lastBallSeenTime;

	// remember last two skills
	vector<SkillType> lastSkills;
	vector<SkillType> executedSkillsForOdometry;

	// record last odometry from particle filter
	SIM::Point2D lastOdometryPos;
	double lastOdometryAngDeg;

	bool fLocalized;
	bool fFallen;

	HCTMatrix localToGlobal, globalToLocal;

	vector<bool> fallenTeammate;
	vector<bool> fallenOpponent;

	string opponentTeamName;

	BallKF* ballKalmanFilter;
	PlayerKF* opponentKalmanFilters;

public:
	double ourV;
		double theirV;
		double ourW;
		double theirW;
	WorldModel();
	~WorldModel();

	inline void setMyConfidence(bool confidence) {
		confident = confidence;
	}
	inline bool getMyConfidence() const {
		return confident;
	}

	inline WorldObject* getWorldObject(int index) {
		return &worldObjects[index];
	}
	void updateGoalPostsAndFlags();
	void updateMatricesAndMovingObjs(VecPosition& fieldXPlusYPlus,
			VecPosition& fieldXPlusYMinus, VecPosition& fieldXMinusYPlus,
			VecPosition& fieldXMinusYMinus);

	inline void setMyPosition(const VecPosition& newPos) {
		myPosition = newPos;
	}
	inline void setMyPosition(const SIM::Point2D& newPos) {
		myPosition.setX(newPos.getX());
		myPosition.setY(newPos.getY());
		// Z is not changed, stays at the default
	}
	inline VecPosition getMyPosition() const {
		return myPosition;
	}

	inline void setMyAngDeg(SIM::AngDeg newAng) {
		myAngDegrees = newAng;
	}
	inline void setMyAngRad(SIM::AngRad newAng) {
		myAngDegrees = Rad2Deg(newAng);
	}
	inline SIM::AngDeg getMyAngDeg() const {
		return myAngDegrees;
	}
	inline SIM::AngRad getMyAngRad() const {
		return Deg2Rad(myAngDegrees);
	}
	inline double distancetoBall(VecPosition p) {
		p.setZ(0);
		return p.getDistanceTo(this->getBall());
	}
	VecPosition predictBall (float time);
	VecPosition predictPlayer (int modelNumber,float time);

	inline void setMyLastPosition(const VecPosition& newPos) {
		myLastPosition = newPos;
	}
	inline VecPosition getMyLastPosition() const {
		return myLastPosition;
	}

	inline void setMyLastAngDeg(SIM::AngDeg newAng) {
		myLastAngDegrees = newAng;
	}
	inline void setMyLastAngRad(SIM::AngRad newAng) {
		myLastAngDegrees = Rad2Deg(newAng);
	}
	inline SIM::AngDeg getMyLastAngDeg() const {
		return myLastAngDegrees;
	}
	inline SIM::AngRad getMyLastAngRad() const {
		return Deg2Rad(myLastAngDegrees);
	}

	inline RVSender *getRVSender() const {
		return rvsend;
	}
	  inline int getRole(int i ) {
	    	return roles[i];
	    }
	inline void setUseGroundTruthDataForLocalization(
			bool fUseGroundTruthDataForLocalization) {
		this->fUseGroundTruthDataForLocalization =
				fUseGroundTruthDataForLocalization;
	}
	inline bool useGroundTruthDataForLocalization() {
		return fUseGroundTruthDataForLocalization;
	}

#ifdef GROUND_TRUTH_SERVER
	inline void setMyPositionGroundTruth(const VecPosition& newPos) {
		myPositionGroundTruth = newPos;
	}
	inline VecPosition getMyPositionGroundTruth() const {
		return myPositionGroundTruth;
	}

	inline void setMyAngDegGroundTruth(double angDeg) {
		myAngGroundTruth = angDeg;
	}
	inline double getMyAngDegGroundTruth() const {
		return myAngGroundTruth;
	}

	inline void setBallGroundTruth(VecPosition newBall) {
		ballGroundTruth = newBall;
	}
	inline VecPosition getBallGroundTruth() const {
		return ballGroundTruth;
	}
#endif

	inline void setLastBallSightingTime(double lastTime) {
		lastBallSightingTime = lastTime;
	}
	inline double getLastBallSightingTime() const {
		return lastBallSightingTime;
	}

	inline void setLastLineSightingTime(double lastTime) {
		lastLineSightingTime = lastTime;
	}
	inline double getLastLineSightingTime() const {
		return lastLineSightingTime;
	}

	inline void setLastBallSeenPosition(VecPosition position) {
		lastBallSeenPosition.insert(lastBallSeenPosition.begin(), position);
		lastBallSeenPosition.pop_back();
	}
	inline vector<VecPosition> getLastBallSeenPosition() const {
		return lastBallSeenPosition;
	}

	inline void setLastBallSeenTime(double time) {
		lastBallSeenTime.insert(lastBallSeenTime.begin(), time);
		lastBallSeenTime.pop_back();
	}

	inline vector<double> getLastBallSeenTime() const {
		return lastBallSeenTime;
	}

	inline unsigned long getCycle() const {
		return cycle;
	}
	inline void incrementCycle() {
		cycle++;
	}
	inline void setCycle(const unsigned long &cycle) {
		this->cycle = cycle;
	}

	inline int getScoreLeft() const {
		return scoreLeft;
	}
	inline void setScoreLeft(const int &scoreLeft) {
		this->scoreLeft = scoreLeft;
	}

	inline int getScoreRight() const {
		return scoreRight;
	}
	inline void setScoreRight(const int &scoreRight) {
		this->scoreRight = scoreRight;
	}

	inline double getTime() const {
		return time;
	}
	inline void setTime(const double &time) {
		this->time = time;
	}

	inline double getGameTime() const {
		return gameTime;
	}
	inline void setGameTime(const double &gameTime) {
		this->gameTime = gameTime;
	}

	inline int getPlayMode() const {
		return playMode;
	}
	inline void setPlayMode(const int &playMode) {
		this->playMode = playMode;
	}

	inline int getLastPlayMode() const {
		return lastPlayMode;
	}
	inline void setLastPlayMode(const int &lastPlayMode) {
		this->lastPlayMode = lastPlayMode;
	}

	inline int getLastDifferentPlayMode() const {
		return lastDifferentPlayMode;
	}
	inline void setLastDifferentPlayMode(const int &lastDifferentPlayMode) {
		this->lastDifferentPlayMode = lastDifferentPlayMode;
	}

	inline int getUNum() const {
		return uNum;
	}
	inline void setUNum(const int &uNum) {
		this->uNum = uNum;
	}
	inline bool isOut(const VecPosition& p) {
		return !(p.getX() > -HALF_FIELD_X  && p.getX() < HALF_FIELD_X
				&&p.getY() > -HALF_FIELD_Y && p.getY() <HALF_FIELD_Y );
	}
	inline bool validPosition(const int &i) {
		return worldObjects[i].validPosition && !isOut(worldObjects[i].pos);
	}
	inline bool getUNumSet() const {
		return uNumSet;
	}
	inline void setUNumSet(const bool &uNumSet) {
		this->uNumSet = uNumSet;
	}

	inline SkillType getLastSkill() const {
		return lastSkills[0];
	}
	inline SkillType getPreviousLastSkill() const {
		return lastSkills[1];
	}
	inline void setLastSkill(const SkillType &lastSkill) {
		this->lastSkills[1] = this->lastSkills[0];
		this->lastSkills[0] = lastSkill;
	}

	// functions for odometry
	inline void addExecutedSkill(const SkillType &skill) {
		executedSkillsForOdometry.push_back(skill);
	}
	inline const vector<SkillType>& getExecutedSkills() const {
		return executedSkillsForOdometry;
	}
	inline void resetExecutedSkills() {
		executedSkillsForOdometry.clear();
	}
	inline double getPlayerTimeTo(int player, VecPosition p) {
		double v, w;
		if (player >= WO_TEAMMATE1 && player <= WO_TEAMMATE11)
			v = ourV, w = ourW;
		else
			v = theirV, w = theirW;

		VecPosition dis = p - worldObjects[player].pos;
		double len = dis.getMagnitude();
		double ang = dis.getPhi() - worldObjects[player].orien;
		if(ang  > 180)
			ang-=360;
		if(ang < 180)
			ang+=360;
		if (ang < 0)
			ang *=-1;
		double t = 0;
		t += ang / w;
		t += len / v;
		return t;
	}

	// tracking odometry
	inline SIM::Point2D& getLastOdometryPos() {
		return lastOdometryPos;
	}
	inline void setLastOdometryPos(const SIM::Point2D& pos) {
		lastOdometryPos = pos;
	}

	inline double& getLastOdometryAngDeg() {
		return lastOdometryAngDeg;
	}
	inline void setLastOdometryAngDeg(const double& ang) {
		lastOdometryAngDeg = ang;
	}

	// THIS IS A SINGLE POINT DETERMINES WHETHER USING BALL KALMAN FILTER
	inline bool useKalmanFilter() {
		return true;
	}

	inline int getSide() {
		return side;
	}
	inline void setSide(const int &side) {
		this->side = side;
		updateGoalPostsAndFlags();
	}

	inline bool getSideSet() {
		return sideSet;
	}
	inline void setSideSet(const bool &sideSet) {
		this->sideSet = sideSet;
	}

	inline VecPosition getGoalPost(const int &i) const {
		return worldObjects[GOALPOST_1_L + i].pos;
	}
	;

	inline VecPosition getMyLeftGoalPost() {
		return ((side == SIDE_LEFT) ?
				worldObjects[GOALPOST_1_L].pos : worldObjects[GOALPOST_2_R].pos);
	}
	inline VecPosition getMyRightGoalPost() {
		return ((side == SIDE_LEFT) ?
				worldObjects[GOALPOST_2_L].pos : worldObjects[GOALPOST_1_R].pos);
	}
	inline VecPosition getOppLeftGoalPost() {
		return ((side == SIDE_LEFT) ?
				worldObjects[GOALPOST_1_R].pos : worldObjects[GOALPOST_2_L].pos);
	}
	inline VecPosition getOppRightGoalPost() {
		return ((side == SIDE_LEFT) ?
				worldObjects[GOALPOST_2_R].pos : worldObjects[GOALPOST_1_L].pos);
	}

	inline double distanceToOppGoal(VecPosition &p) {

		VecPosition oppLeftGoalPost = getOppLeftGoalPost();
		VecPosition oppRightGoalPost = getOppRightGoalPost();

		if (p.getY() > oppLeftGoalPost.getY()) {
			return p.getDistanceTo(oppLeftGoalPost);
		} else if (p.getY() < oppRightGoalPost.getY()) {
			return p.getDistanceTo(oppRightGoalPost);
		} else {
			return fabs(oppLeftGoalPost.getX() - p.getX());
		}
	}

	inline double distanceToMyGoal(VecPosition &p) {

		VecPosition myLeftGoalPost = getMyLeftGoalPost();
		VecPosition myRightGoalPost = getMyRightGoalPost();

		if (p.getY() > myLeftGoalPost.getY()) {
			return p.getDistanceTo(myLeftGoalPost);
		} else if (p.getY() < myRightGoalPost.getY()) {
			return p.getDistanceTo(myRightGoalPost);
		} else {
			return fabs(myLeftGoalPost.getX() - p.getX());
		}
	}

	inline VecPosition getBall() const {
		return worldObjects[WO_BALL].pos;
	}
	;
	inline void setBall(const VecPosition &ball) {
		worldObjects[WO_BALL].pos = ball;
	}

	inline VecPosition getBallWrtTorso() const {
		return g2l(getBall());
	}

	inline VecPosition getTeammate(const int &i) const {
		if (i - WO_TEAMMATE1 + 1 == getUNum())
			return getMyPosition();
		return worldObjects[i].pos;
	}
	;
	inline void setTeammate(const int &i, const VecPosition &teammate) {
		worldObjects[i].pos = teammate;
	}
	inline int getTeammateClosestTo(VecPosition p) {

		int playerClosestToBall = -1;
		double closestDistanceToBall = 10000;
		for (int i = WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; ++i) {
			VecPosition temp;
			int playerNum = i - WO_TEAMMATE1 + 1;
			if (getUNum() == playerNum) {
				// This is us
				temp = getMyPosition();
			} else {
				WorldObject* teammate = getWorldObject(i);
				if (teammate->validPosition && !getFallenTeammate(i)) {
					temp = teammate->pos;
				} else {
					continue;
				}
			}
			temp.setZ(0);

			double distanceToBall = temp.getDistanceTo(p);
			if (distanceToBall < closestDistanceToBall) {
				playerClosestToBall = playerNum;
				closestDistanceToBall = distanceToBall;
			}
		}
		return playerClosestToBall;
	}
	inline int getTeammateFastestTo(VecPosition p) {

		int playerClosestToBall = -1;
		double closestDistanceToBall = 10000;
		for (int i = WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; ++i) {
			VecPosition temp;
			int playerNum = i - WO_TEAMMATE1 + 1;
			if (getUNum() == playerNum) {
				// This is us
				temp = getMyPosition();
			} else {
				WorldObject* teammate = getWorldObject(i);
				if (teammate->validPosition && !getFallenTeammate(i)) {
					temp = teammate->pos;
				} else {
					continue;
				}
			}
			temp.setZ(0);

			double timeToTarget = getPlayerTimeTo(i, p);
			if (timeToTarget < closestDistanceToBall) {
				playerClosestToBall = playerNum;
				closestDistanceToBall = timeToTarget;
			}
		}
		return playerClosestToBall;
	}
	inline VecPosition getOpponentBodyFastestTo(VecPosition p) {
		int playerClosestToBall = -1;
				double closestDistanceToBall = 10000;
				for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; ++i) {
					VecPosition temp;
					int playerNum = i - WO_OPPONENT1 + 1;

					WorldObject* teammate = getWorldObject(i);
					if (teammate->validPosition) {
						temp = teammate->pos;
					} else {
						continue;
					}

					temp.setZ(0);

					double distanceToBall = temp.getDistanceTo(p);
					if (distanceToBall < closestDistanceToBall) {
						playerClosestToBall = playerNum;
						closestDistanceToBall = distanceToBall;
					}
				}
			return getOpponent(playerClosestToBall+WO_OPPONENT1-1);
		}
	inline VecPosition getTeammateBodyFastestTo(VecPosition p) {
			int playerClosestToBall = -1;
					double closestDistanceToBall = 10000;
					for (int i = WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; ++i) {
						VecPosition temp;
						int playerNum = i - WO_TEAMMATE1 + 1;

						WorldObject* teammate = getWorldObject(i);
						if (teammate->validPosition) {
							temp = teammate->pos;
						} else {
							continue;
						}

						temp.setZ(0);

						double distanceToBall = temp.getDistanceTo(p);
						if (distanceToBall < closestDistanceToBall) {
							playerClosestToBall = playerNum;
							closestDistanceToBall = distanceToBall;
						}
					}
				return getOpponent(playerClosestToBall+WO_TEAMMATE1-1);
			}
	inline int getOpponentFastestTo(VecPosition p) {

		int playerClosestToBall = -1;
		double closestDistanceToBall = 10000;
		for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; ++i) {
			VecPosition temp;
			int playerNum = i - WO_OPPONENT1 + 1;

			WorldObject* teammate = getWorldObject(i);
			if (teammate->validPosition && !getFallenOpponent(i)) {
				temp = teammate->pos;
			} else {
				continue;
			}

			temp.setZ(0);

			double timeToTarget = getPlayerTimeTo(i, p);
			if (timeToTarget < closestDistanceToBall) {
				playerClosestToBall = playerNum;
				closestDistanceToBall = timeToTarget;
			}
		}
		return playerClosestToBall;
	}
	inline int getOpponentClosestTo(VecPosition p) {

		int playerClosestToBall = -1;
		double closestDistanceToBall = 10000;
		for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; ++i) {
			VecPosition temp;
			int playerNum = i - WO_OPPONENT1 + 1;

			WorldObject* teammate = getWorldObject(i);
			if (teammate->validPosition && !getFallenOpponent(i)) {
				temp = teammate->pos;
			} else {
				continue;
			}

			temp.setZ(0);

			double distanceToBall = temp.getDistanceTo(p);
			if (distanceToBall < closestDistanceToBall) {
				playerClosestToBall = playerNum;
				closestDistanceToBall = distanceToBall;
			}
		}
		return playerClosestToBall;
	}
	inline VecPosition getOpponent(const int &i) const {
		return worldObjects[i].pos;
	}
	inline int getBallHolder() {
		int teammate = getTeammateClosestTo(getBall())+WO_TEAMMATE1-1;
		int opponent = getOpponentClosestTo(getBall())+WO_OPPONENT1-1;
		if(getTeammate(teammate).getDistanceTo(getBall()) <
				getOpponent(opponent).getDistanceTo(getBall()))
			return teammate;
		else
			return opponent;

	}
	inline bool hasBall() {
		return(getBallHolder() < WO_OPPONENT1);
	}
	inline void setOpponent(const int &i, const VecPosition &opponent) {
		worldObjects[i].pos = opponent;
	}

	inline void setObjectPosition(const int &i, const VecPosition &pos) {
		worldObjects[i].pos = pos;
	}

	inline void setGlobalToLocal(const int &i, const int &j, const double &v) {
		this->globalToLocal.setCell(i, j, v);
	}

	inline void setLocalToGlobal(const int &i, const int &j, const double &v) {
		this->localToGlobal.setCell(i, j, v);
	}

	inline bool isLocalized() const {
		return fLocalized;
	}
	inline void setLocalized(bool fLocalized) {
		this->fLocalized = fLocalized;
	}

	inline bool isFallen() const {
		return fFallen;
	}
	inline void setFallen(bool fFallen) {
		this->fFallen = fFallen;
	}

	inline VecPosition g2l(const VecPosition &global) const {
		return globalToLocal.transform(global);
	}
	inline VecPosition l2g(const VecPosition &local) const {
		return localToGlobal.transform(local);
	}

	inline bool canTrustVision() {
		return fLocalized
				&& getMyPosition().getDistanceTo(getMyLastPosition()) < .2;
	}

	bool getFallenTeammate(int index) const {
		return fallenTeammate[index];
	}
	void setFallenTeammate(int index, bool fFallen) {
		fallenTeammate[index] = fFallen;
	}
	bool getFallenOpponent(int index) const {
		return fallenOpponent[index];
	}
	void setFallenOpponent(int index, bool fFallen) {
		fallenOpponent[index] = fFallen;
	}

	string getOpponentTeamName() const {
		return opponentTeamName;
	}
	void setOpponentTeamName(string name) {
		opponentTeamName = name;
	}

	BallKF* getBallKalmanFilter() const {
		return ballKalmanFilter;
	}
	PlayerKF* getOpponentKalmanFilters() const {
		return opponentKalmanFilters;
	}

	void display();
};

#endif // WORLDMODEL_H

