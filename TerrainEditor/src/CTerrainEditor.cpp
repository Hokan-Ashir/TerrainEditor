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
//#include "TextureTools.h"

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
    wireframeMode(false), paintingEditMode(false), liftingEditMode(false), isLMBPressed(false), isRMBPressed(false) {
        this->pDevice = pDevice;
        this->pVideoDriver = pDevice->getVideoDriver();
        this->pSceneManager = pDevice->getSceneManager();
        collisionParameters.collisionDetected = false;
        // create a pTerrainSceneNode
        /*pTerrainSceneNode = pSceneManager->addTerrainSceneNode(
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

        /*pTerrainSceneNode->setMaterialTexture(0, pVideoDriver->getTexture("media/terrain-texture.jpg"));
        pTerrainSceneNode->setMaterialTexture(1, pVideoDriver->getTexture("media/detailmap3.jpg"));

        pTerrainSceneNode->setMaterialType(video::EMT_DETAIL_MAP);

        pTerrainSceneNode->scaleTexture(1.0f, 100.0f);*/
        //pTerrainSceneNode->scaleTexture(1.0f, 20.0f);

        pTerrainSceneNode
                = new scene::CMultiTexturingTerrainSceneNode(pDevice->getSceneManager(), heightMapName, // height map
                pSceneManager->getRootSceneNode(), // parent node
                -1, // node id
                core::vector3df(0.f, 0.f, 0.f), // position
                core::vector3df(0.f, 0.f, 0.f), // rotation
                core::vector3df(terrainSceneNodeScaleFactor, 3, terrainSceneNodeScaleFactor), // scale
                video::SColor(255, 255, 255, 255), // vertexColor
                5, // maxLOD
                scene::ETPS_17, // patchSize
                3 // smoothFactor
                );

        //pTerrainSceneNode->setDynamicSelectorUpdate(true);

        //scene::CDynamicMeshBuffer* buffer = new scene::CDynamicMeshBuffer(video::EVT_2TCOORDS, video::EIT_16BIT);
        //pTerrainSceneNode->getMeshBufferForLOD(*buffer, 0);
        //video::S3DVertex2TCoords* data = (video::S3DVertex2TCoords*)buffer->getVertexBuffer().getData();

        // Work on data or get the IndexBuffer with a similar call.
        //buffer->drop(); // When done drop the buffer again.

        /*pTerrainSceneNode->setMaterialTexture(0, pVideoDriver->getTexture("media/terrain-texture.jpg"));
        pTerrainSceneNode->setMaterialTexture(1, pVideoDriver->getTexture("media/detailmap3.jpg"));

        pTerrainSceneNode->setMaterialType(video::EMT_DETAIL_MAP);*/

        pTerrainSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);
        pTerrainSceneNode->scaleTexture(1.0f, 20.0f);

        scene::ITriangleSelector* pTerrainSceneNodeSelector = pSceneManager->createTerrainTriangleSelector(pTerrainSceneNode, 0);
        pTerrainSceneNode->setTriangleSelector(pTerrainSceneNodeSelector);
        pTerrainSceneNodeSelector->drop();

        pCameraSceneNode = pSceneManager->addCameraSceneNode(); //addCameraSceneNodeFPS(0, 100.0f, .1f);
        scene::CSceneNodeAnimatorCameraTerrain* pGOTCameraAnimator =
                new scene::CSceneNodeAnimatorCameraTerrain(pDevice->getCursorControl(), pTerrainSceneNode);
        pCameraSceneNode->addAnimator(pGOTCameraAnimator);
        pGOTCameraAnimator->drop();
        //core::vector3df cameraTarget = pTerrainSceneNode->getTerrainCenter();
        core::vector3df cameraTarget = core::vector3df(pTerrainSceneNode->getBoundingBox().MaxEdge.X - 1,
                pTerrainSceneNode->getHeight(pTerrainSceneNode->getBoundingBox().MaxEdge.X - 1, 0),
                0);
        cameraTarget.Y += 10000;
        pCameraSceneNode->setPosition(cameraTarget);
        cameraTarget.Y -= 10000;
        pCameraSceneNode->setTarget(cameraTarget);
        pCameraSceneNode->setFarValue(18000);

        // First pass: rock, sand and grass
        first_pass = pTerrainSceneNode->addPass(
                pVideoDriver->getTexture("Data/textures/splat_1.tga"),
                pVideoDriver->getTexture("Data/textures/Rock.jpg"),
                pVideoDriver->getTexture("Data/textures/Gras.jpg"),
                pVideoDriver->getTexture("Data/textures/Sand.jpg"));

        //Second Pass with Snow, Mud and the Vulcano's base texture
        pTerrainSceneNode->addPass(
                pVideoDriver->getTexture("Data/textures/splat_2.tga"),
                pVideoDriver->getTexture("Data/textures/Snow.jpg"),
                pVideoDriver->getTexture("Data/textures/Mud.jpg"),
                pVideoDriver->getTexture("Data/textures/Ashes.jpg"));

        //Third Pass: the base
        pTerrainSceneNode->addPass(
                pVideoDriver->getTexture("Data/textures/splat_3.tga"),
                0,
                0,
                pVideoDriver->getTexture("Data/textures/Pebbles.jpg"));

        //Fourth Pass: the Lightmap
        pTerrainSceneNode->addPass(
                pVideoDriver->getTexture("Data/textures/lm_terrain.tga"),
                pVideoDriver->getTexture("Data/textures/black.jpg"),
                0,
                0);

        //Fith Pass: Lava, drawn above the Lightmap to make it "glow"
        /*first_pass = */pTerrainSceneNode->addPass(
                pVideoDriver->getTexture("Data/textures/splat_4.tga"),
                pVideoDriver->getTexture("Data/textures/lava_1.jpg"),
                0,
                0);

        pDecalManager = new DecalManager(pDevice);
        pDecalManager->setTerrain(pTerrainSceneNode);

        pBrushManager = new CBrushManager(pVideoDriver, pTerrainSceneNode, pDecalManager);
    }

    /**
     * Virtual destructor
     * Drops only terrain collision selector
     */
    CTerrainEditor::~CTerrainEditor() {
        //pTerrainSceneNodeSelector->drop();
    }

    /**
     * Draws everything that correnspondes to terrain (brushes, blended textures, shadows, lights etc.)
     */
    void CTerrainEditor::drawAll() {
        //pMultiTexturingManager->drawAll();
        if (paintingEditMode || liftingEditMode) {
            if (collisionParameters.collisionDetected) {
                pBrushManager->drawBrushBorder(collisionParameters.collisionPosition);                
            }
        }

        if (liftingEditMode && collisionParameters.collisionDetected && (isLMBPressed || isRMBPressed)) {
            pBrushManager->raiseVerticesWithBrush(collisionParameters.collisionPosition.X / pTerrainSceneNode->getScale().X,
                    collisionParameters.collisionPosition.Z / pTerrainSceneNode->getScale().Z,
                    isLMBPressed);
            /*core::array<core::triangle3df> triangles;
            triangles.set_used(pTerrainSceneNode->getTriangleSelector()->getTriangleCount());
            s32 ntris = 0;
        
            pTerrainSceneNode->getTriangleSelector()->getTriangles(&triangles[0], triangles.size(), ntris);
            triangles[0].pointA.Y += 10;
            triangles[0].pointB.Y += 10;
            triangles[0].pointC.Y += 10;*/
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
        first_pass->splat_texture = pBrushManager->paintTextureWithBrush(intersectionPosition.X, intersectionPosition.Z, Tex);
    }

    /**
     * Gets intersection position with terrain mesh by ray traced from user mouse click position
     * 
     * @param clickPosition mouse click position on the screen
     * @return intersection position (WITHOUT terrain scale factor affected)
     */
    void CTerrainEditor::getIntersectionParametersWithTerrain(const core::position2di clickPosition) {
        const core::line3df rayFromScreenCoordinates = pSceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(clickPosition, pCameraSceneNode);
        core::vector3df collisionPosition;
        core::triangle3df collisionTriangle;
        scene::ISceneNode* collisionNode = 0;
        if (collisionParameters.collisionDetected = pSceneManager->getSceneCollisionManager()->getCollisionPoint(rayFromScreenCoordinates,
                pTerrainSceneNode->getTriangleSelector(),
                collisionPosition,
                collisionTriangle,
                collisionNode)) {
            collisionParameters.collisionPosition = collisionPosition;
            collisionParameters.collisionTriangle = collisionTriangle;


            /*intersectionPosition->X = core::ceil32(intersectionPosition->X);
            intersectionPosition->Y = core::ceil32(intersectionPosition->Y);
            intersectionPosition->Z = core::ceil32(intersectionPosition->Z);*/
        }
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
                pBrushManager->setBrushRadius(pBrushManager->getBrushRadius() + 2);
            } else if (event.KeyInput.Key == KEY_F5) {
                pBrushManager->setBrushRadius(pBrushManager->getBrushRadius() - 2);
            }
        }

        if (paintingEditMode || liftingEditMode) {
            if (event.EventType == EET_MOUSE_INPUT_EVENT) {
                if ((event.MouseInput.Event == EMIE_MOUSE_MOVED || event.MouseInput.Event == EMIE_MOUSE_WHEEL)
                        /*&& pDevice->getGUIEnvironment()->hasFocus(pDevice->getGUIEnvironment()->getRootGUIElement())*/) {
                    const core::position2di mousePosition = pDevice->getCursorControl()->getPosition();
                    getIntersectionParametersWithTerrain(mousePosition);
                    printf("Collision point X: %f\tY: %fZ: %f\n", collisionParameters.collisionPosition.X,
                            collisionParameters.collisionPosition.Y,
                            collisionParameters.collisionPosition.Z);
                } else if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) {
                    isLMBPressed = true;
                } else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP) {
                    isLMBPressed = false;
                    // TODO maybe optimize recreating not all triangle selector, but only triangles inside BBox size of brush radius
                    if (liftingEditMode) {
                        //pTerrainSceneNode->getTriangleSelector()->drop();
                        scene::ITriangleSelector* pTerrainTriangleSelector = 
                                pSceneManager->createTerrainTriangleSelector(pTerrainSceneNode, 0);
                        pTerrainSceneNode->setTriangleSelector(pTerrainTriangleSelector);
                        pTerrainTriangleSelector->drop();
                    }
                } else if (event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN) {
                    isRMBPressed = true;
                } else if (event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP) {
                    isRMBPressed = false;
                    // TODO maybe optimize recreating not all triangle selector, but only triangles inside BBox size of brush radius
                    if (liftingEditMode) {
                        //pTerrainSceneNode->getTriangleSelector()->drop();
                        scene::ITriangleSelector* pTerrainTriangleSelector = 
                                pSceneManager->createTerrainTriangleSelector(pTerrainSceneNode, 0);
                        pTerrainSceneNode->setTriangleSelector(pTerrainTriangleSelector);
                        pTerrainTriangleSelector->drop();
                    }
                }
            }
        }

        if (paintingEditMode && collisionParameters.collisionDetected) {
            pBrushManager->createDecalNode(collisionParameters.collisionPosition, collisionParameters.collisionTriangle.getNormal());
            if (isLMBPressed || isRMBPressed) {
                if (collisionParameters.collisionDetected) {
                    // paint terrain with current brush if any mouse button pressed and editMode active   
                    //textureTerrainWithCurrentBrush(*intersectionPosition);
                    textureTerrainWithCurrentBrush(core::vector3df(collisionParameters.collisionPosition.X / pTerrainSceneNode->getScale().X,
                            collisionParameters.collisionPosition.Y,
                            collisionParameters.collisionPosition.Z / pTerrainSceneNode->getScale().Z));
                }
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

    DecalManager* CTerrainEditor::getDecalManager() const {
        return pDecalManager;
    }

    /**
     * Getter
     * 
     * @return current terrain scale factor
     */
    core::vector3df CTerrainEditor::getTerrainScaleFactor() const {
        return pTerrainSceneNode->getScale();
    }

    /**
     * Setter
     * 
     * @param terrainScaleFactor new terrain scale factor
     */
    void CTerrainEditor::setTerrainScaleFactor(core::vector3df terrainScaleFactor) {
        pTerrainSceneNode->setScale(terrainScaleFactor);
    }

    /**
     * Getter
     * 
     * @return current painting edit mode value
     */
    bool CTerrainEditor::getPaintingEditMode() const {
        return paintingEditMode;
    }

    /**
     * Setter
     * 
     * @param editMode new value of edit mode flag
     */
    void CTerrainEditor::setPaintingEditMode(bool paintingEditMode) {
        this->paintingEditMode = paintingEditMode;

        // Turn on/off mouse control in GOTAnimator in pCameraSceneNode
        core::list<scene::ISceneNodeAnimator*>::ConstIterator cameraAnimatorList = pCameraSceneNode->getAnimators().begin();
        scene::CSceneNodeAnimatorCameraTerrain *GOTCameraAnimator = (scene::CSceneNodeAnimatorCameraTerrain*) * cameraAnimatorList;
        GOTCameraAnimator->setMouseActive(!GOTCameraAnimator->getMouseActive());

        if (paintingEditMode) {
            core::line3d<f32> line;
            line.start = pSceneManager->getActiveCamera()->getAbsolutePosition();
            line.end = pSceneManager->getActiveCamera()->getTarget();
            pBrushManager->createDecalNode(core::vector3df(), core::vector3df(0, 1, 0));

            /*pDecalSceneNode->getMaterial(0).Thickness = 2.0f;
            pDecalSceneNode->getMaterial(0).EmissiveColor = video::SColor(0, 255, 0, 0);
            pVideoDriver->setMaterial(pDecalSceneNode->getMaterial(0));*/
        } /*else {
            delete pDecalSceneNode;
        }*/
        /*if (!this->editMode) {
            intersectionPosition = 0;
        }*/
    }

    /**
     * Getter
     * 
     * @return current lifting edit mode value
     */
    bool CTerrainEditor::getLiftingEditMode() const {
        return liftingEditMode;
    }

    /**
     * Setter
     * 
     * @param liftingEditMode new value of lifting edit mode flag
     */
    void CTerrainEditor::setLiftingEditMode(bool liftingEditMode) {
        this->liftingEditMode = liftingEditMode;
    }


} // end of namespace irr

