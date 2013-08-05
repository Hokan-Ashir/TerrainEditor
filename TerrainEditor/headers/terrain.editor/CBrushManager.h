/* 
 * File:   CBrushManager.h
 * Author: hokan
 *
 * Created on 26 Июль 2013 г., 13:04
 */

#ifndef CBRUSHMANAGER_H
#define	CBRUSHMANAGER_H

#include <IEventReceiver.h>
#include <IVideoDriver.h>
#include <ITerrainSceneNode.h>
#include "../../headers/terrain.editor/decal.system/DecalManager.h"
#include "../../headers/terrain.editor/decal.system/DecalSceneNode.h"

namespace irr {

    /**
     * Class for managment brushes - loading, changing brushes, changing parameters, drawing
     */
    class CBrushManager : public IEventReceiver {
    public:
        /**
         * Constructor
         * 
         * @param pVideoDriver  pointer to IVideoDriver interface
         * @param pTerrainSceneNode     pointer to ITerrainSceneNode interface; over which brushes draws
         */
        CBrushManager(video::IVideoDriver* pVideoDriver, scene::ITerrainSceneNode* pTerrainSceneNode, DecalManager* pDecalManager);
        
        /**
         * Copy constructor (not implemented)
         * 
         * @param orig  reference to original class instance
         */
        CBrushManager(const CBrushManager& orig) {};
        
        /**
         * Virtual destuctor
         */
        virtual ~CBrushManager();
        
        void constructBrush();
        
        void createDecalNode(core::vector3df nodePosition, core::vector3df nodeNormal);
        
        /**
         * Draws everything - brush, its border, any other suff
         */
        void drawAll();
        
        /**
         * Draws square border over terrain node
         * 
         * @param brushCenter   coordinates of brush center
         */
        void drawSquareBrushBorder(core::vector3df brushCenter);
        
        /**
         * Draws circle border over terrain node
         * 
         * @param brushCenter   coordinates of brush center
         */
        void drawCircleBrushBorder(core::vector3df brushCenter);
        
        video::ITexture* paintTextureWithBrush(s32 vertexXCoordinate, s32 vertexZCoordinate, video::ITexture* paintingTexture);
        
        void raiseVerticesWithBrush(s32 vertexXCoordinate, s32 vertexZCoordinate, bool up);
        
        /**
         * Operates events affecting on brush - changing parameters, drawing etc.
         * 
         * @param event incomming event
         * @return true if incomming event processed
         */
        virtual bool OnEvent(const SEvent& event);
        
        /**
         * Getter
         * 
         * @return current brush angle around Y-axis
         */
        f32 getBrushYAngle() const;
        
        /**
         * Setter
         * 
         * @param brushYAngle new brush angle aroung Y-axis
         */
        void setBrushYAngle(f32 brushYAngle);
        
        /**
         * Getter
         * 
         * @return current brush radius
         */
        f32 getBrushRadius() const;
        
        /**
         * Setter
         * 
         * @param brushRadius new brush radius
         */
        void setBrushRadius(f32 brushRadius);
        
        /**
         * Getter
         * 
         * @return current brush maximum radius
         */
        f32 getBrushMaximumRadius() const;
        
        /**
         * Setter
         * 
         * @param brushMaximumRadius new brush maximum radius
         */
        void setBrushMaximumRadius(f32 brushMaximumRadius);
        
        /**
         * Getter
         * 
         * @return current brush minimum radius
         */
        f32 getBrushMinimumRadius() const;
        
        /**
         * Setter
         * 
         * @param brushMinimumRadius new brush minimum radius
         */
        void setBrushMinimumRadius(f32 brushMinimumRadius);
        
        /**
         * Getter
         * 
         * @return current brush border colour
         */
        video::SColor getBrushBorderColour() const;
        
        /**
         * Setter
         * 
         * @param borderColour new brush border colour
         */
        void setBrushBorderColour(video::SColor borderColour);         
        
        /**
         * Getter
         * 
         * @return pointer to ITexture interface representing current brush
         */
        video:: ITexture* getCurrentBrush() const;
    private:
        
        bool isLCtrlButtonPressed;
        
        bool isLAltButtonPressed;
        
        void outOfTerrainPerimeterCheck(f32* x, f32* z);
        
        /**
         * Pointer to ITexture interface representing current brush
         */
        video::ITexture* pBrush;
        
        /**
         * Pointer to IVideoDriver interface
         */
        video::IVideoDriver* pVideoDriver;
        
        /**
         * Pointer to ITerrainSceneNode interface representing terrain node over wich brush will be drawn
         */
        scene::ITerrainSceneNode* pTerrainSceneNode;
        
        /**
         * Current brush angle around Y-axis
         */
        f32 brushYAngle;
        
        /**
         * Current brush radius
         */
        f32 brushRadius;       
        
        /**
         * Current brush maximum radius
         */
        f32 brushMaximumRadius;
        
        /**
         * Current brush minimum radius
         */
        f32 brushMinimumRadius;
        
        /**
         * Current brush border colour
         */
        video::SColor borderColour;
        
        f32 raiseStep;
        
        DecalManager* pDecalManager;
        
        scene::DecalSceneNode* pDecalSceneNode;  
    };
} // end of namespace irr 

#endif	/* CBRUSHMANAGER_H */

