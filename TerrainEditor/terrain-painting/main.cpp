#include "irrlicht.h"
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;

/*==============================================================================
  Receiver class
==============================================================================*/
class MyEventReceiver : public IEventReceiver 
{ 
   bool KeyIsDown[KEY_KEY_CODES_COUNT];
   bool LEFTBUTTONCLICKED;
   bool RIGHTBUTTONCLICKED;

public: 
    virtual bool OnEvent(const SEvent& event) 
    { 
        if (event.EventType == irr::EET_KEY_INPUT_EVENT) 
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

        if(event.EventType == EET_MOUSE_INPUT_EVENT)
        {
         if     (event.MouseInput.Event==EMIE_LMOUSE_PRESSED_DOWN) LEFTBUTTONCLICKED = true;
         else if(event.MouseInput.Event==EMIE_LMOUSE_LEFT_UP     ) LEFTBUTTONCLICKED = false;
         else if(event.MouseInput.Event==EMIE_RMOUSE_PRESSED_DOWN) RIGHTBUTTONCLICKED = true;
         else if(event.MouseInput.Event==EMIE_RMOUSE_LEFT_UP     ) RIGHTBUTTONCLICKED = false;
        }

        return false; 
    } 

    virtual bool IsKeyDown(EKEY_CODE keyCode) const { return KeyIsDown[keyCode]; } 
    virtual bool IsLMBDown() const { return LEFTBUTTONCLICKED; } 
    virtual bool IsRMBDown() const { return RIGHTBUTTONCLICKED; } 

    MyEventReceiver() 
    { 
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
          KeyIsDown[i] = false;

        LEFTBUTTONCLICKED = RIGHTBUTTONCLICKED = false;
    } 
}; 

/*==============================================================================
  Raise or lower terrain (selected vertice)
==============================================================================*/
void RaiseTerrainVertex(ITerrainSceneNode* terrain, s32 index, f32 step, bool up)
{
   IMesh* pMesh = terrain->getMesh(); 

   s32 b; 
   for (b=0; b<pMesh->getMeshBufferCount(); ++b) 
   { 
      IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(b); 
      // skip mesh buffers that are not the right type 
      if (pMeshBuffer->getVertexType() != video::EVT_2TCOORDS) continue; 

      video::S3DVertex2TCoords* pVertices = (video::S3DVertex2TCoords*)pMeshBuffer->getVertices(); 

      pVertices[index].Pos.Y += (up) ? step : -step;
   }

   // force terrain render buffer to reload 
   terrain->setPosition(terrain->getPosition()); 
}

/*==============================================================================
  Save file
==============================================================================*/
void save (IVideoDriver* driver, ITerrainSceneNode* terrain)
{
   core::dimension2d<s32> dim (256,256);
   video::IImage *img = driver->createImage(ECF_R8G8B8, dim);

   u32 VertexCount = terrain->getMesh()->getMeshBuffer(0)->getVertexCount(); 
   S3DVertex2TCoords* verts = (S3DVertex2TCoords*)terrain->getMesh()->getMeshBuffer(0)->getVertices(); 

   for (u32 i=0; i<VertexCount; i++)
   {
     S3DVertex2TCoords* vertex = verts + i;
     u8 y = (u8)vertex->Pos.Y;
     img->setPixel((u32)vertex->Pos.X, (u32)vertex->Pos.Z, video::SColor(0, y,y,y));
   }

   driver->writeImageToFile(img, "heightmap.bmp", 0);
   img->drop();
}

/*==============================================================================
  Entry point
==============================================================================*/
int main()
{
  IrrlichtDevice* device = createDevice(EDT_OPENGL, dimension2d<s32>(800,600), 32, false);
  if (!device) return 0;

    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr  = device->getSceneManager();
    device->getCursorControl()->setVisible(false);
    device->setWindowCaption(L"terrain");
    MyEventReceiver receiver;
    device->setEventReceiver(&receiver);

  //lumière
	smgr->addLightSceneNode(0, core::vector3df(0,2000,1000), video::SColorf(1.0f,1.0f,1.0f),20000);

  // Terrain
  ITerrainSceneNode* terrain = smgr->addTerrainSceneNode("heightmap.bmp");
    terrain->setScale(core::vector3df(32, 2.f, 32));
    terrain->setMaterialFlag(video::EMF_LIGHTING, false);
    terrain->setMaterialTexture(0, driver->getTexture("aer.jpg"));
   ITriangleSelector* terrainSelector = smgr->createTerrainTriangleSelector(terrain, 0);

  // Camera
   scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS(0, 100.0f, .1f); 
   cam->setPosition(core::vector3df(-100,500,100)); 
   cam->setTarget(core::vector3df(0,0,0)); 

  // Arrow
   ISceneNode* arrow = smgr->addAnimatedMeshSceneNode(smgr->addArrowMesh("arrow", 
            SColor(255, 255, 0, 0), SColor(255, 0, 255, 0)), NULL);
   arrow->setMaterialFlag(video::EMF_LIGHTING, false);
   arrow->setScale(vector3df(20,20,20));
   arrow->setRotation(vector3df(0,0,180));

  // GUI
    IGUIEnvironment* guienv = device->getGUIEnvironment();
    IGUIFont* font = guienv->getFont("battlefield.bmp");
    guienv->getSkin()->setFont(font);

    IGUIStaticText* txt[2];
    txt[0] = guienv->addStaticText(L"Left/Right Mouse Button : raise/lower\nF4/F5 : increase/decrease step\nW : switch wireframe\nS : save heightmap",
                core::rect<s32>(10,10,600,120), false, true, 0, -1, false);
    txt[0]->setOverrideColor(video::SColor(255,150,100,60));


    txt[1] = guienv->addStaticText(L"", core::rect<s32>(10,80,600,200), false, true, 0, -1, false);
    txt[1]->setOverrideColor(video::SColor(255,0,255,0));


  ITimer* irrTimer = device->getTimer();
  u32 then = 0, then30 = 0;
  char c[24];
  f32 step = 2.f;
  bool wireframe = false;

  while(device->run()) if (device->isWindowActive()) 
   { 
      u32 now = irrTimer->getTime();
      if (then30 < now)
      {
        if(receiver.IsKeyDown(irr::KEY_ESCAPE)) break;


        if (receiver.IsKeyDown(irr::KEY_KEY_W) && then < now)
        {
          wireframe = !wireframe;
          terrain->setMaterialFlag(video::EMF_WIREFRAME, wireframe);
          then = now + 300;
        }

        if (receiver.IsKeyDown(irr::KEY_F4) && then < now)
        {
          step += 1.f;
          then = now + 100;
        }
        else if (receiver.IsKeyDown(irr::KEY_F5) && then < now && step > 0)
        {
          step -= 1.f;
          then = now + 100;
        }

        if(receiver.IsKeyDown(irr::KEY_KEY_S))
          save (driver, terrain);

        // move the arrow to the nearest vertex ...
        const position2di clickPosition = device->getCursorControl()->getPosition(); 
        const line3df ray = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(clickPosition, cam);
        vector3df pos;
        triangle3df Tri;
        if (smgr->getSceneCollisionManager()->getCollisionPoint(ray, terrainSelector, pos, Tri))
        {
          //arrow->setPosition(pos);
          static const s32 scale = 32; // terrain is scaled 32X
          static const s32 size = 256; // heightmap is 256X256 pixels
          s32 x = (s32)(pos.X / scale);
          s32 z = (s32)(pos.Z / scale);
          s32 index = x * size + z;

          // ... Move it if clicked
          if( (receiver.IsLMBDown() || receiver.IsRMBDown()) && then < now)
          {
            RaiseTerrainVertex(terrain, index, step, receiver.IsLMBDown());
            then = now + 100;
          }

          x *= scale;
          z *= scale;

          arrow->setPosition(vector3df(x, terrain->getHeight(x, z) + 20, z));
        }

        driver->beginScene(true, true, video::SColor(255,0,0,0)); 
        smgr->drawAll();

        sprintf(c,"elevation step : %.0f units", step);
        txt[1]->setText(stringw(c).c_str());

        guienv->drawAll();
        driver->endScene(); 

        then30 = now + 30;

      } // 30Hz
   }
 
  device->closeDevice();  
  device->drop();

  return 0;
}

