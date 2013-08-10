/* 
 * File:   CBrushManager.cpp
 * Author: hokan
 * 
 * Created on 26 Июль 2013 г., 13:04
 */

#include "../../headers/terrain.editor/CBrushManager.h"

// TODO use pBrush pixels as transparency layer
namespace irr {

    /**
     * Constructor
     * 
     * @param pVideoDriver  pointer to IVideoDriver interface
     * @param pTerrainSceneNode     pointer to ITerrainSceneNode interface; over which brushes draws
     */
    CBrushManager::CBrushManager(video::IVideoDriver* pVideoDriver, scene::ITerrainSceneNode* pTerrainSceneNode, DecalManager* pDecalManager) :
    brushYAngle(0.0f), borderColour(video::SColor(0, 255, 0, 0)), isLCtrlButtonPressed(false), isLAltButtonPressed(false), brushType(0),
    pDecalSceneNode(0), raiseStep(5.0f), pDecalTexture(0), pBrush(0) {
        this->pVideoDriver = pVideoDriver;
        this->pTerrainSceneNode = pTerrainSceneNode;
        this->pDecalManager = pDecalManager;
        constructBrush("Data/textures/Gras.jpg", "texture/curcle_brush.png");
    }

    /**
     * Virtual destuctor
     * drops only loaded image of brush
     */
    CBrushManager::~CBrushManager() {
        pDecalTexture->drop();
        pBrush->drop();
    }

    bool CBrushManager::constructBrush(const io::path& decalImageFilePath, const io::path& brushFilePath) {
        video::IImage* pDecalImage = pVideoDriver->createImageFromFile(decalImageFilePath);
        video::IImage* pBrushImage = pVideoDriver->createImageFromFile(brushFilePath);
        video::IImage* pResultImage;
        // do not recreate image with alpha channel if we already have one
        if (pDecalTexture == 0 || pDecalTexture->getSize() != pDecalImage->getDimension()) {
            if (pDecalImage->getColorFormat() != video::ECF_A8R8G8B8) {
                pResultImage = pVideoDriver->createImage(video::ECF_A8R8G8B8, pDecalImage->getDimension());
            } else {
                pResultImage = pDecalImage;
            }
            
            if (pDecalTexture != 0) {
                pDecalTexture->drop();
            }
            pDecalTexture = pVideoDriver->addTexture("decalTexture", pResultImage);
            pDecalTexture->grab();
            pResultImage->drop();
        }

        // TODO check if this code is faster than lower one
        /*video::SColor* brushImagePixels = (video::SColor*)pBrushImage->lock();
        video::SColor* resultImagePixels = (video::SColor*)pResultImage->lock();
        for (u32 i = 0; i < pBrushImage->getDimension().Height; ++i) {
            for (u32 j = 0; j < pBrushImage->getDimension().Width; ++j) {
                if (brushImagePixels[i * pBrushImage->getDimension().Width + j].getRed() == 0) {
                        resultImagePixels[i * pBrushImage->getDimension().Width + j].set(0, 255, 255, 255);
                }
            }
        }
        pBrushImage->unlock();
        pResultImage->unlock();*/

        u32 heightScaleFactor = 1;
        u32 widthScaleFactor = 1;
        if (pDecalTexture->getSize().Height > pBrushImage->getDimension().Height) {
            heightScaleFactor = pDecalTexture->getSize().Height / pBrushImage->getDimension().Height;
        }

        if (pDecalTexture->getSize().Width > pBrushImage->getDimension().Width) {
            widthScaleFactor = pDecalTexture->getSize().Width / pBrushImage->getDimension().Width;
        }

        video::SColor* pDecalTexturePixels = (video::SColor*)pDecalTexture->lock(video::ETLM_READ_WRITE, 0);
        for (u32 i = 0; i < pDecalTexture->getSize().Height; ++i) {
            for (u32 j = 0; j < pDecalTexture->getSize().Width; ++j) {
                if (pBrushImage->getPixel(i / heightScaleFactor, j / widthScaleFactor).getRed() == 0) {
                    pDecalTexturePixels[j + i * pDecalTexture->getSize().Width].set(0, 255, 255, 255);
                } else {
                    pDecalTexturePixels[j + i * pDecalTexture->getSize().Width].set(pDecalImage->getPixel(i, j).color);
                }
            }
        }
        pDecalTexture->unlock();
        pDecalImage->drop();        

        brushRadius = pBrushImage->getDimension().Height / 2;
        brushMaximumRadius = brushRadius * 2;
        brushMinimumRadius = brushRadius / 2;

        if (pBrush != 0) {
            pBrush->drop();
        }
        pBrush = pVideoDriver->addTexture("brush", pBrushImage);
        pBrush->grab();
        pBrushImage->drop();

        return true;
    }

    void CBrushManager::createDecalNode(core::vector3df nodePosition, core::vector3df nodeNormal) {
        // TODO change to refresh-method, set rotation & far value
        if (pDecalSceneNode == 0) {
            pDecalSceneNode = pDecalManager->addDecal(pDecalTexture,
                    core::vector3df(brushRadius * 8,
                    0,
                    brushRadius * 8),
                    core::vector3df(brushRadius * 8,
                    brushRadius * 4,
                    brushRadius * 8),
                    nodeNormal,
                    brushYAngle,
                    0,
                    0,
                    pTerrainSceneNode->getBoundingBox().MaxEdge.X); // pCameraSceneNode->getFarValue()
        } else {
            pDecalSceneNode->setPosition(nodePosition);
            /*pDecalSceneNode->remove();
            pDecalSceneNode = pDecalManager->addDecal(pBrush,
                    nodePosition,
                    core::vector3df(brushRadius * 8,
                    0,
                    brushRadius * 8),
                    nodeNormal, //collisionTriangle.getNormal(), //line.start - line.end,
                    brushYAngle,
                    0,
                    0,
                    pTerrainSceneNode->getBoundingBox().MaxEdge.X); // pCameraSceneNode->getFarValue()*/
        }
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
            *x = pTerrainSceneNode->getBoundingBox().MaxEdge.X - 0.1;
        } else if (*x <= pTerrainSceneNode->getBoundingBox().MinEdge.X) {
            *x = pTerrainSceneNode->getBoundingBox().MinEdge.X + 0.1;
        }

        if (*z >= pTerrainSceneNode->getBoundingBox().MaxEdge.Z) {
            *z = pTerrainSceneNode->getBoundingBox().MaxEdge.Z - 0.1;
        } else if (*z <= pTerrainSceneNode->getBoundingBox().MinEdge.Z) {
            *z = pTerrainSceneNode->getBoundingBox().MinEdge.Z + 0.1;
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
        pVideoDriver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

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

    void CBrushManager::drawBrushBorder(core::vector3df brushCenter) {
        switch (brushType) {
            case 0:
                drawCircleBrushBorder(brushCenter);
                break;
            case 1:
                drawSquareBrushBorder(brushCenter);
                break;
            default:
                drawSquareBrushBorder(brushCenter);
                break;
        }
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
        pVideoDriver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

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

    video::ITexture* CBrushManager::paintTextureWithBrush(s32 vertexXCoordinate, s32 vertexZCoordinate, video::ITexture* paintingTexture) {
        // TODO this painting works "nice" (visually all seems good) only on square terrains
        // change pDecalTexture to pBrush

        u32 paintingTextureWidth = paintingTexture->getSize().Width;
        u32 paintingTextureHeight = paintingTexture->getSize().Height;
        // original start index is:
        // vertexZCoordinate * terrainWidth + (terrainWidth - vertexXCoordinate)
        // but if texture size different from terrain size we should use scale factor
        // first we calculate unscaled terrain-heightmap (terrain texture) size
        u32 terrainToTextureWidthScaleFactor =
                pTerrainSceneNode->getBoundingBox().MaxEdge.X / pTerrainSceneNode->getScale().X; //core::max_(pTerrainSceneNode->getScale().Z, pTerrainSceneNode->getScale().X);
        u32 terrainToTextureHeightScaleFactor =
                pTerrainSceneNode->getBoundingBox().MaxEdge.Z / pTerrainSceneNode->getScale().Z; //core::max_(pTerrainSceneNode->getScale().Z, pTerrainSceneNode->getScale().X);
        // and then divide painting texture size by terrain texture size
        terrainToTextureWidthScaleFactor = paintingTextureWidth / terrainToTextureWidthScaleFactor;
        terrainToTextureHeightScaleFactor = paintingTextureHeight / terrainToTextureHeightScaleFactor;
        // using scale factors calculate start index of pixel-array
        u32 startIndex = (vertexZCoordinate * paintingTextureWidth * terrainToTextureHeightScaleFactor
                + (paintingTextureWidth - vertexXCoordinate * terrainToTextureWidthScaleFactor));

        // get brush width, brush height, total size & change start index to left upper corner of brush
        u32 brushWidth = brushRadius * 2;
        u32 brushHeight = brushRadius * 2;
        u32 paintingTextureSize = paintingTextureWidth * paintingTextureHeight;

        startIndex -= ((brushWidth / 2)
                + (brushHeight / 2 * paintingTextureWidth));

        // this scale factors are used, because brush texture and it's actual size not necessary identical,
        // as we use similar (see ... function in ... class: pResultImage->getDimension().Height / pBrushImage->getDimension().Height)
        // scale factors to "inject" brush texture into brush, here we use mirror-like scale factors to "extract" brush pixels from 
        // brush texture
        u32 brushTextureToBrushSizeWidthScaleFactor = pDecalTexture->getSize().Width / brushWidth;
        u32 brushTextureToBrushSizeHeightScaleFactor = pDecalTexture->getSize().Height / brushHeight;

        // lock array of pixels & paint painting texture if and only if brush texture pixels not transparent
        video::SColor* pPaintingTexturePixels = (video::SColor*)paintingTexture->lock(video::ETLM_WRITE_ONLY);
        video::SColor* pBrushPixels = (video::SColor*)pDecalTexture->lock(video::ETLM_READ_ONLY);
        for (u32 x = 0; x < brushHeight; ++x) {
            for (u32 y = 0; y < brushWidth; ++y) {
                if (pBrushPixels[y * brushTextureToBrushSizeWidthScaleFactor
                        + x * pDecalTexture->getSize().Width * brushTextureToBrushSizeHeightScaleFactor].getAlpha() != 0) {
                    if ((startIndex + (y + x * paintingTextureWidth)) >= 0
                            && (startIndex + (y + x * paintingTextureWidth)) <= paintingTextureSize)
                        pPaintingTexturePixels[startIndex + (y + x * paintingTextureWidth)].set(255, 0, 255, 0);
                }
            }
        }

        paintingTexture->unlock();
        pDecalTexture->unlock();

        return paintingTexture;
    }

    /*==============================================================================
  Raise or lower terrain (selected vertice by brush)
==============================================================================*/
    void CBrushManager::raiseVerticesWithBrush(s32 vertexXCoordinate, s32 vertexZCoordinate, bool up) {
        // TODO make dynamic triangle selector

        scene::IMesh* pMesh = pTerrainSceneNode->getMesh();
        vertexXCoordinate++;
        //vertexZCoordinate++;

        s32 heightmapWidth; // = pTerrainSceneNode->getBoundingBox().MaxEdge.X / pTerrainSceneNode->getScale().X;
        s32 heightmapSize;
        //s32 heightmapHeight = heightmap->getDimension().Height;

        for (s32 b = 0; b < pMesh->getMeshBufferCount(); ++b) {
            scene::IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(b);
            // skip mesh buffers that are not the right type
            if (pMeshBuffer->getVertexType() != video::EVT_2TCOORDS)
                continue;

            heightmapWidth = static_cast<s32> (core::squareroot(static_cast<f32> (pMeshBuffer->getVertexCount())));
            heightmapSize = heightmapWidth * heightmapWidth;

            video::S3DVertex2TCoords* pVertices = (video::S3DVertex2TCoords*)pMeshBuffer->getVertices();

            s32 brushWidth = pBrush->getSize().Width; //brushRadius * 2;
            s32 brushHeight = pBrush->getSize().Height; //brushRadius * 2;
            u32 startIndex = vertexXCoordinate * heightmapWidth + vertexZCoordinate;
            startIndex -= ((brushWidth / 2) + (brushHeight / 2 * heightmapWidth));

            video::SColor* pBrushPixels = (video::SColor*)pBrush->lock(video::ETLM_READ_ONLY);
            for (s32 y = 0; y < brushHeight; ++y) {
                for (s32 x = 0; x < brushWidth; ++x) {
                    if ((startIndex + (x + y * heightmapWidth)) >= 0
                            && (startIndex + (x + y * heightmapWidth)) <= heightmapSize) {
                        f32 hy = pVertices[startIndex + (x + y * heightmapWidth)].Pos.Y;
                        f32 bp = pBrushPixels[x + y * brushWidth].getRed() / 255.0 * raiseStep;
                        bp = (up) ? bp : -bp;
                        hy += bp;

                        if (hy <= 255 && hy >= -255) {
                            pVertices[startIndex + (x + y * heightmapWidth)].Pos.Y = hy;
                        }
                    }
                    ; //pVertices[startIndex + x + y * heightmapWidth].Pos.Y += 25;
                }
            }
            //pVertices[startIndex].Pos.Y += 25;
            /*for (u32 i = 0; i < heightmapWidth + 2; ++i) {
                pVertices[i].Pos.Y += 25;
            }*/
            pBrush->unlock();
        }

        // force terrain render buffer to reload
        pTerrainSceneNode->setPosition(pTerrainSceneNode->getPosition());
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
                brushRadius += event.MouseInput.Wheel;
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
        if (brushRadius >= brushMaximumRadius) {
            this->brushMaximumRadius = brushRadius * 2;
        }

        if (brushRadius <= brushMinimumRadius) {
            this->brushMinimumRadius = brushRadius / 2;
        }
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
     * @return pointer to ITexture interface representing current brush
     */
    video::ITexture* CBrushManager::getCurrentBrush() const {
        return pBrush;
    }

    u8 CBrushManager::getBrushType() const {
        return brushType;
    }

    void CBrushManager::setBrushType(u8 brushType) {
        bool recreationBrushSuccess = false;
        if (this->brushType != brushType) {
            switch (brushType) {
                case 0:
                    recreationBrushSuccess = constructBrush("Data/textures/Gras.jpg", "texture/curcle_brush.png");
                    break;
                case 1:
                    recreationBrushSuccess = constructBrush("Data/textures/Gras.jpg", "texture/square_brush.png");
                    break;
                default:
                    recreationBrushSuccess = constructBrush("Data/textures/Gras.jpg", "texture/square_brush.png");
                    break;
            }
        }

        if (recreationBrushSuccess) {
            this->brushType = brushType;
        }
    }

} // end of namespace irr