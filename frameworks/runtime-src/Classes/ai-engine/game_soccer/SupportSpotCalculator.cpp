
#include "SupportSpotCalculator.h"
#include "PlayerBase.h"
#include "SoccerGoal.h"
#include "SoccerBall.h"
#include "SoccerTeam.h"
#include "SoccerPitch.h"
#include "ParaConfigSoccer.h"
#include "common/misc/Regulator.h"

//------------------------------- ctor ----------------------------------------
//-----------------------------------------------------------------------------
SupportSpotCalculator::SupportSpotCalculator(int numX, int numY, SoccerTeam* team):
                                                                                                    m_pBestSupportingSpot(NULL),
                                                                                                    m_pTeam(team)
{
    const Region* PlayingField = team->getPitch()->getPlayingArea();

    //calculate the positions of each sweet spot, create them and store them in m_Spots
    float HeightOfSSRegion = PlayingField->height() * 0.8;
    float WidthOfSSRegion  = PlayingField->width() * 0.9;
    float SliceX = WidthOfSSRegion / numX ;
    float SliceY = HeightOfSSRegion / numY;

    float left  = PlayingField->left() + (PlayingField->width()-WidthOfSSRegion)/2.0 + SliceX/2.0;
    float right = PlayingField->right() - (PlayingField->width()-WidthOfSSRegion)/2.0 - SliceX/2.0;
    float top   = PlayingField->top() + (PlayingField->height()-HeightOfSSRegion)/2.0 + SliceY/2.0;

    for (int x=0; x<(numX/2)-1; ++x)
    {
        for (int y=0; y<numY; ++y)
        {      
            if (m_pTeam->getTeamColor() == SoccerTeam::blue)
            {
                m_Spots.push_back(SupportSpot(Vector2D(left+x*SliceX, top+y*SliceY), 0.0));
            }

            else
            {
                m_Spots.push_back(SupportSpot(Vector2D(right-x*SliceX, top+y*SliceY), 0.0));
            }
        }
    }

    m_pRegulator = new Regulator(Para_SupportSpotUpdateFreq);
}

SupportSpotCalculator::~SupportSpotCalculator()
{
    delete m_pRegulator;
}

//--------------------------- determineBestSupportingPosition -----------------
//this method iterates through each possible spot and calculates its score.
//-----------------------------------------------------------------------------
Vector2D SupportSpotCalculator::determineBestSupportingPosition()
{
    //only update the spots every few frames                              
    if (!m_pRegulator->isReady() && m_pBestSupportingSpot)
    {
        return m_pBestSupportingSpot->m_vPos;
    }

    //reset the best supporting spot
    m_pBestSupportingSpot = NULL;

    float BestScoreSoFar = 0.0f;

    std::vector<SupportSpot>::iterator curSpot;

    for (curSpot = m_Spots.begin(); curSpot != m_Spots.end(); ++curSpot)
    {
        //first remove any previous score. (the score is set to one so that
        //the viewer can see the positions of all the spots if he has the 
        //aids turned on)
        curSpot->m_dScore = 1.0;

        //Test 1. is it possible to make a safe pass from the ball's position to this position?
        if(m_pTeam->isPassSafeFromAllOpponents(  m_pTeam->getControllingPlayer()->getPos(),
                                                                           curSpot->m_vPos,
                                                                           NULL,
                                                                           Para_MaxPassingForce))
        {
            curSpot->m_dScore += Para_Spot_CanPassScore;
        }


        //Test 2. Determine if a goal can be scored from this position.  
        if( m_pTeam->canShoot(curSpot->m_vPos, Para_MaxShootingForce))
        {
            curSpot->m_dScore += Para_Spot_CanScoreFromPositionScore;
        }   


        //Test 3. calculate how far this spot is away from the controlling
        //player. The further away, the higher the score. Any distances further
        //away than OptimalDistance pixels do not receive a score.
        if (m_pTeam->getSupportingPlayer())
        {
            const float OptimalDistance = 200.0f;

            float dist = Vec2Distance(m_pTeam->getControllingPlayer()->getPos(), curSpot->m_vPos);
            float temp = fabs(OptimalDistance - dist);

            if (temp < OptimalDistance)
            {
                //normalize the distance and add it to the score
                curSpot->m_dScore += Para_Spot_DistFromControllingPlayerScore *
                                                    (OptimalDistance-temp)/OptimalDistance;  
            }
        }

        //check to see if this spot has the highest score so far
        if (curSpot->m_dScore > BestScoreSoFar)
        {
            BestScoreSoFar = curSpot->m_dScore;
            m_pBestSupportingSpot = &(*curSpot);
        }    
    }

    return m_pBestSupportingSpot->m_vPos;
}


Vector2D SupportSpotCalculator::getBestSupportingSpot()
{
    if (m_pBestSupportingSpot)
    {
        return m_pBestSupportingSpot->m_vPos;
    }

    else
    { 
        return determineBestSupportingPosition();
    }
}
