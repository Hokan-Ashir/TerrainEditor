#include "irrlicht.h"
#include "CMasterEventReceiver.h"
#include "CSceneNodeAnimatorCameraTerrain.h"
#include "CGUIMainMenu.h"
#include "CMultiTexturingManager.h"
using namespace irr;

#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().
//#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:console /ENTRY:mainCRTStartup")
#endif

/**
 * Terrain height map
 */
video::IImage* pHeightmap;

/**
 * Brush using over pTerrainSceneNode
 */
video::IImage* pBrush;

/**
 * Some texture
 */
//video::IImage* pTexture;

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
void drawSquareBrushBorder(video::IVideoDriver* pVideoDriver,
                           scene::ITerrainSceneNode* pTerrainSceneNode,
                           core::vector3df brushCenter,
                           f32 brushRadius,
                           f32 yAxisRotationAngle,
                           video::SColor borderColour)
{
    // set steps:
    f32 hoverStep = 0.1f;       // because border must hover alittle over terrain, not stuck in it
    f32 step = 17.0f;           // vertices space step; optimum = terain grid cell width
    f32 paintingStepX = step;// * sin(yAxisRotationAngle);      // painting on x-axis
    f32 paintingStepZ = step;// * cos(yAxisRotationAngle);      // painting on z-axis, they different because of rotation
    
    // create line material - line thickness & colour, and set it as material for future paining
    video::SMaterial lineMaterial;
    lineMaterial.Thickness = 2.0f;
    lineMaterial.EmissiveColor = borderColour;
    pVideoDriver->setMaterial(lineMaterial);
    
    // set begin indexes for different sides, optimize calculations
    // fistSideIndexBegin = 0
    u32 secondSideIndexBegin = (u32) (2 * brushRadius) / step;      // (2 * brushRadius) / step
    u32 thirdSideIndexBegin = 2 * secondSideIndexBegin;                 // 2 * (2 * brushRadius) / step
    u32 forthSideIndexBegin = 3 * secondSideIndexBegin;                 // 3 * (2 * brushRadius) / step
    
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
                              pTerrainSceneNode->getHeight( brushCenter.X - brushRadius, brushCenter.Z + brushRadius) + hoverStep,
                              brushCenter.Z + brushRadius);
    vertex.Pos = upperLeftCorner;
    verticesList[thirdSideIndexBegin] = vertex;
    indicesList[thirdSideIndexBegin] = thirdSideIndexBegin;
   
    // for each side recalculate position of next point, fill arrays and swap new and current positions
    for (indexNumber = 1; indexNumber < secondSideIndexBegin; ++indexNumber)
    {
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
void drawCircleBrushBorder(video::IVideoDriver* pVideoDriver,
                           scene::ITerrainSceneNode* pTerrainSceneNode,
                           core::vector3df brushCenter,
                           f32 brushRadius,
                           video::SColor borderColour)
{
    // set steps:
    f32 hoverStep = 0.1f;       // because border must hover alittle over terrain, not stuck in it
    f32 step = 0.1f;           // vertices space step
    
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
    for (indexNumber = 1; indexNumber < verticesCount; ++indexNumber)
    {
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
 * Raise or lower pTerrainSceneNode vertex
 *
 * @param pTerrainSceneNode	ITerrainSceneNode pointer which vertices to be raised
 * @param vertexIndex		vertex which should be raised
 * @param raiseStep		raising step
 * @param raise		if true - raise vertices, otherwise - lower them
 */
void changeTerrainVertex(scene::ITerrainSceneNode* pTerrainSceneNode, s32 vertexIndex, f32 raiseStep, bool raise)
{
    scene::IMesh* pMesh = pTerrainSceneNode->getMesh();

    for (s32 i = 0; i < pMesh->getMeshBufferCount(); ++i)
    {
        scene::IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(i);
        // skip mesh buffers that are not the right type
        if (pMeshBuffer->getVertexType() != video::EVT_2TCOORDS)
            continue;

        video::S3DVertex2TCoords* pVertices = (video::S3DVertex2TCoords*)pMeshBuffer->getVertices();

        pVertices[vertexIndex].Pos.Y += (raise) ? raiseStep : -raiseStep;
    }

    // force pTerrainSceneNode render buffer to reload
    pTerrainSceneNode->setPosition(pTerrainSceneNode->getPosition());
}

/**
 * Raise or lower pTerrainSceneNode vertices by brush
 *
 * @param pTerrainSceneNode	ITerrainSceneNode pointer which vertices to be raised
 * @param centerVertexIndex		center vertex of pTerrainSceneNode which will be raised
 * @param raiseStep		raising step
 * @param raise		if true - raise vertices, otherwise - lower them
 */
void changeTerrainVerticesByBrush(scene::ITerrainSceneNode* pTerrainSceneNode, s32 centerVertexIndex, f32 raiseStep, bool raise)
{
    scene::IMesh* pMesh = pTerrainSceneNode->getMesh();

    s32 heightmapWidth = pHeightmap->getDimension().Width;
    //s32 heightmapHeight = pHeightmap->getDimension().Height;

    for (s32 i = 0; i < pMesh->getMeshBufferCount(); ++i)
    {
        scene::IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(i);
        // skip mesh buffers that are not the right type
        if (pMeshBuffer->getVertexType() != video::EVT_2TCOORDS)
            continue;

        video::S3DVertex2TCoords* pVertices = (video::S3DVertex2TCoords*)pMeshBuffer->getVertices();

        s32 brushWidth = pBrush->getDimension().Width;
        s32 brushHeight = pBrush->getDimension().Height;

        for (s32 y = 0; y < brushHeight; ++y)
        {
            for (s32 x = 0; x < brushWidth; ++x)
            {
                video::SColor brushPixel = pBrush->getPixel(x, y);

                if ((centerVertexIndex - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth)) >= 0)
                {
                    f32 vertexYCoordinate = pVertices[centerVertexIndex - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth)].Pos.Y;
                    raiseStep *= brushPixel.getRed() / 255.0;
                    raiseStep = (raise) ? raiseStep : -raiseStep;

                    if (/*bp > 0 &&*/ (vertexYCoordinate + raiseStep) <= 255 && (vertexYCoordinate + raiseStep) >= 0)
                        pVertices[centerVertexIndex - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth)].Pos.Y = vertexYCoordinate + raiseStep;
                }
            }
        }
    }

    // force pTerrainSceneNode render buffer to reload
    pTerrainSceneNode->setPosition(pTerrainSceneNode->getPosition());
}

/**
 * Some test code, need to be checked
 *
 * @param pTerrainSceneNode
 * @param index
 */
void PaintTerrainWithTexture(scene::ITerrainSceneNode* pTerrainSceneNode, s32 index)
{
    //scene::IMesh* pMesh = pTerrainSceneNode->getMesh();
    video::ITexture* pTexture = pTerrainSceneNode->getMaterial(0).getTexture(0);
    video::SColor* pPixels = (video::SColor*)pTexture->lock();
    //s32 h = pTexture->getSize().Height;
    //s32 w = pTexture->getSize().Width;

    s32 heightmapWidth = pHeightmap->getDimension().Width;
    s32 heightmapHeight = pHeightmap->getDimension().Height;


    //for (s32 b = 0; b < pMesh->getMeshBufferCount(); ++b)
    //{
    //scene::IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(b);
    // skip mesh buffers that are not the right type
    //if (pMeshBuffer->getVertexType() != video::EVT_2TCOORDS)
    //	continue;

    //video::S3DVertex2TCoords* pVertices = (video::S3DVertex2TCoords*)pMeshBuffer->getVertices();

    s32 brushWidth = pBrush->getDimension().Width;
    s32 brushHeight = pBrush->getDimension().Height;
    //pPixels[index * 32] = video::SColor(255, 0, 255, 0);
    //printf("Index: %d\t h * Index: %d\t w - Index: %d\n", index, h * index, h - index);
    u32 t = 0;
    printf("Red: %d\t Green: %d\t Blue: %d\n", pPixels[t].getRed(), pPixels[t].getGreen(), pPixels[t].getBlue());
    t = 1;
    printf("Red: %d\t Green: %d\t Blue: %d\n", pPixels[t].getRed(), pPixels[t].getGreen(), pPixels[t].getBlue());
    t = 255;
    printf("Red: %d\t Green: %d\t Blue: %d\n", pPixels[t].getRed(), pPixels[t].getGreen(), pPixels[t].getBlue());
    t = 256;
    printf("Red: %d\t Green: %d\t Blue: %d\n", pPixels[t].getRed(), pPixels[t].getGreen(), pPixels[t].getBlue());

    for (s32 y = 0; y < brushHeight; ++y)
    {
        for (s32 x = 0; x < brushWidth; ++x)
        {
            video::SColor brushPixel = pBrush->getPixel(x, y);

            // assuming that brushWidth == brushHeigth
            if ((index - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth)) >= 0)
            {
                //pVertices[index - (brushWidth/2) - (brushWidth/2 * heightmapWidth) + (x + y * heightmapWidth)].Color = brushPixel;
                pPixels[(index - (brushWidth / 2) - (brushWidth / 2 * heightmapWidth) + (x + y * heightmapWidth))] = brushPixel;
                //f32 hy = pVertices[index - (brushWidth/2) - (brushWidth/2 * heightmapWidth) + (x + y * heightmapWidth)].Pos.Y;
                //f32 bp = brushPixel.getRed()/255.0;// * step;
                //bp = (up) ? bp : -bp;

                // if(/*bp > 0 &&*/ (hy + bp) <= 255 &&  (hy + bp) >= 0)
                //    pVertices[index - (brushWidth/2) - (brushWidth/2 * heightmapWidth) + (x + y * heightmapWidth)].Pos.Y = hy + bp;
            }
        }
    }
    //}

    pPixels[1024] = video::SColor(0, 0, 0, 255);
    pTexture->unlock();
    // force pTerrainSceneNode render buffer to reload
    pTerrainSceneNode->setPosition(pTerrainSceneNode->getPosition());
}

/**
 * Converter from ITexture* to IImage*
 *
 * @param pVideoDriver	IVideoDriver pointer
 * @param pTexture	ITexture pointer to be converted
 * @return			pointer to IImage
 */
video::IImage* TextureToImage(video::IVideoDriver* pVideoDriver, video::ITexture* pTexture)
{
    video::IImage* pImage = pVideoDriver->createImageFromData(pTexture->getColorFormat(),
                                                              pTexture->getSize(),
                                                              pTexture->lock(),
                                                              false); //copy memory

    pTexture->unlock();
    return pImage;
}

/**
 * Converter from IImage* to ITexture*
 *
 * @param pVideoDriver	IVideoDriver pointer
 * @param pImage		IImage pointer to be converted
 * @param textureName		name of result ITexture*
 * @return			pointer to ITexture
 */
video::ITexture* ImageToTexture(video::IVideoDriver* pVideoDriver, video::IImage* pImage, core::stringc textureName)
{
    video::ITexture* pTexture = pVideoDriver->addTexture(textureName.c_str(), pImage);
    pTexture->grab();
    return pTexture;
}

/**
 * Paint pTerrainSceneNode node with brush
 *
 * @param pVideoDriver	IVideoDriver pointer
 * @param pTerrainSceneNode	ITerrainSceneNode pointer to be textured
 * @param x			center position of brush on x-axis
 * @param z			center position of brush on z-axis
 * @param texture	ITexture pointer which instance user to paint pTerrainSceneNode node
 * @return			pointer to result, painted texture
 */
video::ITexture* textureBrush(video::IVideoDriver* pVideoDriver, scene::ITerrainSceneNode* pSceneNodeTerrain, s32 vertexXCoordinate, s32 vertexZCoordinate, video::ITexture* texture)
{
    s32 index = (vertexZCoordinate * 512 + (512 - vertexXCoordinate));
    s32 brushWidth = pBrush->getDimension().Width;
    s32 brushHeight = pBrush->getDimension().Height;
    s32 heightmapWidth = 512;

    video::SColor* pPixels = (video::SColor*)texture->lock();

    for (int y = 0; y < brushHeight; ++y)
    {
        for (int x = 0; x < brushWidth; ++x)
        {
            video::SColor brushPixel = pBrush->getPixel(x, y);

            f32 bp = brushPixel.getRed() / 255.0;

            if (bp > 0)
            {
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
 * Save pTerrainSceneNode height map
 *
 * @param pVideoDriver		IVideoDriver pointer
 * @param pTerrainSceneNode		ITerraiSceneNode pointer
 * @param heightMapName	name of pTerrainSceneNode height map
 */
void saveTerrainHeightMap(video::IVideoDriver* pVideoDriver, scene::ITerrainSceneNode* pTerrainSceneNode, core::stringc heightMapName)
{
    core::dimension2d<u32> dim(256, 256);
    video::IImage *img = pVideoDriver->createImage(video::ECF_R8G8B8, dim);

    u32 VertexCount = pTerrainSceneNode->getMesh()->getMeshBuffer(0)->getVertexCount();
    video::S3DVertex2TCoords* verts = (video::S3DVertex2TCoords*)pTerrainSceneNode->getMesh()->getMeshBuffer(0)->getVertices();

    for (u32 i = 0; i < VertexCount; ++i)
    {
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
 * Entry point
 *
 * @return application exit code
 */
s32 main(s32 argc, char** argv)
{
    // let user select pVideoDriver type
    video::E_DRIVER_TYPE pVideoDriverType = video::EDT_OPENGL/*EDT_DIRECT3D9*/;

    //video::E_DRIVER_TYPE pVideoDriverType = pVideoDriverChoiceConsole();
    //if (pVideoDriverType == video::EDT_COUNT)
    //	return 1;

    // create event receiver (with GUI events)
    CMasterEventReceiver* pEventReceiver = new CMasterEventReceiver();

    // create device
    IrrlichtDevice* pDevice = createDevice(pVideoDriverType, core::dimension2d<u32 > (800, 600), 32, false, false, false, pEventReceiver);
    if (!pDevice)
        return 0;

    // get video driver & scene manager
    video::IVideoDriver* pVideoDriver = pDevice->getVideoDriver();
    scene::ISceneManager* pSceneManager = pDevice->getSceneManager();
    //device->getCursorControl()->setVisible(false);

    // set default texture archive (folder)
    pDevice->getFileSystem()->addFileArchive("texture/");

    // add light
    //pSceneManager->addLightSceneNode(0, core::vector3df(0,2000,1000), video::SColorf(1.0f,1.0f,1.0f),20000);

    // set pTerrainSceneNode heightmap, brush file
    // TODO check what pointers needs exactly
    //io::path heightmapFile = "pTerrainSceneNode-heightmap.bmp";
    //pHeightmap = pVideoDriver->createImageFromFile(heightmapFile);
    pBrush = pVideoDriver->createImageFromFile("brush.png");
    //pTexture = pVideoDriver->createImageFromFile("fill.jpg");

    // Terrain
    /*scene::ITerrainSceneNode* pTerrainSceneNode = pSceneManager->addTerrainSceneNode(heightmapFile);
    f32 scale = 32.0f;
pTerrainSceneNode->setScale(core::vector3df(scale, 2.f, scale));
pTerrainSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);
pTerrainSceneNode->setMaterialTexture(0, pVideoDriver->getTexture("pTerrainSceneNode-texture3.jpg"));
    scene::ITriangleSelector* pTerrainSceneNodeSelector = pSceneManager->createTerrainTriangleSelector(pTerrainSceneNode, 0);*/
    
    // create skydome and texture manager with default properties
    scene::ISceneNode* skydome = pSceneManager->addSkyDomeSceneNode(pVideoDriver->getTexture("Data/textures/skydome.jpg"), 16, 8, 0.95f, 2.0f);
    skydome->setRotation(core::vector3df(0, -100, 0));
    CMultiTexturingManager* pTextureManager = new CMultiTexturingManager(pSceneManager);

    f32 pTerrainSceneNodeScaleFactor = 10.0f;
    // create a pTerrainSceneNode
    scene::ITerrainSceneNode* pTerrainSceneNode = pSceneManager->addTerrainSceneNode(
                                                                                     "Data/textures/splatting_test2.png", // height map
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

    //pTerrainSceneNode->setScale(core::vector3df(5,2,5));
    scene::ITriangleSelector* pTerrainSceneNodeSelector = pSceneManager->createTerrainTriangleSelector(pTerrainSceneNode, 0);

    //Add it to the manager
    pTextureManager->addNode(pTerrainSceneNode);

    //Set the passes
    //First Pass, with Sand, Gras and Rock
    CMultiTexturingManager::STexturePass *first_pass = pTextureManager->addPass(pTerrainSceneNode,
                                                                                pVideoDriver->getTexture("Data/textures/splat_1.tga"),
                                                                                pVideoDriver->getTexture("Data/textures/Rock.jpg"),
                                                                                pVideoDriver->getTexture("Data/textures/Gras.jpg"),
                                                                                pVideoDriver->getTexture("Data/textures/Sand.jpg"));

    //Second Pass with Snow, Mud and the Vulcano's base texture
    pTextureManager->addPass(pTerrainSceneNode,
                             pVideoDriver->getTexture("Data/textures/splat_2.tga"),
                             pVideoDriver->getTexture("Data/textures/Snow.jpg"),
                             pVideoDriver->getTexture("Data/textures/Mud.jpg"),
                             pVideoDriver->getTexture("Data/textures/Ashes.jpg"));

    //Third Pass: the base
    pTextureManager->addPass(pTerrainSceneNode,
                             pVideoDriver->getTexture("Data/textures/splat_3.tga"),
                             0,
                             0,
                             pVideoDriver->getTexture("Data/textures/Pebbles.jpg"));

    //Fourth Pass: the Lightmap
    pTextureManager->addPass(pTerrainSceneNode,
                             pVideoDriver->getTexture("Data/textures/lm_terrain.tga"),
                             pVideoDriver->getTexture("Data/textures/black.jpg"),
                             0,
                             0);

    //Fith Pass: Lava, drawn above the Lightmap to make it "glow"
    CMultiTexturingManager::STexturePass *anim_lava_pass = pTextureManager->addPass(pTerrainSceneNode,
                                                                                    pVideoDriver->getTexture("Data/textures/splat_4.tga"),
                                                                                    pVideoDriver->getTexture("Data/textures/lava_1.jpg"),
                                                                                    0,
                                                                                    0);

    // create camera & attach GOT (glide over terrain) animator to it
    scene::ICameraSceneNode* pCameraSceneNode = pSceneManager->addCameraSceneNode(); //addCameraSceneNodeFPS(0, 100.0f, .1f);
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

    //pTerrainSceneNode->setRotation(core::vector3df(0.0f, 180.0f, 0.0f));

    // Arrow
    /* scene::ISceneNode* arrow = pSceneManager->addAnimatedMeshSceneNode(pSceneManager->addArrowMesh("arrow",
     * 	video::SColor(255, 255, 0, 0), video::SColor(255, 0, 255, 0)), NULL);
     * 	   	arrow->setMaterialFlag(video::EMF_LIGHTING, false);
     * 	   	    arrow->setScale(core::vector3df(20,20,20));
     * 	   	       	   arrow->setRotation(core::vector3df(0,0,180));*/

    // create GUI manager & set its font
    gui::IGUIEnvironment* pGUIEnviroment = pDevice->getGUIEnvironment();
    gui::IGUIFont* font = pGUIEnviroment->getFont("fonthaettenschweiler.bmp");
    pGUIEnviroment->getSkin()->setFont(font);


    /*gui::IGUIStaticText* txt[2];
txt[0] = guienv->addStaticText(L"Left/Right Mouse Button : raise/lower\nF4/F5 : increase/decrease step\nW : switch wireframe\nS : save heightmap",
            core::rect<s32>(10,10,600,120));
txt[0]->setOverrideColor(video::SColor(255,150,100,60));


txt[1] = guienv->addStaticText(L"", core::rect<s32>(10,80,600,200));
txt[1]->setOverrideColor(video::SColor(255,0,255,0));*/

    // create GUI itself (menu bars)
    gui::CGUIMainMenu* MainMenu = new gui::CGUIMainMenu(pGUIEnviroment);
    // test commit

    /*for(s32 i = 0; i < 256; ++i)
            PaintTerrainWithTexture(pTerrainSceneNode, i);*/

    char c[50];
    f32 step = 2.f;
    f32 yAngle = 0.0f;
    f32 brushSize = 100.0f;
    bool wireframeMode = false;
    bool editMode = false;

    while (pDevice->run())
    {
        if (pDevice->isWindowActive())
        {
            // End event process
            pEventReceiver->endEventProcess();


            // Key pressing events

            // exit application
            if (pEventReceiver->isKeyPressed(KEY_ESCAPE))
                break;


                // Turn on/off wireframe mode
            else if (pEventReceiver->isKeyPressed(KEY_KEY_W))
            {
                wireframeMode = !wireframeMode;
                pTerrainSceneNode->setMaterialFlag(video::EMF_WIREFRAME, wireframeMode);
            }
                /*if (pEventReceiver->isKeyPressed(KEY_KEY_Q))
                {
                        video::ITexture* Tex = pVideoDriver->getTexture("Data/textures/splat_1.tga");
                        video::SColor* pPixels = (video::SColor*)Tex->lock();
                        for(s32 i = 0; i < 32; ++i)
                        {
                                pPixels[32 + i].set(255, 0, 255, 0);
                        }
                        Tex->unlock();
                        first_pass->splat_texture = Tex;
                                
                }

                /*if(pEventReceiver->isKeyPressed(KEY_KEY_T))
                {
                        video::ITexture* Tex = pVideoDriver->getTexture("Data/textures/splat_1.tga");

                        video::SColor* pPixels = (video::SColor*)Tex->lock();
                        for(s32 i = 0; i < 10; ++i)
                        {
                                pPixels[i].set(255, 0, 255, 0);
                                                     /*if(index < 128) // 128 - TerrainSize
                            pPixels[(mod - index / 128) * (index + 1)].set(255, 0, 255, 0);
                        else
                            pPixels[(mod - index / 128) * (index % 128 + 1)].set(255, 0, 255, 0);*/
                /*RaiseTerrainVertex2(pTerrainSceneNode, i, 30, true);
            }
            Tex->unlock();
            first_pass->splat_texture = Tex;
        }*/



                // increase/descrease lift step
            else if (pEventReceiver->isKeyPressed(KEY_F4))
            {
                step += 1.f;
                yAngle++;
                brushSize++;
            }
            else if (pEventReceiver->isKeyPressed(KEY_F5) && step > 0)
            {
                step -= 1.f;
                yAngle--;
                brushSize--;
            }
            else if (pEventReceiver->isKeyPressed(KEY_SPACE))
            {
                core::vector3df cameraTarget = pTerrainSceneNode->getTerrainCenter();
                cameraTarget.Y += 10000;
                pCameraSceneNode->setPosition(cameraTarget);
                cameraTarget.Y -= 10000;
                pCameraSceneNode->setTarget(cameraTarget);
            }

            // GUI elements events


            // saving landscape (currently only hight map)
            if (pEventReceiver->getEventCallerByID() == gui::GUI_ID_BUTTON_SAVE_SCENE
                    && pEventReceiver->getEventCallerByElement(gui::EGET_BUTTON_CLICKED))
                saveTerrainHeightMap(pVideoDriver, pTerrainSceneNode, "heightmap_");

                //
            else if (pEventReceiver->getEventCallerByID() == gui::GUI_ID_BUTTON_LIFT_DOWN
                    && pEventReceiver->getEventCallerByElement(gui::EGET_BUTTON_CLICKED))
            {
                // Turn on/off lift/down-edit mode
                editMode = !editMode;

                // Turn on/off mouse control in GOTAnimator in pCameraSceneNode
                core::list<scene::ISceneNodeAnimator*>::ConstIterator cameraAnimatorList = pCameraSceneNode->getAnimators().begin();
                scene::CSceneNodeAnimatorCameraTerrain *GOTCameraAnimator = (scene::CSceneNodeAnimatorCameraTerrain*) * cameraAnimatorList;
                GOTCameraAnimator->setMouseActive(!GOTCameraAnimator->getMouseActive());
            }

            // move the arrow to the nearest vertex ...
            const core::position2di clickPosition = pDevice->getCursorControl()->getPosition();
            const core::line3df ray = pSceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(clickPosition, pCameraSceneNode);
            core::vector3df pos;
            core::triangle3df Tri;
            scene::ISceneNode* node = 0;
            if (pSceneManager->getSceneCollisionManager()->getCollisionPoint(ray, pTerrainSceneNodeSelector, pos, Tri, node))
            {
                //arrow->setPosition(pos);
                //static const s32 scale = /*32*/1; // pTerrainSceneNode is scaled 32X
                static const s32 size = 257; // heightmap is 512x512 pixels
                //printf("X: %f \t Z: %f\n", pos.X, pos.Z);
                s32 x = (s32) (pos.X / pTerrainSceneNodeScaleFactor);
                s32 z = (s32) (pos.Z / pTerrainSceneNodeScaleFactor);
                //drawCircleBrushBorder(pVideoDriver, pos / 10, 10.0f);
                s32 index = x * size + z;
                //printf("X: %d \t Z: %d\n", x, z);

                // ... Move it if clicked
                if ((pEventReceiver->isLeftMouseDown() || pEventReceiver->isRightMouseDown()) && editMode)
                {
                    video::ITexture* Tex = /*first_pass*/first_pass->splat_texture; //pVideoDriver->getTexture("Data/textures/splat_1.tga");
                    /*first_pass*/first_pass->splat_texture = ::textureBrush(pVideoDriver, pTerrainSceneNode, x, z, Tex);
                    //video::SColor* pPixels = (video::SColor*)Tex->lock();
                    //index = (z * 512 + (512 - x));
                    //pPixels[index].set(255, 0, 255, 0);                                
                    //Tex->unlock();
                    //first_pass->splat_texture = Tex;


                    //RaiseTerrainVertex(pTerrainSceneNode, index, step, eventReceiver->isLeftMouseDown());
                    //PaintTerrainWithTexture(pTerrainSceneNode, index);
                    //textureBrush(pVideoDriver, pTerrainSceneNode, x, z, texture);
                }

                x *= pTerrainSceneNodeScaleFactor;
                z *= pTerrainSceneNodeScaleFactor;

                //arrow->setPosition(core::vector3df(x, pTerrainSceneNode->getHeight(x, z) + 20, z));
            }
            
            core::stringw windowCaption = "LTerrain Editor [";
            windowCaption += pVideoDriver->getFPS();
            if (pSceneManager->getSceneCollisionManager()->getCollisionPoint(ray, pTerrainSceneNodeSelector, pos, Tri, node))
            {
                windowCaption += "] Mouse position: X:";
                windowCaption += pos.X;
                windowCaption += " Y:";
                windowCaption += pos.Y;
                windowCaption += " Z:";
                windowCaption += pos.Z;
            }
            pDevice->setWindowCaption(windowCaption.c_str());

            pVideoDriver->beginScene(true, true, video::SColor(50, 50, 50, 50));
            pSceneManager->drawAll();
            pTextureManager->drawAll();

            //sprintf(c,"elevation step : %.0f units", step);
            //txt[1]->setText(core::stringw(c).c_str());

            //drawCircleBrushBorder(pVideoDriver, pTerrainSceneNode, pTerrainSceneNode->getTerrainCenter(), brushSize, video::SColor(0, 255, 0, 0));
            drawSquareBrushBorder(pVideoDriver, pTerrainSceneNode, pTerrainSceneNode->getTerrainCenter(), brushSize, yAngle, video::SColor(0, 255, 0, 0));
            
            pGUIEnviroment->drawAll();
            
            pVideoDriver->endScene();

            // End event process
            pEventReceiver->startEventProcess();
        } else
            pDevice->yield();
    }

    //pHeightmap->drop();
    pBrush->drop();

    pTerrainSceneNodeSelector->drop();

    pDevice->closeDevice();
    pDevice->drop();

    return 0;
}