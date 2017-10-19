
#ifndef VEHICLESTEERINGCONFIG_H
#define VEHICLESTEERINGCONFIG_H 


#define Para_MaxSpeed           150.0f
#define Para_VehicleMass        1.0f
#define Para_MaxForce           400


//use these values to tweak the amount that each steering force
//contributes to the total steering force
#define Para_Weight_Separation          1.0f
#define Para_Weight_Alignment           1.0f
#define Para_Weight_Cohesion            2.0f
#define Para_Weight_ObstacleAvoidance   10.0f
#define Para_Weight_WallAvoidance       10.0f
#define Para_Weight_Wander              1.0f
#define Para_Weight_Seek                1.0f
#define Para_Weight_Flee                1.0f
#define Para_Weight_Arrive              1.0f
#define Para_Weight_Pursuit             1.0f
#define Para_Weight_OffsetPursuit      1.0f
#define Para_Weight_Interpose          1.0f
#define Para_Weight_Hide                1.0f
#define Para_Weight_Evade              0.01f
#define Para_Weight_FollowPath          0.05f

//this is used to multiply the steering force AND all the multipliers
//found in SteeringBehavior
#define Para_SteeringForceTweaker     200.0f

#define GetWeight(w) (w * Para_SteeringForceTweaker )



//how close a neighbour must be before an agent perceives it (considers it
//to be within its neighborhood)
#define Para_ViewDistance              50.0f

//used in obstacle avoidance
#define Para_MinDetectionBoxLength     40.0f

//used in wall avoidance
#define Para_WallDetectionFeelerLength 40.0f

//the radius of the constraining circle for the wander behavior
#define Para_WanderRadius    1.2f

//distance the wander circle is projected in front of the agent
#define Para_WanderDist    2.0f

//the maximum amount of displacement along the circle each frame
#define Para_WanderJitterPerSec  80.0f

//used in path following
#define Para_WaypointSeekDist    20

//these are the probabilities that a steering behavior will be used
//when the Prioritized Dither calculate method is used to sum
//combined behaviors
#define Para_Dither_WallAvoidance       0.5f
#define Para_Dither_ObstacleAvoidance   0.5f
#define Para_Dither_Separation                0.2f
#define Para_Dither_Alignment                 0.3f
#define Para_Dither_Cohesion                  0.6f
#define Para_Dither_Wander                    0.8f
#define Para_Dither_Seek                      0.8f
#define Para_Dither_Flee                      0.6f
#define Para_Dither_Evade                     1.0f
#define Para_Dither_Hide                      0.8f
#define Para_Dither_Arrive                    0.5f



#endif 
