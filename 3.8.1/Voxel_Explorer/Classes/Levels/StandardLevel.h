//
//  StandardLevel.h
//  Voxel_Explorer
//
//  Created by wang haibo on 15/9/24.
//
//

#ifndef __Voxel_Explorer__StandardLevel__
#define __Voxel_Explorer__StandardLevel__

#include "BaseLevel.h"
#include "Area.h"

typedef enum
{
    LS_STANDARD = 0,
    LS_TUNNEL,
    LS_PASSAGE,
    LS_UNKNOWN
} LevelStyle;

class StandardLevel : public BaseLevel
{
protected:
    StandardLevel();
    
    virtual bool build();
    virtual bool createTerrain();
    virtual bool decorate();

    virtual bool initAreas();
    virtual void splitArea(const cocos2d::Rect& rect);
    virtual void assignAreasType();
    
    virtual void generate();
    
    virtual void showMap(bool show);
    
    virtual void updateAreaFogOfWarByPos(const cocos2d::Vec2& pos);
    virtual bool generatePickableItemsByPos(const cocos2d::Vec2& pos, int num);
protected:
    void placeDoors(Area* area);
    void generateDoors(Area* area);
    bool mergeSmallIntersectArea(Area* area, Area* other, bool generate = true);
    void placeTraps();  ///放置陷阱
    void generateSpawnPoint();
    int randomMonsterRespawnCell();
    int randomPickableRespawnCell();
protected:
    std::vector<PathGraphNode*>    m_Areas;
    Area*               m_AreaEntrance;
    Area*               m_AreaExit;
    std::vector<Area::AREA_TYPE>       m_SpecailAreas;
    int                 m_nSecretDoors;
    
    int                 m_nMinAreaSize;
    int                 m_nMaxAreaSize;
    
    int                 m_nStandardAreaCount;
    int                 m_nSpecialAreaCount;
    int                 m_nTunnelAreaCount;
    int                 m_nPassageAreaCount;
    
    LevelStyle          m_Style;
};

#endif /* defined(__Voxel_Explorer__StandardLevel__) */
