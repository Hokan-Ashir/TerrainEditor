#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;

//!Класс, отвечающий за передачу данных в шейдер
class cShaderCallback : public IShaderConstantSetCallBack
{
    public:
        cShaderCallback(){};

        //Основная функция, непосредственно выполняющая передачу переменных
        void OnSetConstants(IMaterialRendererServices* services, s32 userData)
        {
            //Номера текстурных слоёв.
            int baseTexture = 0;
            int layer1Texture = 1;
            int layer2Texture = 2;
            int layer3Texture = 3;
            int layer4Texture = 4;
            int layer5Texture = 5;
            int mask123Texture = 6;
            int mask45Texture = 7;
            //Передаём адреса текстур в шейдер, указывая на имена переменных,
            //которые должны их получить.
            services->setPixelShaderConstant("baseMap",(float*)(&baseTexture),1);
            services->setPixelShaderConstant("layer1",(float*)(&layer1Texture),1);
            services->setPixelShaderConstant("layer2",(float*)(&layer2Texture),1);
            services->setPixelShaderConstant("layer3",(float*)(&layer3Texture),1);
            services->setPixelShaderConstant("layer4",(float*)(&layer4Texture),1);
            services->setPixelShaderConstant("layer5",(float*)(&layer5Texture),1);
            services->setPixelShaderConstant("mask123",(float*)(&mask123Texture),1);
            services->setPixelShaderConstant("mask45",(float*)(&mask45Texture),1);
        }
};

//! M A I N
int main(int argc, char** argv)
{
    IrrlichtDevice *device =
        createDevice(EDT_OPENGL, dimension2d<u32>(640, 480), 32,
            false, true, false, 0);

    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();

    stringw txt("Max texture layers: "); txt += MATERIAL_MAX_TEXTURES;
    device->setWindowCaption(txt.c_str());

    //Настройка шейдера
    io::path psFileName = "shader/frag2.glsl"; //фрагментный (пиксельный) шейдер
        io::path vsFileName = "shader/vert.glsl"; //вертексный (вершинный) шейдер

    IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	cShaderCallback* sC = new cShaderCallback();
	s32 shaderMaterialType = gpu->addHighLevelShaderMaterialFromFiles(
                vsFileName, "vertexMain", video::EVST_VS_1_1,
				psFileName, "pixelMain", video::EPST_PS_1_1,
				sC, video::EMT_SOLID);

    if (!driver->queryFeature(video::EVDF_PIXEL_SHADER_1_1) &&
        !driver->queryFeature(video::EVDF_ARB_FRAGMENT_PROGRAM_1))
    {
        device->getLogger()->log("WARNING: Pixel shaders disabled "\
            "because of missing driver/hardware support.");
        psFileName = "";
    }

    if (!driver->queryFeature(video::EVDF_VERTEX_SHADER_1_1) &&
        !driver->queryFeature(video::EVDF_ARB_VERTEX_PROGRAM_1))
    {
        device->getLogger()->log("WARNING: Vertex shaders disabled "\
            "because of missing driver/hardware support.");
        vsFileName = "";
    }

    //Тестовая нода
    ISceneNode* node = smgr->addCubeSceneNode(25);

    if (node)
    {
        //Игнорируется по причине использования шейдера. Как работать со светом? Честно - пока не знаю.
        node->setMaterialFlag(EMF_LIGHTING, false);
        //Отключаю генерацию mipmaps`ов, т.к. на моей видеокарте (рабочий ноут) некорректно отображается результат
        //Советую включить и посмотреть как будет у вас. К тому же mipmaps`ы существенно увеличивают производительность в ущерб памяти.
        driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS,false);


        node->setMaterialTexture( 0, driver->getTexture("textures/grass.jpg") );
        node->setMaterialTexture( 1, driver->getTexture("textures/sand.jpg") );
        node->setMaterialTexture( 2, driver->getTexture("textures/dirt.jpg") );
        node->setMaterialTexture( 3, driver->getTexture("textures/stone.jpg") );
        node->setMaterialTexture( 4, driver->getTexture("textures/lava.jpg") );
        node->setMaterialTexture( 5, driver->getTexture("textures/water.jpg") );
        node->setMaterialTexture( 6, driver->getTexture("textures/mask123.jpg") );
        node->setMaterialTexture( 7, driver->getTexture("textures/mask45.jpg") );

        node->setMaterialType((E_MATERIAL_TYPE)shaderMaterialType); //!ВНИМАНИЕ. Назначаем тип материала по номеру шейдера

        //Итого 8 текстурных слоев
    }

    //Камера
    ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0,30,0.3);
    camera->setPosition(vector3df(0,30,0));
    camera->setTarget(vector3df(0,0,0));

    device->getCursorControl()->setVisible(false);

    //Основной цикл
    while(device->run())
    {
        driver->beginScene(true, true, SColor(0,200,200,200));

            smgr->drawAll();

        driver->endScene();
    }

    device->drop();

    return 0;
}

