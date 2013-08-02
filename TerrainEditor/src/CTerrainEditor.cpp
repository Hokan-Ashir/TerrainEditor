/* 
 * File:   CTerrainEditor.cpp
 * Author: hokan
 * 
 * Created on 26 Июль 2013 г., 13:06
 */

#include "../headers/CTerrainEditor.h"
#include "../headers/terrain.editor/CBrushManager.h"
#include "../headers/terrain.editor/CMultiTexturingManager.h"
#include "../headers/terrain.editor/CSceneNodeAnimatorCameraTerrain.h"
#include "TextureTools.h"

namespace irr {

    /**
     * Constructor
     * Create terrain node itself, camera node (set GOT - Glide Over Terrain animator) 
     * & initialize MultiTexturingManager so as BrushManager
     * 
     * @param pDevice pointer to IrrlichtDevice class instance
     * @param heightMapName height map name which will be used to construct terrain
     * @param terrainSceneNodeScaleFactor   terrain scale factor
     */
    CTerrainEditor::CTerrainEditor(IrrlichtDevice* pDevice, core::stringw heightMapName, f32 terrainSceneNodeScaleFactor) :
    wireframeMode(false), editMode(false), intersectionPosition(0), size(32) {
        this->pDevice = pDevice;
        this->pVideoDriver = pDevice->getVideoDriver();
        this->pSceneManager = pDevice->getSceneManager();
        this->terrainSceneNodeScaleFactor = terrainSceneNodeScaleFactor;
        // create a pTerrainSceneNode
        pTerrainSceneNode = pSceneManager->addTerrainSceneNode(
                heightMapName, // height map
                0, // parent node
                -1, // node id
                core::vector3df(0.f, 2.f, 0.f), // position
                core::vector3df(0.f, 0.f, 0.f), // rotation
                core::vector3df(terrainSceneNodeScaleFactor, 3, terrainSceneNodeScaleFactor), // scale
                video::SColor(255, 255, 255, 255), // vertexColor
                5, // maxLOD
                scene::ETPS_17, // patchSize
                3 // smoothFactor
                );
        pTerrainSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);

        pTerrainSceneNode->setMaterialTexture(0, pVideoDriver->getTexture("media/terrain-texture.jpg"));
        pTerrainSceneNode->setMaterialTexture(1, pVideoDriver->getTexture("media/detailmap3.jpg"));

        pTerrainSceneNode->setMaterialType(video::EMT_DETAIL_MAP);

        pTerrainSceneNode->scaleTexture(1.0f, 100.0f);
        //pTerrainSceneNode->scaleTexture(1.0f, 20.0f);

        pTerrainSceneNodeSelector = pSceneManager->createTerrainTriangleSelector(pTerrainSceneNode, 0);
        pTerrainSceneNode->setTriangleSelector(pTerrainSceneNodeSelector);
        pTerrainSceneNodeSelector->drop();

        pCameraSceneNode = pSceneManager->addCameraSceneNode(); //addCameraSceneNodeFPS(0, 100.0f, .1f);
        scene::CSceneNodeAnimatorCameraTerrain* pGOTCameraAnimator =
                new scene::CSceneNodeAnimatorCameraTerrain(pDevice->getCursorControl(), pTerrainSceneNode);
        pCameraSceneNode->addAnimator(pGOTCameraAnimator);
        pGOTCameraAnimator->drop();
        core::vector3df cameraTarget = pTerrainSceneNode->getTerrainCenter();
        cameraTarget.Y += 10000;
        pCameraSceneNode->setPosition(cameraTarget);
        cameraTarget.Y -= 10000;
        pCameraSceneNode->setTarget(cameraTarget);
        pCameraSceneNode->setFarValue(18000);

        /*pMultiTexturingManager = new CMultiTexturingManager(pSceneManager);

        //Add it to the manager
        pMultiTexturingManager->addNode(pTerrainSceneNode);

        //Set the passes
        //First Pass, with Sand, Gras and Rock
        first_pass = pMultiTexturingManager->addPass(pTerrainSceneNode,
                pVideoDriver->getTexture("Data/textures/splat_1.tga"),
                pVideoDriver->getTexture("Data/textures/Rock.jpg"),
                pVideoDriver->getTexture("Data/textures/Gras.jpg"),
                pVideoDriver->getTexture("Data/textures/Sand.jpg"));

        //Second Pass with Snow, Mud and the Vulcano's base texture
        pMultiTexturingManager->addPass(pTerrainSceneNode,
                pVideoDriver->getTexture("Data/textures/splat_2.tga"),
                pVideoDriver->getTexture("Data/textures/Snow.jpg"),
                pVideoDriver->getTexture("Data/textures/Mud.jpg"),
                pVideoDriver->getTexture("Data/textures/Ashes.jpg"));

        //Third Pass: the base
        pMultiTexturingManager->addPass(pTerrainSceneNode,
                pVideoDriver->getTexture("Data/textures/splat_3.tga"),
                0,
                0,
                pVideoDriver->getTexture("Data/textures/Pebbles.jpg"));

        //Fourth Pass: the Lightmap
        pMultiTexturingManager->addPass(pTerrainSceneNode,
                pVideoDriver->getTexture("Data/textures/lm_terrain.tga"),
                pVideoDriver->getTexture("Data/textures/black.jpg"),
                0,
                0);

        //Fith Pass: Lava, drawn above the Lightmap to make it "glow"
        CMultiTexturingManager::STexturePass *anim_lava_pass = pMultiTexturingManager->addPass(pTerrainSceneNode,
                pVideoDriver->getTexture("Data/textures/splat_4.tga"),
                pVideoDriver->getTexture("Data/textures/lava_1.jpg"),
                0,
                0);*/

        pBrushManager = new CBrushManager(pVideoDriver, pTerrainSceneNode);

        pDecalManager = new DecalManager(pDevice);
        pDecalManager->setTerrain(pTerrainSceneNode);
    }

    /**
     * Virtual destructor
     * Drops only terrain collision selector
     */
    CTerrainEditor::~CTerrainEditor() {
        if (intersectionPosition != 0) {
            delete intersectionPosition;
        }
        //pTerrainSceneNodeSelector->drop();
    }

    /**
     * Draws everything that correnspondes to terrain (brushes, blended textures, shadows, lights etc.)
     */
    void CTerrainEditor::drawAll() {
        //pMultiTexturingManager->drawAll();
        if (editMode) {
            if (intersectionPosition != 0) {
                pBrushManager->drawCircleBrushBorder(core::vector3df(intersectionPosition->X * terrainSceneNodeScaleFactor,
                        intersectionPosition->Y,
                        intersectionPosition->Z * terrainSceneNodeScaleFactor));
                /*pBrushManager->drawSquareBrushBorder(core::vector3df(intersectionPosition->X * terrainSceneNodeScaleFactor,
                        intersectionPosition->Y,
                        intersectionPosition->Z * terrainSceneNodeScaleFactor));*/
            }
        }

        //pBrushManager->drawAll();
    }

    /**
     * Paints terrain node via CMultiTexturingManager, using brush from CBrushManage
     * 
     * @param clickPosition click position of mouse on screen
     */
    void CTerrainEditor::textureTerrainWithCurrentBrush(const core::vector3df intersectionPosition) {
        video::ITexture* Tex = first_pass->splat_texture; //pVideoDriver->getTexture("Data/textures/splat_1.tga");
        first_pass->splat_texture = textureBrush2(intersectionPosition.X, intersectionPosition.Z, Tex);
    }

    /**
     * Gets intersection position with terrain mesh by ray traced from user mouse click position
     * 
     * @param clickPosition mouse click position on the screen
     * @return intersection position (WITHOUT terrain scale factor affected)
     */
    core::vector3df* CTerrainEditor::getIntersectionPositionWithTerrain(const core::position2di clickPosition) {
        const core::line3df rayFromScreenCoordinates = pSceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(clickPosition, pCameraSceneNode);
        core::vector3df* intersectionPosition = new core::vector3df();
        core::triangle3df intersectionTriangle;
        scene::ISceneNode* intersectionNode = 0;
        if (pSceneManager->getSceneCollisionManager()->getCollisionPoint(rayFromScreenCoordinates,
                pTerrainSceneNodeSelector,
                (core::vector3df&) * intersectionPosition,
                intersectionTriangle,
                intersectionNode)) {
            //arrow->setPosition(pos);
            //static const s32 scale = /*32*/1; // pTerrainSceneNode is scaled 32X
            //static const s32 size = 257; // heightmap is 512x512 pixels
            //printf("X: %f \t Z: %f\n", pos.X, pos.Z);
            s32 x = (s32) (intersectionPosition->X / terrainSceneNodeScaleFactor);
            s32 z = (s32) (intersectionPosition->Z / terrainSceneNodeScaleFactor);
            intersectionPosition->X = x;
            intersectionPosition->Z = z;

            return intersectionPosition;
            //drawCircleBrushBorder(pVideoDriver, pos / 10, 10.0f);
            //s32 index = x * size + z;
        }
        return 0;
    }

    /**
     * Saves terrain current height map
     * 
     * @param heightMapName file name of current height map
     */
    void CTerrainEditor::saveTerrainHeightMap(core::stringc heightMapName) {
        core::dimension2d<u32> terrainDimension(pTerrainSceneNode->getBoundingBox().MaxEdge.X / pTerrainSceneNode->getScale().X,
                pTerrainSceneNode->getBoundingBox().MaxEdge.Z / pTerrainSceneNode->getScale().Z);
        video::IImage *terrainHeightMapImage = pVideoDriver->createImage(video::ECF_R8G8B8, terrainDimension);

        u32 vertexCount = pTerrainSceneNode->getMesh()->getMeshBuffer(0)->getVertexCount();
        video::S3DVertex2TCoords* verticies = (video::S3DVertex2TCoords*)pTerrainSceneNode->getMesh()->getMeshBuffer(0)->getVertices();

        for (u32 i = 0; i < vertexCount; ++i) {
            video::S3DVertex2TCoords* vertex = verticies + i;
            u8 y = (u8) vertex->Pos.Y;
            terrainHeightMapImage->setPixel((u32) vertex->Pos.X, (u32) vertex->Pos.Z, video::SColor(0, y, y, y));
        }

        core::stringc fileName = "texture/";
        fileName += heightMapName;
        fileName += ".png";
        pVideoDriver->writeImageToFile(terrainHeightMapImage, fileName, 0);
        terrainHeightMapImage->drop();
    }

    /**
     * Paint pTerrainSceneNode node with brush
     *
     * @param vertexXCoordinate			center position of brush on X-axis
     * @param vertexZCoordinate			center position of brush on Z-axis
     * @param texture	ITexture pointer which instance user to paint pTerrainSceneNode node
     * @return			pointer to result, painted texture
     */
    video::ITexture* CTerrainEditor::textureBrush(s32 vertexXCoordinate, s32 vertexZCoordinate, video::ITexture* texture) {
        s32 index = (vertexZCoordinate * 512 + (512 - vertexXCoordinate));
        s32 brushWidth = pBrushManager->getCurrentBrush()->getDimension().Width;
        s32 brushHeight = pBrushManager->getCurrentBrush()->getDimension().Height;
        s32 heightmapWidth = 512;

        video::SColor* pPixels = (video::SColor*)texture->lock();

        for (s32 y = 0; y < brushHeight; ++y) {
            for (s32 x = 0; x < brushWidth; ++x) {
                //video::SColor brushPixel = pBrushManager->getCurrentBrush()->getPixel(x, y);

                //f32 brushPixelRedComponent = brushPixel.getRed() / 255.0;

                if ((pBrushManager->getCurrentBrush()->getPixel(x, y).getRed() / 255.0) > 0) {
                    if ((index - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth)) >= 0 &&
                            (index - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth)) <= 512 * 512)
                        pPixels[(index - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth))].set(255, 0, 255, 0);
                }
            }
        }
        texture->unlock();

        return texture;
    }

    video::ITexture* CTerrainEditor::textureBrush2(s32 vertexXCoordinate, s32 vertexZCoordinate, video::ITexture* texture) {
        u32 terrainWidth = pTerrainSceneNode->getBoundingBox().MaxEdge.X / core::max_(pTerrainSceneNode->getScale().Z, pTerrainSceneNode->getScale().X);
        u32 terrainHeight = pTerrainSceneNode->getBoundingBox().MaxEdge.Z / core::max_(pTerrainSceneNode->getScale().Z, pTerrainSceneNode->getScale().X);
        u32 index = (vertexZCoordinate * terrainHeight + (terrainWidth - vertexXCoordinate));
        u32 brushWidth = size;
        u32 brushHeight = brushWidth;
        printf("brushWidth: %d\n", brushWidth);
        u32 terrainSize = terrainWidth * terrainHeight;
        // divide by two, cause it's one half of width-additional pixels
        // otherwise: 
        // (pBrushManager->getCurrentBrush()->getDimension().Width * pTerrainSceneNode->getScale().X
        // - pBrushManager->getCurrentBrush()->getDimension().Width) / 2
        // NOTE that Width field of IImage field is original with of file brush, that's why we use such equation
        u32 widthScaleCoefficient = (brushWidth - pBrushManager->getCurrentBrush()->getDimension().Width) / 2;
        if (widthScaleCoefficient == 0)
            widthScaleCoefficient = 1;
        printf("widthScaleCoefficient: %d\n", widthScaleCoefficient);
        // u32 widthScaleCoefficient = brushWidth / pTerrainSceneNode->getScale().X;
        // u32 heightScaleCoefficient = brushHeight / pTerrainSceneNode->getScale().Z;

        video::SColor* pPixels = (video::SColor*)texture->lock();

        index -= ((/*pBrushManager->getCurrentBrush()->getDimension().Width*/brushWidth / 2)
                + (/*pBrushManager->getCurrentBrush()->getDimension().Height*/brushHeight / 2 * terrainWidth));

        for (u32 x = 0; x < brushHeight; ++x) {
            for (u32 y = 0; y < brushWidth; ++y) {
                if ((pBrushManager->getCurrentBrush()->getPixel(x / widthScaleCoefficient, y / widthScaleCoefficient).getRed() / 255.0) > 0) {
                    if ((index + (y + x * terrainWidth)) >= 0
                            && (index + (y + x * terrainWidth)) <= terrainSize)
                        pPixels[index + (y + x * terrainWidth)].set(255, 0, 255, 0);
                }
                //pPixels[y + x * terrainWidth].set(255, 0, 255, 0);
            }
        }
        /*for (u32 i = 0; i < 256; ++i)
            pPixels[i].set(255, 0, 255, 0);*/
        texture->unlock();

        return texture;
    }

    /**
     * Operates events affecting on terrain itself - changing wireframe view and other little stuff
     * 
     * @param event incomming event
     * @return true if incomming event processed
     */
    bool CTerrainEditor::OnEvent(const SEvent& event) {
        if ((event.EventType == EET_KEY_INPUT_EVENT) && event.KeyInput.PressedDown) {
            if (event.KeyInput.Key == KEY_KEY_W) {
                wireframeMode = !wireframeMode;
                pTerrainSceneNode->setMaterialFlag(video::EMF_WIREFRAME, wireframeMode);
            } else if (event.KeyInput.Key == KEY_SPACE) {
                core::vector3df cameraTarget = pTerrainSceneNode->getTerrainCenter();
                cameraTarget.Y += 10000;
                pCameraSceneNode->setPosition(cameraTarget);
                cameraTarget.Y -= 10000;
                pCameraSceneNode->setTarget(cameraTarget);
            } else if (event.KeyInput.Key == KEY_F4) {
                size += 2;
            } else if (event.KeyInput.Key == KEY_F5) {
                size -= 2;
            }
        }

        if (editMode) {
            /*if ((event.EventType == EET_MOUSE_INPUT_EVENT)
                    && (event.MouseInput.Event == EMIE_MOUSE_MOVED || event.MouseInput.Event == EMIE_MOUSE_WHEEL)) {
                const core::position2di mousePosition = pDevice->getCursorControl()->getPosition();
                intersectionPosition = getIntersectionPositionWithTerrain(mousePosition);
                if (intersectionPosition != 0) {
                        printf("X: %f\tY: %fZ: %f\n", intersectionPosition->X, intersectionPosition->Y, intersectionPosition->Z);
                }
            }

            if ((event.EventType == EET_MOUSE_INPUT_EVENT) && (event.MouseInput.isLeftPressed() || event.MouseInput.isRightPressed())) {
                if (intersectionPosition != 0) {
                    // paint terrain with current brush if any mouse button pressed and editMode active                
                    textureTerrainWithCurrentBrush(*intersectionPosition);
                }
            }*/
        }
        return false;
    }

    /**
     * Getter
     * 
     * @return current brush manager
     */
    CBrushManager* CTerrainEditor::getBrushManager() const {
        return pBrushManager;
    }

    /**
     * Getter
     * 
     * @return  current camera scene node
     */
    scene::ICameraSceneNode* CTerrainEditor::getCameraSceneNode() const {
        return pCameraSceneNode;
    }

    /**
     * Getter
     * 
     * @return current terrain scene node itself
     */
    scene::ITerrainSceneNode* CTerrainEditor::getTerrainSceneNode() const {
        return pTerrainSceneNode;
    }

    DecalManager* CTerrainEditor::getDecalManager() const {
        return pDecalManager;
    }

    /**
     * Getter
     * 
     * @return current terrain scale factor
     */
    f32 CTerrainEditor::getTerrainScaleFactor() const {
        return terrainSceneNodeScaleFactor;
    }

    /**
     * Setter
     * 
     * @param terrainScaleFactor new terrain scale factor
     */
    void CTerrainEditor::setTerrainScaleFactor(f32 terrainScaleFactor) {
        this->terrainSceneNodeScaleFactor = terrainScaleFactor;
    }

    /**
     * Getter
     * 
     * @return current edit mode value
     */
    bool CTerrainEditor::getEditMode() const {
        return editMode;
    }

    /**
     * Setter
     * 
     * @param editMode new value of edit mode flag
     */
    void CTerrainEditor::setEditMode(bool editMode) {
        this->editMode = editMode;

        // Turn on/off mouse control in GOTAnimator in pCameraSceneNode
        core::list<scene::ISceneNodeAnimator*>::ConstIterator cameraAnimatorList = pCameraSceneNode->getAnimators().begin();
        scene::CSceneNodeAnimatorCameraTerrain *GOTCameraAnimator = (scene::CSceneNodeAnimatorCameraTerrain*) * cameraAnimatorList;
        GOTCameraAnimator->setMouseActive(!GOTCameraAnimator->getMouseActive());

        /*if (!this->editMode) {
            intersectionPosition = 0;
        }*/
    }


} // end of namespace irr

