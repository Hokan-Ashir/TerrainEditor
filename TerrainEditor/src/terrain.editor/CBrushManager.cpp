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
    brushYAngle(0.0f), brushRadius(100.0f), borderColour(video::SColor(0, 255, 0, 0)) {
        this->pVideoDriver = pVideoDriver;
        this->pTerrainSceneNode = pTerrainSceneNode;
        pBrush = this->pVideoDriver->createImageFromFile("brush.png");
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
        drawSquareBrushBorder(pTerrainSceneNode->getTerrainCenter());
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
        core::vector3df lowerRightCorner
                = core::vector3df(brushCenter.X + brushRadius,
                pTerrainSceneNode->getHeight(brushCenter.X + brushRadius, brushCenter.Z - brushRadius) + hoverStep,
                brushCenter.Z - brushRadius);
        vertex.Pos = lowerRightCorner;
        verticesList[indexNumber] = vertex;
        indicesList[indexNumber] = indexNumber;

        // TODO find elegant solution
        // copy first vertex to the end of array to form vertex loop
        // EPT_LINE_LOOP flag good, but lines created by it are UNDER terrain
        verticesList[forthSideIndexBegin + secondSideIndexBegin] = vertex;
        indicesList[forthSideIndexBegin + secondSideIndexBegin] = forthSideIndexBegin + secondSideIndexBegin;

        core::vector3df lowerLeftCorner
                = core::vector3df(brushCenter.X - brushRadius,
                pTerrainSceneNode->getHeight(brushCenter.X - brushRadius, brushCenter.Z - brushRadius) + hoverStep,
                brushCenter.Z - brushRadius);
        vertex.Pos = lowerLeftCorner;
        verticesList[forthSideIndexBegin] = vertex;
        indicesList[forthSideIndexBegin] = forthSideIndexBegin;

        core::vector3df upperRightCorner
                = core::vector3df(brushCenter.X + brushRadius,
                pTerrainSceneNode->getHeight(brushCenter.X + brushRadius, brushCenter.Z + brushRadius) + hoverStep,
                brushCenter.Z + brushRadius);
        vertex.Pos = upperRightCorner;
        verticesList[secondSideIndexBegin] = vertex;
        indicesList[secondSideIndexBegin] = secondSideIndexBegin;

        core::vector3df upperLeftCorner
                = core::vector3df(brushCenter.X - brushRadius,
                pTerrainSceneNode->getHeight(brushCenter.X - brushRadius, brushCenter.Z + brushRadius) + hoverStep,
                brushCenter.Z + brushRadius);
        vertex.Pos = upperLeftCorner;
        verticesList[thirdSideIndexBegin] = vertex;
        indicesList[thirdSideIndexBegin] = thirdSideIndexBegin;

        // for each side recalculate position of next point, fill arrays and swap new and current positions
        for (indexNumber = 1; indexNumber < secondSideIndexBegin; ++indexNumber) {
            vertex.Pos
                    = core::vector3df(lowerRightCorner.X,
                    pTerrainSceneNode->getHeight(lowerRightCorner.X, lowerRightCorner.Z + paintingStepZ) + hoverStep,
                    lowerRightCorner.Z + paintingStepZ);
            lowerRightCorner = vertex.Pos;
            verticesList[indexNumber] = vertex;
            indicesList[indexNumber] = indexNumber;

            vertex.Pos
                    = core::vector3df(lowerLeftCorner.X + paintingStepX,
                    pTerrainSceneNode->getHeight(lowerLeftCorner.X + paintingStepX, lowerLeftCorner.Z) + hoverStep,
                    lowerLeftCorner.Z);
            lowerLeftCorner = vertex.Pos;
            verticesList[forthSideIndexBegin + indexNumber] = vertex;
            indicesList[forthSideIndexBegin + indexNumber] = forthSideIndexBegin + indexNumber;

            vertex.Pos
                    = core::vector3df(upperRightCorner.X - paintingStepX,
                    pTerrainSceneNode->getHeight(upperRightCorner.X - paintingStepX, upperRightCorner.Z) + hoverStep,
                    upperRightCorner.Z);
            upperRightCorner = vertex.Pos;
            verticesList[secondSideIndexBegin + indexNumber] = vertex;
            indicesList[secondSideIndexBegin + indexNumber] = secondSideIndexBegin + indexNumber;

            vertex.Pos
                    = core::vector3df(upperLeftCorner.X,
                    pTerrainSceneNode->getHeight(upperLeftCorner.X, upperLeftCorner.Z - paintingStepZ) + hoverStep,
                    upperLeftCorner.Z - paintingStepZ);
            upperLeftCorner = vertex.Pos;
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

        u16 indexNumber = 0;

        // calculate start position
        vertex.Pos = core::vector3df(brushCenter.X + brushRadius,
                pTerrainSceneNode->getHeight(brushRadius + brushCenter.X, brushCenter.Z) + hoverStep,
                brushCenter.Z);
        verticesList[indexNumber] = vertex;
        indicesList[indexNumber] = indexNumber;

        // for each radian-part recalculate position of next point and fill arrays
        for (indexNumber = 1; indexNumber < verticesCount; ++indexNumber) {
            // calculate X and Z coordinates new vertex
            f32 x = brushCenter.X + brushRadius * cos(indexNumber * step);
            f32 z = brushCenter.Z + brushRadius * sin(indexNumber * step);

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
        if ((event.EventType == EET_KEY_INPUT_EVENT) && event.KeyInput.PressedDown) {
            if (event.KeyInput.Key == KEY_F4) {
                brushYAngle++;
                brushRadius++;
            } else if (event.KeyInput.Key == KEY_F5) {
                brushYAngle--;
                brushRadius--;
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
     * @return current brush border colour
     */
    video::SColor CBrushManager::getBrushBorderColour() const {
        return borderColour;
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