#include "pkbehaviors.h"

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
/////// GOALIE
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

PKGoalieBehavior::
PKGoalieBehavior( const std::string teamName,
                  int uNum,
                  const map<string, string>& namedParams_,
                  const string& rsg_)
    : NaoBehavior( teamName,
                   uNum,
                   namedParams_,
                   rsg_) {
}

void PKGoalieBehavior::
beam( double& beamX, double& beamY, double& beamAngle ) {
    beamX = -HALF_FIELD_X+.5;
    beamY = 0;
    beamAngle = 0;
}




SkillType PKGoalieBehavior::
selectSkill() {

	VecPosition target;
	double oldX=0.0,oldY=0.0;
	double timeforGK=0.0;

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
			// Limit to penalty area

				if(worldModel->getMyPosition().getX()>-12.2)
				{
					target.setX(-12.2);
				}

				if(fabs(worldModel->getMyPosition().getY())>3)
				{
					if(worldModel->getMyPosition().getY()>0)
						target.setY(3);
					else
						target.setY(-3);
				}
			return goToTarget(target);
		}
		oldX = newX;
		oldY = newY;
		return kickBall(KICK_IK, VecPosition(newX, newY, 0));
	}
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
}





////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
/////// SHOOTER
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
PKShooterBehavior::
PKShooterBehavior( const std::string teamName,
                   int uNum,
                   const map<string, string>& namedParams_,
                   const string& rsg_ )
    : NaoBehavior( teamName,
                   uNum,
                   namedParams_,
                   rsg_ ) {
}

void PKShooterBehavior::
beam( double& beamX, double& beamY, double& beamAngle ) {
    beamX = -0.5;
    beamY = 0;
    beamAngle = 0;
}

SkillType PKShooterBehavior::
selectSkill() {
    return SKILL_STAND;
}
