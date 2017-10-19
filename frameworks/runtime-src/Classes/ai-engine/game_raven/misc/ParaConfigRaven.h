
#ifndef PARACONFIGRAVEN_H
#define PARACONFIGRAVEN_H 

typedef enum {
    type_wall,
    type_bot,
    type_unused,
    type_waypoint,
    type_health,
    type_spawn_point,
    type_rail_gun,
    type_rocket_launcher,
    type_shotgun,
    type_blaster,
    type_obstacle,
    type_sliding_door,
    type_door_trigger
}entityType;

#define Para_StartMap  "maps/Raven_DM1.map"


typedef enum {
    smooth_quick,
    smooth_precise
}smoothType;

//-------------------------[[ General game parameters ]]-------------------------
//-------------------------------------------------------------------------------

//the number of bots the game instantiates

#define Para_NumBots   3

//this is the maximum number of search cycles allocated to *all* current path
//planning searches per update
#define Para_MaxSearchCyclesPerUpdateStep  1000

//the name of the default map
#define Para_StartMap  "maps/Raven_DM1.map"

//cell space partitioning defaults
#define Para_NumCellsX  10
#define Para_NumCellsY  10

//how long the graves remain on screen
#define Para_GraveLifetime   5


//-------------------------[[ pathfinder parameters ]]----------------------------------
//-------------------------------------------------------------------------------
#define para_path_smooth_quick      1
#define para_path_smooth_precise    0



//-------------------------[[ bot parameters ]]----------------------------------
//-------------------------------------------------------------------------------

#define Para_Bot_MaxHealth   100.0f
#define Para_Bot_MaxSpeed    1.0f
#define Para_Bot_Mass        1.0f
#define Para_Bot_MaxForce    1.0f
#define Para_Bot_MaxHeadTurnRate   0.2f
#define Para_Bot_Scale         0.8f

//special movement speeds (unused)
#define Para_Bot_MaxSwimmingSpeed   (Para_Bot_MaxSpeed * 0.2f)
#define Para_Bot_MaxCrawlingSpeed   (Para_Bot_MaxSpeed * 0.6f)

//the number of times a second a bot 'thinks' about weapon selection
#define Para_Bot_WeaponSelectionFrequency   2

//the number of times a second a bot 'thinks' about changing strategy
#define Para_Bot_GoalAppraisalUpdateFreq   4

//the number of times a second a bot updates its target info
#define Para_Bot_TargetingUpdateFreq   2

//the number of times a second the triggers are updated
#define Para_Bot_TriggerUpdateFreq   8

//the number of times a second a bot updates its vision
#define Para_Bot_VisionUpdateFreq   4

//note that a frequency of -1 will disable the feature and a frequency of zero
//will ensure the feature is updated every bot update


//the bot's field of view (in degrees)
#define Para_Bot_FOV   180

//the bot's reaction time (in seconds)
#define Para_Bot_ReactionTime   0.2f

//how long (in seconds) the bot will keep pointing its weapon at its target
//after the target goes out of view
#define Para_Bot_AimPersistance   1

//how accurate the bots are at aiming. 0 is very accurate, (the value represents
//the max deviation in range (in radians))
#define Para_Bot_AimAccuracy   0.0f

//how long a flash is displayed when the bot is hit
#define Para_HitFlashTime   0.2f

//how long (in seconds) a bot's sensory memory persists
#define Para_Bot_MemorySpan   5

//goal tweakers
#define Para_Bot_HealthGoalTweaker       1.0f
#define Para_Bot_ShotgunGoalTweaker      1.0f
#define Para_Bot_RailgunGoalTweaker      1.0f
#define Para_Bot_RocketLauncherTweaker   1.0f
#define Para_Bot_AggroGoalTweaker        1.0f

//-------------------------[[ steering parameters ]]-----------------------------
//-------------------------------------------------------------------------------
//use these values to tweak the amount that each steering force
//contributes to the total steering force
#define Para_SeparationWeight           10.0f
#define Para_WallAvoidanceWeight        10.0f
#define Para_WanderWeight               1.0f
#define Para_SeekWeight                 0.5f
#define Para_ArriveWeight               1.0f

//how close a neighbour must be before an agent considers it
//to be within its neighborhood (for separation)
#define Para_ViewDistance               15.0f

//max feeler length
#define Para_WallDetectionFeelerLength   (25.0f * Para_Bot_Scale)

//used in path following. Determines how close a bot must be to a waypoint
//before it seeks the next waypoint
#define Para_WaypointSeekDist     5.0f

//-------------------------[[ giver-trigger parameters ]]-----------------------------
//-------------------------------------------------------------------------------

//how close a bot must be to a giver-trigger for it to affect it
#define Para_DefaultGiverTriggerRange    10.0f

//how many milliseconds before a giver-trigger reactivates itself
#define Para_Health_RespawnDelay     10000  //10 sec
#define Para_Weapon_RespawnDelay     15000 //15 sec

#define Para_Health_Given    50



//-------------------------[[ weapon parameters ]]-------------------------------
//-------------------------------------------------------------------------------
#define Para_Blaster_FiringFreq         3
#define Para_Blaster_MaxSpeed		   5
#define Para_Blaster_DefaultRounds      0  //not used, a blaster always has ammo
#define Para_Blaster_MaxRoundsCarried   0 //as above
#define Para_Blaster_IdealRange		     50
#define Para_Blaster_SoundRange	       100

#define Para_Bolt_MaxSpeed      5
#define Para_Bolt_Mass          1
#define Para_Bolt_MaxForce      100.0f
#define Para_Bolt_Scale         Para_Bot_Scale
#define Para_Bolt_Damage        1

#define Para_RocketLauncher_FiringFreq         1.5f
#define Para_RocketLauncher_DefaultRounds        15
#define Para_RocketLauncher_MaxRoundsCarried   50
#define Para_RocketLauncher_IdealRange    150
#define Para_RocketLauncher_SoundRange    400

#define Para_Rocket_BlastRadius   20
#define Para_Rocket_MaxSpeed      3
#define Para_Rocket_Mass          1
#define Para_Rocket_MaxForce      10.0f
#define Para_Rocket_Scale         Para_Bot_Scale
#define Para_Rocket_Damage        10
#define Para_Rocket_ExplosionDecayRate   2.0f   //how fast the explosion occurs (in secs)

#define Para_RailGun_FiringFreq         1
#define Para_RailGun_DefaultRounds        15
#define Para_RailGun_MaxRoundsCarried   50
#define Para_RailGun_IdealRange    200
#define Para_RailGun_SoundRange    400

#define Para_Slug_MaxSpeed      5000
#define Para_Slug_Mass          0.1f
#define Para_Slug_MaxForce      10000.0f
#define Para_Slug_Scale         Para_Bot_Scale
#define Para_Slug_Persistance   0.2f
#define Para_Slug_Damage        10

#define Para_ShotGun_FiringFreq         1
#define Para_ShotGun_DefaultRounds        15
#define Para_ShotGun_MaxRoundsCarried   50
#define Para_ShotGun_NumBallsInShell    10
#define Para_ShotGun_Spread             0.05f
#define Para_ShotGun_IdealRange    100
#define Para_ShotGun_SoundRange    400

#define Para_Pellet_MaxSpeed      5000
#define Para_Pellet_Mass          0.1f
#define Para_Pellet_MaxForce      1000.0f
#define Para_Pellet_Scale         Para_Bot_Scale
#define Para_Pellet_Persistance   0.1f
#define Para_Pellet_Damage        1


#define Para_Weaopn_Sound_LiftTime  7.0f

#endif

