/* 
 * File:   CTerrainEditor.h
 * Author: hokan
 *
 * Created on 26 Июль 2013 г., 13:06
 */

#ifndef CTERRAINEDITOR_H
#define	CTERRAINEDITOR_H

#include <IEventReceiver.h>
#include <ITerrainSceneNode.h>
#include <ISceneManager.h>
#include "terrain.editor/CMultiTexturingManager.h"
#include "terrain.editor/CBrushManager.h"

namespace irr {

    class CTerrainEditor : public IEventReceiver {
    public:
        CTerrainEditor(IrrlichtDevice* pDevice, core::stringw heightMapName, f32 terrainSceneNodeScaleFactor);
        CTerrainEditor(const CTerrainEditor& orig);
        virtual ~CTerrainEditor();
        void textureTerrainWithCurrentBrush(const core::position2di clickPosition);
        void saveTerrainHeightMap(core::stringc heightMapName);        
        virtual bool OnEvent(const SEvent& event);
        void drawAll();
        CBrushManager* getBrushManager() const;
        scene::ICameraSceneNode* getCameraSceneNode() const;
        scene::ITerrainSceneNode* getTerrainSceneNode() const;
    private:
        CMultiTexturingManager::STexturePass *first_pass;
        core::vector3df getIntersectionPositionWithTerrain(const core::position2di clickPosition);
        f32 terrainSceneNodeScaleFactor;
        video::ITexture* textureBrush(s32 vertexXCoordinate, s32 vertexZCoordinate, video::ITexture* texture);
        video::IVideoDriver* pVideoDriver;
        scene::ISceneManager* pSceneManager;
        scene::ITerrainSceneNode* pTerrainSceneNode;
        bool wireframeMode;
        scene::ICameraSceneNode* pCameraSceneNode;
        scene::ITriangleSelector* pTerrainSceneNodeSelector;
        CMultiTexturingManager* pMultiTexturingManager;
        CBrushManager* pBrushManager;
    };
} // end of namespace irr

#endif	/* CTERRAINEDITOR_H */

