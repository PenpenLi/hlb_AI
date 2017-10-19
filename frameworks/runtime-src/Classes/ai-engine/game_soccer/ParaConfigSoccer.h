#ifndef PARACONFIGSOCCER_H
#define PARACONFIGSOCCER_H 

#define Invoid_Not_Overlap      false 

//defines the size of a team -- do not adjust
#define Para_TeamSize 5

#define Para_PlayerKickingAccuracy   0.99f


#define Para_GoalWidth 100



#define Para_BallSize   5.0f
#define Para_BallMass   1.0f
#define Para_BallFriction    -0.015f 

//the goalkeeper has to be this close to the ball to be able to interact with it
#define Para_KeeperInBallRange      10.0f
#define Para_PlayerInTargetRange    10.0f

//how close the ball must be to a receiver before he starts chasing it
#define Para_BallWithinReceivingRange   10.0f

//player has to be this close to the ball to be able to kick it. The higher
//the value this gets, the easier it gets to tackle. 
#define Para_PlayerKickingDistance      10.0f

//the number of times a player can kick the ball per second
#define Para_PlayerKickFrequency         8

#define Para_PlayerMass     3.0f
#define Para_PlayerMaxForce     1.0f
#define Para_PlayerMaxSpeedWithBall     1.4f
#define Para_PlayerMaxSpeedWithoutBall      2.2f
#define Para_PlayerMaxTurnRate      0.4f

//when an opponents comes within this range the player will attempt to pass
//the ball. Players tend to pass more often, the higher the value
#define Para_PlayerComfortZone      60.0f

//the chance a player might take a random pot shot at the goal
#define Para_ChancePlayerAttemptsPotShot     0.005f

//this is the chance that a player will receive a pass using the arrive
//steering behavior, rather than Pursuit
#define Para_ChanceOfUsingArriveTypeReceiveBehavior     0.5f

//how many times per second the support spots will be calculated
#define Para_SupportSpotUpdateFreq  1

//the number of times the SoccerTeam::canShoot method attempts to find a valid shot
#define Para_NumAttemptsToFindValidStrike    5

#define Para_MaxDribbleForce    2.0f
#define Para_MaxShootingForce   8.0f
#define Para_MaxPassingForce    4.0f

//the minimum distance a receiving player must be from the passing player
#define Para_MinPassDistance    120.0f
//the minimum distance a player must be from the goalkeeper before it will pass the ball
#define Para_GoalkeeperMinPassDistance  50.0f


//use to set up the sweet spot calculator
#define Para_NumSupportSpotsX     13
#define Para_NumSupportSpotsY     6

//these values tweak the various rules used to calculate the support spots
#define Para_Spot_CanPassScore   2.0f
#define Para_Spot_CanScoreFromPositionScore     1.0f
#define Para_Spot_DistFromControllingPlayerScore    2.0f
#define Para_Spot_ClosenessToSupportingPlayerScore      0.0f
#define Para_Spot_AheadOfAttackerScore      0.0f

//the minimum distance a player must be from the goalkeeper before it will pass the ball
#define Para_GoalkeeperMinPassDistance  50.0f

//this is the distance the keeper puts between the back of the net 
//and the ball when using the interpose steering behavior
#define Para_GoalKeeperTendingDistance  20.0f

//when the ball becomes within this distance of the goalkeeper he changes state to intercept the ball
#define Para_GoalKeeperInterceptRange   100.0f


//--------------------------------------------steering behavior stuff
#define Para_SeparationCoefficient      5.0f

//how close a neighbour must be to be considered for separation
#define Para_ViewDistance       36.0f

// 1=ON; 0=OFF
#define bNonPenetrationConstraint      0


#endif 
