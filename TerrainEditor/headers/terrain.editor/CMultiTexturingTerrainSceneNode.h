/* 
 * File:   CMultiTexturingTerrainSceneNode.h
 * Author: hokan
 *
 * Created on 2 Август 2013 г., 14:59
 */

#ifndef CMULTITEXTURINGTERRAINSCENENODE_H
#define	CMULTITEXTURINGTERRAINSCENENODE_H

#include <ITerrainSceneNode.h>
#include <ISceneManager.h>
#include <IMaterialRendererServices.h>
#include <IVideoDriver.h>
#include <IShaderConstantSetCallBack.h>
#include <IGPUProgrammingServices.h>
#include "CTerrainSceneNode.h"

namespace irr {
    namespace scene {

        struct STexturePass {
            video::ITexture *splat_texture;
            video::ITexture *red_texture;
            video::ITexture *green_texture;
            video::ITexture *blue_texture;
        };

        class CMultiTexturingTerrainSceneNode : public CTerrainSceneNode, video::IShaderConstantSetCallBack {
        public:
            CMultiTexturingTerrainSceneNode(ISceneManager* pSceneManager, const io::path& heightMapFileName,
                    ISceneNode* parent = 0, s32 id = -1,
                    const core::vector3df& position = core::vector3df(0.0f, 0.0f, 0.0f),
                    const core::vector3df& rotation = core::vector3df(0.0f, 0.0f, 0.0f),
                    const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
                    video::SColor vertexColor = video::SColor(255, 255, 255, 255),
                    s32 maxLOD = 5, E_TERRAIN_PATCH_SIZE patchSize = ETPS_17, s32 smoothFactor = 0,
                    bool addAlsoIfHeightmapEmpty = false);

            virtual ~CMultiTexturingTerrainSceneNode();
            virtual void render();
            //void OnRegisterSceneNode();

            virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData);

            STexturePass *addPass(scene::ISceneNode *node);
            STexturePass *addPass(video::ITexture *splat, video::ITexture *red, video::ITexture *green, video::ITexture *blue);
            STexturePass *addPass(STexturePass *pass);

            bool removePass(u32 layer);
            bool removePass(STexturePass *pass);
        private:
            core::aabbox3d<f32> boundringBox;
            s32 shaderMaterial;
            core::array<STexturePass*> array_Passes;

        };
    } // end of namespace scene
} // end of namespace irr

#endif	/* CMULTITEXTURINGTERRAINSCENENODE_H */

