/* 
 * File:   CBrushManager.cpp
 * Author: hokan
 * 
 * Created on 26 Июль 2013 г., 13:04
 */

#include "../../headers/terrain.editor/CBrushManager.h"

namespace irr {

    /**
     * Constructor
     * 
     * @param pVideoDriver  pointer to IVideoDriver interface
     * @param pTerrainSceneNode     pointer to ITerrainSceneNode interface; over which brushes draws
     */
    CBrushManager::CBrushManager(video::IVideoDriver* pVideoDriver, scene::ITerrainSceneNode* pTerrainSceneNode) :
    brushYAngle(0.0f), borderColour(video::SColor(0, 255, 0, 0)), isLCtrlButtonPressed(false), isLAltButtonPressed(false) {
        this->pVideoDriver = pVideoDriver;
        this->pTerrainSceneNode = pTerrainSceneNode;
        pBrush = this->pVideoDriver->createImageFromFile("brush.png");
        // divide by two, cause it's radius, not brush size
        brushRadius = pBrush->getDimension().Height * pTerrainSceneNode->getScale().X / 2;
        brushMaximumRadius = brushRadius * 2;
        brushMinimumRadius = brushRadius / 2;
        
        video::IImage* img = pVideoDriver->createImage(video::ECF_R8G8B8, core::dimension2du(8, 8));
        pBrush->copyToScaling(img);
    }

    /**
     * Virtual destuctor
     * drops only loaded image of brush
     */
    CBrushManager::~CBrushManager() {
        pBrush->drop();
    }

    /**
     * Draws everything - brush, its border, any other suff
     */
    void CBrushManager::drawAll() {
        //drawCircleBrushBorder(pTerrainSceneNode->getTerrainCenter());
        //drawSquareBrushBorder(pTerrainSceneNode->getTerrainCenter());
    }

    void CBrushManager::outOfTerrainPerimeterCheck(f32* x, f32* z) {
        // this conditions needs for out-of-terrain perimeter verticies
        if (*x >= pTerrainSceneNode->getBoundingBox().MaxEdge.X) {
            *x = pTerrainSceneNode->getBoundingBox().MaxEdge.X - 1;
        } else if (*x <= pTerrainSceneNode->getBoundingBox().MinEdge.X) {
            *x = pTerrainSceneNode->getBoundingBox().MinEdge.X + 1;
        }

        if (*z >= pTerrainSceneNode->getBoundingBox().MaxEdge.Z) {
            *z = pTerrainSceneNode->getBoundingBox().MaxEdge.Z - 1;
        } else if (*z <= pTerrainSceneNode->getBoundingBox().MinEdge.Z) {
            *z = pTerrainSceneNode->getBoundingBox().MinEdge.Z + 1;
        }
    }

    // TODO write rotation
    // optimum step = terrain grid cell width; how get this?

    /**
     * Draws square border over terrain node
     * 
     * @param brushCenter   coordinates of brush center
     */
    void CBrushManager::drawSquareBrushBorder(core::vector3df brushCenter) {
        // set steps:
        f32 hoverStep = 0.1f; // because border must hover alittle over terrain, not stuck in it
        f32 step = 17.0f; // vertices space step; optimum = terain grid cell width
        f32 paintingStepX = step; // * sin(yAxisRotationAngle);      // painting on x-axis
        f32 paintingStepZ = step; // * cos(yAxisRotationAngle);      // painting on z-axis, they different because of rotation

        // create line material - line thickness & colour, and set it as material for future paining
        video::SMaterial lineMaterial;
        lineMaterial.Thickness = 2.0f;
        lineMaterial.EmissiveColor = borderColour;
        pVideoDriver->setMaterial(lineMaterial);

        // set begin indexes for different sides, optimize calculations
        // fistSideIndexBegin = 0
        u32 secondSideIndexBegin = (u32) (2 * brushRadius) / step; // (2 * brushRadius) / step
        u32 thirdSideIndexBegin = 2 * secondSideIndexBegin; // 2 * (2 * brushRadius) / step
        u32 forthSideIndexBegin = 3 * secondSideIndexBegin; // 3 * (2 * brushRadius) / step

        // create static arrays of vertices and its indexes; vertex instance & per-side index counter
        // additional vertex to the end of array to form vertex loop; see more below
        core::array<video::S3DVertex> verticesList(4 * secondSideIndexBegin + 1);
        core::array<u16> indicesList(4 * secondSideIndexBegin + 1);

        video::S3DVertex vertex;

        u16 indexNumber = 0;

        // calculate start positions, put them in vertices array and its indexes in index array
        /* For every position we calculate new coordinates, remember them as new one, check out-of-terrain perimeter
         * conditions & set result as vertex of border. Exactly in such order, cause if you will set new value
         * after validation you'll get much longer border lines than you need; you can check it, just
         * move all XXXX = core::vector(x, 0, z) calls after outOfTerrainPerimeterCheck calls & move mouse next to 
         * terrain corner with maximum scale
         * We don't use such technique in drawCircleBrushBorder, cause all vertices're independent
         */
        // lowerRightCorner
        f32 x = brushCenter.X + brushRadius;
        f32 z = brushCenter.Z - brushRadius;
        core::vector3df lowerRightCorner = core::vector3df(x, 0, z);
        outOfTerrainPerimeterCheck(&x, &z);

        vertex.Pos = core::vector3df(x,
                pTerrainSceneNode->getHeight(x, z) + hoverStep,
                z); 
        verticesList[indexNumber] = vertex;
        indicesList[indexNumber] = indexNumber;

        // TODO find elegant solution
        // copy first vertex to the end of array to form vertex loop
        // EPT_LINE_LOOP flag good, but lines created by it are UNDER terrain texture
        verticesList[forthSideIndexBegin + secondSideIndexBegin] = vertex;
        indicesList[forthSideIndexBegin + secondSideIndexBegin] = forthSideIndexBegin + secondSideIndexBegin;

        // lowerLeftCorner
        x = brushCenter.X - brushRadius;
        z = brushCenter.Z - brushRadius;
        core::vector3df lowerLeftCorner = core::vector3df(x, 0, z);
        outOfTerrainPerimeterCheck(&x, &z);

        vertex.Pos = core::vector3df(x,
                pTerrainSceneNode->getHeight(x, z) + hoverStep,
                z); 
        verticesList[forthSideIndexBegin] = vertex;
        indicesList[forthSideIndexBegin] = forthSideIndexBegin;

        // upperRightCorner
        x = brushCenter.X + brushRadius;
        z = brushCenter.Z + brushRadius;
        core::vector3df upperRightCorner = core::vector3df(x, 0, z);
        outOfTerrainPerimeterCheck(&x, &z);

        vertex.Pos = core::vector3df(x,
                pTerrainSceneNode->getHeight(x, z) + hoverStep,
                z); 
        verticesList[secondSideIndexBegin] = vertex;
        indicesList[secondSideIndexBegin] = secondSideIndexBegin;

        // upperLeftCorner
        x = brushCenter.X - brushRadius;
        z = brushCenter.Z + brushRadius;
        core::vector3df upperLeftCorner = core::vector3df(x, 0, z);
        outOfTerrainPerimeterCheck(&x, &z);

        vertex.Pos = core::vector3df(x,
                pTerrainSceneNode->getHeight(x, z) + hoverStep,
                z); 
        verticesList[thirdSideIndexBegin] = vertex;
        indicesList[thirdSideIndexBegin] = thirdSideIndexBegin;

        // for each side recalculate position of next point, fill arrays and swap new and current positions
        for (indexNumber = 1; indexNumber < secondSideIndexBegin; ++indexNumber) {
            // lowerRightCorner
            x = lowerRightCorner.X;
            z = lowerRightCorner.Z + paintingStepZ;
            lowerRightCorner = core::vector3df(x, 0, z);
            outOfTerrainPerimeterCheck(&x, &z);

            vertex.Pos
                    = core::vector3df(x,
                    pTerrainSceneNode->getHeight(x, z) + hoverStep,
                    z);
            verticesList[indexNumber] = vertex;
            indicesList[indexNumber] = indexNumber;

            // lowerLeftCorner
            x = lowerLeftCorner.X + paintingStepX;
            z = lowerLeftCorner.Z;
            lowerLeftCorner = core::vector3df(x, 0, z);
            outOfTerrainPerimeterCheck(&x, &z);

            vertex.Pos
                    = core::vector3df(x,
                    pTerrainSceneNode->getHeight(x, z) + hoverStep,
                    z);
            verticesList[forthSideIndexBegin + indexNumber] = vertex;
            indicesList[forthSideIndexBegin + indexNumber] = forthSideIndexBegin + indexNumber;

            // upperRightCorner
            x = upperRightCorner.X - paintingStepX;
            z = upperRightCorner.Z;
            upperRightCorner = core::vector3df(x, 0, z);
            outOfTerrainPerimeterCheck(&x, &z);

            vertex.Pos
                    = core::vector3df(x,
                    pTerrainSceneNode->getHeight(x, z) + hoverStep,
                    z);
            verticesList[secondSideIndexBegin + indexNumber] = vertex;
            indicesList[secondSideIndexBegin + indexNumber] = secondSideIndexBegin + indexNumber;

            // upperLeftCorner
            x = upperLeftCorner.X;
            z = upperLeftCorner.Z - paintingStepZ;
            upperLeftCorner = core::vector3df(x, 0, z);
            outOfTerrainPerimeterCheck(&x, &z);

            vertex.Pos
                    = core::vector3df(x,
                    pTerrainSceneNode->getHeight(x, z) + hoverStep,
                    z);
            verticesList[thirdSideIndexBegin + indexNumber] = vertex;
            indicesList[thirdSideIndexBegin + indexNumber] = thirdSideIndexBegin + indexNumber;
        }

        // draw everything
        pVideoDriver->drawVertexPrimitiveList(&verticesList[0],
                4 * secondSideIndexBegin + 1,
                &indicesList[0],
                4 * secondSideIndexBegin,
                video::EVT_STANDARD,
                scene::EPT_LINE_STRIP,
                video::EIT_16BIT);

        // restore default material to video driver
        pVideoDriver->setMaterial(video::SMaterial());
    }

    // TODO rotation (not need in circle, but use for brush direction)

    /**
     * Draws circle border over terrain node
     * 
     * @param brushCenter   coordinates of brush center
     */
    void CBrushManager::drawCircleBrushBorder(core::vector3df brushCenter) {
        // set steps:
        f32 hoverStep = 0.1f; // because border must hover alittle over terrain, not stuck in it
        f32 step = 0.1f; // vertices space step

        // create line material - line thickness & colour, and set it as material for future paining
        video::SMaterial lineMaterial;
        lineMaterial.Thickness = 2.0f;
        lineMaterial.EmissiveColor = borderColour;
        pVideoDriver->setMaterial(lineMaterial);

        // create static arrays of vertices and its indexes; vertex instance & per-side index counter
        u32 verticesCount = core::ceil32(2 * core::PI) / step;
        core::array<video::S3DVertex> verticesList(verticesCount);
        core::array<u16> indicesList(verticesCount);

        video::S3DVertex vertex;

        //f32 brushRadius = this->brushRadius * pTerrainSceneNode->getScale().X;
        u16 indexNumber = 0;
        // calculate X and Z coordinates base position
        f32 x = brushCenter.X + brushRadius;
        f32 z = brushCenter.Z;
        outOfTerrainPerimeterCheck(&x, &z);

        // calculate start position
        vertex.Pos = core::vector3df(x,
                pTerrainSceneNode->getHeight(x, z) + hoverStep,
                z);
        verticesList[indexNumber] = vertex;
        indicesList[indexNumber] = indexNumber;

        // for each radian-part recalculate position of next point and fill arrays
        for (indexNumber = 1; indexNumber < verticesCount; ++indexNumber) {
            // calculate X and Z coordinates new vertex
            x = brushCenter.X + brushRadius * cos(indexNumber * step);
            z = brushCenter.Z + brushRadius * sin(indexNumber * step);
            outOfTerrainPerimeterCheck(&x, &z);

            // fill arrays
            vertex.Pos = core::vector3df(x,
                    pTerrainSceneNode->getHeight(x, z) + hoverStep,
                    z);
            verticesList[indexNumber] = vertex;
            indicesList[indexNumber] = indexNumber;
        }

        // draw everything
        pVideoDriver->drawVertexPrimitiveList(&verticesList[0],
                verticesCount,
                &indicesList[0],
                verticesCount - 1,
                video::EVT_STANDARD,
                scene::EPT_LINE_STRIP,
                video::EIT_16BIT);

        // restore default material to video driver
        pVideoDriver->setMaterial(video::SMaterial());
    }

    /**
     * Operates events affecting on brush - changing parameters, drawing etc.
     * increase/decrease brush angle around Y-axis and brush radius when user presses F4 & F5 respectively
     * 
     * @param event incomming event
     * @return true if incomming event processed
     */
    bool CBrushManager::OnEvent(const SEvent& event) {
        if (event.EventType == EET_KEY_INPUT_EVENT) {
            if (event.KeyInput.Key == KEY_LCONTROL) {
                isLCtrlButtonPressed = event.KeyInput.PressedDown;
            } else if (event.KeyInput.Key == KEY_LMENU) {
                isLAltButtonPressed = event.KeyInput.PressedDown;
            }
        }

        if (event.EventType == EET_MOUSE_INPUT_EVENT
                && event.MouseInput.Event == EMIE_MOUSE_WHEEL
                && event.MouseInput.Wheel != 0) {
            if (isLCtrlButtonPressed) {
                brushRadius += (event.MouseInput.Wheel > 0) * 2 * pTerrainSceneNode->getScale().X;
                if (brushRadius > brushMaximumRadius) {
                    brushRadius = brushMaximumRadius;
                } else if (brushRadius < brushMinimumRadius) {
                    brushRadius = brushMinimumRadius;
                }
                return true;
            } else if (isLAltButtonPressed) {
                brushYAngle += event.MouseInput.Wheel;
                if (brushYAngle > 2.0f) {
                    brushYAngle -= 2.0f;
                } else if (brushYAngle < -2.0f) {
                    brushYAngle += 2.0f;
                }
                return true;
            }
        }
        return false;
    }

    /**
     * Getter
     * 
     * @return current brush angle around Y-axis
     */
    f32 CBrushManager::getBrushYAngle() const {
        return brushYAngle;
    }

    /**
     * Setter
     * 
     * @param brushYAngle new brush angle aroung Y-axis
     */
    void CBrushManager::setBrushYAngle(f32 brushYAngle) {
        this->brushYAngle = brushYAngle;
    }

    /**
     * Getter
     * 
     * @return current brush radius
     */
    f32 CBrushManager::getBrushRadius() const {
        return brushRadius;
    }

    /**
     * Setter
     * 
     * @param brushRadius new brush radius
     */
    void CBrushManager::setBrushRadius(f32 brushRadius) {
        this->brushRadius = brushRadius;
    }

    /**
     * Getter
     * 
     * @return current brush maximum radius
     */
    f32 CBrushManager::getBrushMaximumRadius() const {
        return brushMaximumRadius;
    }

    /**
     * Setter
     * 
     * @param brushMaximumRadius new brush maximum radius
     */
    void CBrushManager::setBrushMaximumRadius(f32 brushMaximumRadius) {
        this->brushMaximumRadius = brushMaximumRadius;
    }

    /**
     * Getter
     * 
     * @return current brush minimum radius
     */
    f32 CBrushManager::getBrushMinimumRadius() const {
        return brushMinimumRadius;
    }

    /**
     * Setter
     * 
     * @param brushMinimumRadius new brush minimum radius
     */
    void CBrushManager::setBrushMinimumRadius(f32 brushMinimumRadius) {
        this->brushMinimumRadius = brushMinimumRadius;
    }

    /**
     * Getter
     * 
     * @return current brush border colour
     */
    video::SColor CBrushManager::getBrushBorderColour() const {
        return borderColour;
    }

    /**
     * Setter
     * 
     * @param borderColour new brush border colour
     */
    void CBrushManager::setBrushBorderColour(video::SColor borderColour) {
        this->borderColour = borderColour;
    }

    /**
     * Getter
     * 
     * @return pointer to IImage interface representing current brush
     */
    video::IImage* CBrushManager::getCurrentBrush() const {
        return pBrush;
    }

} // end of namespace irr