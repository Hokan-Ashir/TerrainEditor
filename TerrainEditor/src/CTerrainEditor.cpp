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
    CTerrainEditor::CTerrainEditor(IrrlichtDevice* pDevice, core::stringw heightMapName, f32 terrainSceneNodeScaleFactor) : wireframeMode(false) {
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
                core::vector3df(10, 3, 10), // scale
                video::SColor(255, 255, 255, 255), // vertexColor
                5, // maxLOD
                scene::ETPS_17, // patchSize
                3 // smoothFactor
                );
        pTerrainSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);

        pTerrainSceneNode->scaleTexture(1.0f, 20.0f);

        pTerrainSceneNodeSelector = pSceneManager->createTerrainTriangleSelector(pTerrainSceneNode, 0);

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

        pMultiTexturingManager = new CMultiTexturingManager(pSceneManager);

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
                0);

        pBrushManager = new CBrushManager(pVideoDriver, pTerrainSceneNode);
    }

    /**
     * Virtual destructor
     * Drops only terrain collision selector
     */
    CTerrainEditor::~CTerrainEditor() {
        pTerrainSceneNodeSelector->drop();
    }

    /**
     * Draws everything that correnspondes to terrain (brushes, blended textures, shadows, lights etc.)
     */
    void CTerrainEditor::drawAll() {
        pMultiTexturingManager->drawAll();
        pBrushManager->drawAll();
    }

    /**
     * Paints terrain node via CMultiTexturingManager, using brush from CBrushManage
     * 
     * @param clickPosition click position of mouse on screen
     */
    void CTerrainEditor::textureTerrainWithCurrentBrush(const core::position2di clickPosition) {
        const core::line3df ray = pSceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(clickPosition, pCameraSceneNode);
        core::vector3df pos;
        core::triangle3df Tri;
        scene::ISceneNode* node = 0;
        if (pSceneManager->getSceneCollisionManager()->getCollisionPoint(ray, pTerrainSceneNodeSelector, pos, Tri, node)) {
            //arrow->setPosition(pos);
            //static const s32 scale = /*32*/1; // pTerrainSceneNode is scaled 32X
            //static const s32 size = 257; // heightmap is 512x512 pixels
            //printf("X: %f \t Z: %f\n", pos.X, pos.Z);
            s32 x = (s32) (pos.X / terrainSceneNodeScaleFactor);
            s32 z = (s32) (pos.Z / terrainSceneNodeScaleFactor);
            //drawCircleBrushBorder(pVideoDriver, pos / 10, 10.0f);
            //s32 index = x * size + z;
            video::ITexture* Tex = first_pass->splat_texture; //pVideoDriver->getTexture("Data/textures/splat_1.tga");
            first_pass->splat_texture = textureBrush(x, z, Tex);
        }

    }

    /**
     * Gets intersection position with terrain mesh by ray traced from user mouse click position
     * 
     * @param clickPosition mouse click position on the screen
     * @return intersection position (WITHOUT terrain scale factor affected)
     */
    core::vector3df CTerrainEditor::getIntersectionPositionWithTerrain(const core::position2di clickPosition) {
        const core::line3df ray = pSceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(clickPosition, pCameraSceneNode);
        core::vector3df pos;
        core::triangle3df Tri;
        scene::ISceneNode* node = 0;
        if (pSceneManager->getSceneCollisionManager()->getCollisionPoint(ray, pTerrainSceneNodeSelector, pos, Tri, node)) {
            //arrow->setPosition(pos);
            //static const s32 scale = /*32*/1; // pTerrainSceneNode is scaled 32X
            //static const s32 size = 257; // heightmap is 512x512 pixels
            //printf("X: %f \t Z: %f\n", pos.X, pos.Z);
            s32 x = (s32) (pos.X / terrainSceneNodeScaleFactor);
            s32 z = (s32) (pos.Z / terrainSceneNodeScaleFactor);
            //drawCircleBrushBorder(pVideoDriver, pos / 10, 10.0f);
            //s32 index = x * size + z;
        }
        return pos;
    }

    /**
     * Saves terrain current height map
     * 
     * @param heightMapName file name of current height map
     */
    void CTerrainEditor::saveTerrainHeightMap(core::stringc heightMapName) {
        core::dimension2d<u32> dim(256, 256);
        video::IImage *img = pVideoDriver->createImage(video::ECF_R8G8B8, dim);

        u32 VertexCount = pTerrainSceneNode->getMesh()->getMeshBuffer(0)->getVertexCount();
        video::S3DVertex2TCoords* verts = (video::S3DVertex2TCoords*)pTerrainSceneNode->getMesh()->getMeshBuffer(0)->getVertices();

        for (u32 i = 0; i < VertexCount; ++i) {
            video::S3DVertex2TCoords* vertex = verts + i;
            u8 y = (u8) vertex->Pos.Y;
            img->setPixel((u32) vertex->Pos.X, (u32) vertex->Pos.Z, video::SColor(0, y, y, y));
        }

        core::stringc fileName = "texture/";
        fileName += heightMapName;
        fileName += ".bmp";
        pVideoDriver->writeImageToFile(img, fileName, 0);
        img->drop();
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

        for (int y = 0; y < brushHeight; ++y) {
            for (int x = 0; x < brushWidth; ++x) {
                video::SColor brushPixel = pBrushManager->getCurrentBrush()->getPixel(x, y);

                f32 bp = brushPixel.getRed() / 255.0;

                if (bp > 0) {
                    if ((index - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth)) >= 0 &&
                            (index - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth)) <= 512 * 512)
                        pPixels[(index - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth))].set(255, 0, 255, 0);
                }
            }
        }
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
            }
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


} // end of namespace irr

