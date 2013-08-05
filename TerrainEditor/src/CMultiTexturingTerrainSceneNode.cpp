/* 
 * File:   CMultiTexturingTerrainSceneNode.cpp
 * Author: hokan
 * 
 * Created on 2 Август 2013 г., 14:59
 */

#include "../headers/terrain.editor/CMultiTexturingTerrainSceneNode.h"
#include "IFileSystem.h"

namespace irr {
    namespace scene {

#ifdef MAKE_IRR_ID
        const int MULTY_TEX_TERRAIN_NODE_ID = MAKE_IRR_ID('m', 't', 'e', 'r');
#else
        const int MULTY_TEX_TERRAIN_NODE_ID = 'mter';
#endif // MAKE_IRR_ID

        CMultiTexturingTerrainSceneNode::CMultiTexturingTerrainSceneNode(ISceneManager* pSceneManager, const io::path& heightMapFileName,
                ISceneNode* parent, s32 id,
                const core::vector3df& position,
                const core::vector3df& rotation,
                const core::vector3df& scale,
                video::SColor vertexColor,
                s32 maxLOD, E_TERRAIN_PATCH_SIZE patchSize, s32 smoothFactor,
                bool addAlsoIfHeightmapEmpty)
        : CTerrainSceneNode(parent, pSceneManager, pSceneManager->getFileSystem(), id, maxLOD, patchSize, position, rotation, scale) {
            loadHeightMap(pSceneManager->getFileSystem()->createAndOpenFile(heightMapFileName), vertexColor, smoothFactor);
            ///GLSL Shader Code
            //Vertex Shader
            // TODO smth
            const c8 *vertShader = "void main(void)\
                            {\
                                gl_TexCoord[0] = gl_MultiTexCoord0;\
                                gl_TexCoord[1] = gl_MultiTexCoord1;\
                                gl_Position = ftransform();\
                            }\
                            ";

            //Fragment Shader
            const c8 *fragShader = "uniform sampler2D splatMap;\
                            uniform sampler2D layer_red;\
                            uniform sampler2D layer_green;\
                            uniform sampler2D layer_blue;\
                            \
                            void main(void)\
                            {\
                                vec4 SplatCol = texture2D(splatMap, vec2(gl_TexCoord[0]));\
                                vec4 RedCol = texture2D(layer_red, vec2(gl_TexCoord[1]));\
                                vec4 GreenCol = texture2D(layer_green, vec2(gl_TexCoord[1]));\
                                vec4 BlueCol = texture2D(layer_blue, vec2(gl_TexCoord[1]));\
                                gl_FragColor = (vec4(SplatCol.r * RedCol + SplatCol.g * GreenCol + SplatCol.b * BlueCol)) * vec4(1, 1, 1, SplatCol.a);\
                            }\
                            ";

            //Create the shader material
            shaderMaterial = pSceneManager->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterial(
                    vertShader, "main", video::EVST_VS_1_1,
                    fragShader, "main", video::EPST_PS_1_1,
                    this, video::EMT_TRANSPARENT_ALPHA_CHANNEL);
        }

        CMultiTexturingTerrainSceneNode::~CMultiTexturingTerrainSceneNode() {
            for (u32 i; i < array_Passes.size(); ++i) {
                array_Passes.erase(i);
            }
        }

        /*void CMultiTexturingTerrainSceneNode::OnRegisterSceneNode() {
            if (IsVisible)
                SceneManager->registerNodeForRendering(this);

            ISceneNode::OnRegisterSceneNode();
        }*/

        void CMultiTexturingTerrainSceneNode::render() {
            //CTerrainSceneNode::render();
            //I learned this meshbuffer trick from Viz_Fuerte's "Simple but useful projects"
            if (!SceneManager->isCulled(this)) {
                /*setVisible(true);
                OnRegisterSceneNode();
                updateAbsolutePosition();
                setVisible(false);*/
                //Reset the transformation
                SceneManager->getVideoDriver()->setTransform(video::ETS_WORLD, core::IdentityMatrix);

                for (u32 i = 0; i < array_Passes.size(); i++) {
                    setMaterialTexture(0, array_Passes[i]->splat_texture);
                    setMaterialTexture(1, array_Passes[i]->red_texture);
                    setMaterialTexture(2, array_Passes[i]->green_texture);
                    setMaterialTexture(3, array_Passes[i]->blue_texture);

                    video::SMaterial material = getMaterial(0);
                    material.MaterialType = (video::E_MATERIAL_TYPE)shaderMaterial;
                    material.MaterialTypeParam = video::pack_textureBlendFunc(video::EBF_DST_COLOR, video::EBF_ONE);

                    SceneManager->getVideoDriver()->setMaterial(material);
                    SceneManager->getVideoDriver()->drawMeshBuffer(getRenderBuffer());
                }
            }
        }

        STexturePass * CMultiTexturingTerrainSceneNode::addPass(scene::ISceneNode * node) {
            STexturePass *pass = new STexturePass();
            pass->red_texture = node->getMaterial(0).getTexture(0);
            pass->green_texture = node->getMaterial(0).getTexture(1);
            pass->blue_texture = node->getMaterial(0).getTexture(2);
            pass->splat_texture = node->getMaterial(0).getTexture(3);

            array_Passes.push_back(pass);
            return pass;
        }

        STexturePass * CMultiTexturingTerrainSceneNode::addPass(video::ITexture *splat, video::ITexture *red, video::ITexture *green, video::ITexture * blue) {
            STexturePass *pass = new STexturePass();
            pass->red_texture = red;
            pass->green_texture = green;
            pass->blue_texture = blue;
            pass->splat_texture = splat;

            array_Passes.push_back(pass);
            return pass;
        }

        STexturePass * CMultiTexturingTerrainSceneNode::addPass(STexturePass * pass) {
            array_Passes.push_back(pass);
            return pass;
        }

        bool CMultiTexturingTerrainSceneNode::removePass(u32 layer) {
            for (u32 j = 0; j < array_Passes.size(); j++) {
                if (j == layer)
                    array_Passes.erase(j);
                return true;
            }
            return false;
        }

        bool CMultiTexturingTerrainSceneNode::removePass(STexturePass * pass) {
            for (u32 j = 0; j < array_Passes.size(); j++) {
                if (array_Passes[j] == pass)
                    array_Passes.erase(j);
                return true;
            }
            return false;
        }

        void CMultiTexturingTerrainSceneNode::OnSetConstants(video::IMaterialRendererServices* services, s32 userData) {
            s32 layer0 = 0;
            s32 layer1 = 1;
            s32 layer2 = 2;
            s32 layer3 = 3;
            services->setPixelShaderConstant("splatMap", &layer0, 1);
            services->setPixelShaderConstant("layer_red", &layer1, 1);
            services->setPixelShaderConstant("layer_green", &layer2, 1);
            services->setPixelShaderConstant("layer_blue", &layer3, 1);
        }

    } // end of namespace scene
} // end of namespace irr