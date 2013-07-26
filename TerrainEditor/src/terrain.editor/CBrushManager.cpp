/* 
 * File:   CBrushManager.cpp
 * Author: hokan
 * 
 * Created on 26 Июль 2013 г., 13:04
 */

#include "../../headers/terrain.editor/CBrushManager.h"

namespace irr {

    CBrushManager::CBrushManager(video::IVideoDriver* pVideoDriver, scene::ITerrainSceneNode* pTerrainSceneNode) : 
    brushYAngle(0.0f), brushSize(100.0f) {
        this->pVideoDriver = pVideoDriver;
        this->pTerrainSceneNode = pTerrainSceneNode;
        pBrush = this->pVideoDriver->createImageFromFile("brush.png");
    }

    CBrushManager::CBrushManager(const CBrushManager& orig) {
    }

    CBrushManager::~CBrushManager() {
        pBrush->drop();
    }
    
    bool CBrushManager::OnEvent(const SEvent& event) {
        if ((event.EventType == EET_KEY_INPUT_EVENT) && event.KeyInput.PressedDown) {
            if (event.KeyInput.Key == KEY_F4) {
                brushYAngle++;
                brushSize++;
            } else if (event.KeyInput.Key == KEY_F5) {
                brushYAngle--;
                brushSize--;
            }
        }
        return false;
    }
    
    void CBrushManager::drawAll() {
        drawSquareBrushBorder(pTerrainSceneNode->getTerrainCenter(), brushSize, brushYAngle, video::SColor(0, 255, 0, 0));
    }

    // TODO write rotation
    // optimum step = terrain grid cell width; how get this?

    /**
     * paints square brush border that lay on terrain
     * 
     * @param pVideoDriver          pointer to IVideoDriver instance
     * @param pTerrainSceneNode     pointer to ITerrainSceneNode instance
     * @param brushCenter           brush center point
     * @param brushRadius           brush radius
     * @param yAxisRotationAngle    angle rotation around y-axis, for rotation
     * @param borderColour          brush border colour
     */
    void CBrushManager::drawSquareBrushBorder(core::vector3df brushCenter,
            f32 brushRadius,
            f32 yAxisRotationAngle,
            video::SColor borderColour) {
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
     * paints circle brush border that lay on terrain
     * 
     * @param pVideoDriver          pointer to IVideoDriver instance
     * @param pTerrainSceneNode     pointer to ITerrainSceneNode instance
     * @param brushCenter           brush center point
     * @param brushRadius           brush radius
     * @param borderColour           brush border colour
     */
    void CBrushManager::drawCircleBrushBorder(core::vector3df brushCenter,
            f32 brushRadius,
            video::SColor borderColour) {
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
    
    f32 CBrushManager::getBrushSize() const {
        return brushSize;
    }
    
    f32 CBrushManager::getBrushYAngle() const {
        return brushYAngle;
    }
    
    void CBrushManager::setBrushSize(f32 brushSize) {
        this->brushSize = brushSize;
    }
    
    void CBrushManager::setBrushYAngle(f32 brushYAngle) {
        this->brushYAngle = brushYAngle;
    }
    
    video::IImage* CBrushManager::getCurrentBrush() const {
        return pBrush;
    }

} // end of namespace irr