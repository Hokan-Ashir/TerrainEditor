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
#include "terrain.editor/decal.system/DecalManager.h"

namespace irr {

    /**
     * Class that implements all stuff for creation & manipulating terrain like
     * verticies lifting, texture blending etc. via different classes
     */
    class CTerrainEditor : public IEventReceiver {
    public:
        /**
         * Constructor
         * 
         * @param pDevice pointer to IrrlichtDevice class instance
         * @param heightMapName height map name which will be used to construct terrain
         * @param terrainSceneNodeScaleFactor   terrain scale factor
         */
        CTerrainEditor(IrrlichtDevice* pDevice, core::stringw heightMapName, f32 terrainSceneNodeScaleFactor);

        /**
         * Copy constructor (not implemented)
         * 
         * @param orig reference to original class instance
         */
        CTerrainEditor(const CTerrainEditor& orig) {
        };

        /**
         * Virtual destructor
         */
        virtual ~CTerrainEditor();

        /**
         * Paints terrain node via CMultiTexturingManager, using brush from CBrushManage
         * 
         * @param clickPosition click position of mouse on screen
         */
        void textureTerrainWithCurrentBrush(const core::vector3df intersectionPosition);
        
        /**
         * Gets intersection position with terrain mesh by ray traced from user mouse click position
         * 
         * @param clickPosition mouse click position on the screen
         * @return intersection position (WITHOUT terrain scale factor affected)
         */
        core::vector3df* getIntersectionPositionWithTerrain(const core::position2di clickPosition);

        /**
         * Saves terrain current height map
         * 
         * @param heightMapName file name of current height map
         */
        void saveTerrainHeightMap(core::stringc heightMapName);

        /**
         * Operates events affecting on terrain itself - changing wireframe view and other little stuff
         * 
         * @param event incomming event
         * @return true if incomming event processed
         */
        virtual bool OnEvent(const SEvent& event);

        /**
         * Draws everything that correnspondes to terrain (brushes, blended textures, shadows, lights etc.)
         */
        void drawAll();

        /**
         * Getter
         * 
         * @return current brush manager
         */
        CBrushManager* getBrushManager() const;

        /**
         * Getter
         * 
         * @return  current camera scene node
         */
        scene::ICameraSceneNode* getCameraSceneNode() const;

        /**
         * Getter
         * 
         * @return current terrain scene node itself
         */
        scene::ITerrainSceneNode* getTerrainSceneNode() const;
        
        DecalManager* getDecalManager() const;
        
        /**
         * Getter
         * 
         * @return current terrain scale factor
         */
        f32 getTerrainScaleFactor() const;
        
        /**
         * Setter
         * 
         * @param terrainScaleFactor new terrain scale factor
         */
        void setTerrainScaleFactor(f32 terrainScaleFactor);
        
        /**
         * Getter
         * 
         * @return current edit mode value
         */
        bool getEditMode() const;

        /**
         * Setter
         * 
         * @param editMode new value of edit mode flag
         */
        void setEditMode(bool editMode);
    private:
        f32 size;
        
        /**
         * Texture pass from CMultiTexturingManager which will be edited by brush
         */
        CMultiTexturingManager::STexturePass *first_pass;

        /**
         * Current terrain scale factor
         */
        f32 terrainSceneNodeScaleFactor;
        
        /**
         * Current flag value representing edit mode
         */
        bool editMode;
        
        core::vector3df* intersectionPosition;

        /**
         * Paint pTerrainSceneNode node with brush
         *
         * @param vertexXCoordinate			center position of brush on X-axis
         * @param vertexZCoordinate			center position of brush on Z-axis
         * @param texture	ITexture pointer which instance user to paint pTerrainSceneNode node
         * @return			pointer to result, painted texture
         */
        video::ITexture* textureBrush(s32 vertexXCoordinate, s32 vertexZCoordinate, video::ITexture* texture);
        
        video::ITexture* textureBrush2(s32 vertexXCoordinate, s32 vertexZCoordinate, video::ITexture* texture);

        IrrlichtDevice* pDevice;
        /**
         * Pointer to IVideoDriver interface class instance
         */
        video::IVideoDriver* pVideoDriver;

        /**
         * Pointer to ISceneManager interface class instance
         */
        scene::ISceneManager* pSceneManager;

        /**
         * Pointer to ITerrainSceneNode interface class instance
         */
        scene::ITerrainSceneNode* pTerrainSceneNode;

        /**
         * If set terrain node will rendered via wireframe flag, without textures
         */
        bool wireframeMode;

        /**
         * Pointer to ICameraSceneNode interface class instance
         */
        scene::ICameraSceneNode* pCameraSceneNode;

        /**
         * Pointer to ITriangleSelector interface class instance
         * Represents selector over all terrain
         */
        scene::ITriangleSelector* pTerrainSceneNodeSelector;

        /**
         * Pointer to CMultiTexturingManager class instance
         */
        CMultiTexturingManager* pMultiTexturingManager;

        /**
         * Pointer to CBrushManager class instance
         */
        CBrushManager* pBrushManager;
        
        DecalManager* pDecalManager;
    };
} // end of namespace irr

#endif	/* CTERRAINEDITOR_H */

