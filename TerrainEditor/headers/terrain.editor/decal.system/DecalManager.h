/**
 * DecalManager.h
 *
 * Tank @ War Project
 * October 2011
 */

#ifndef DECALMANAGER_H_
#define DECALMANAGER_H_

#include <irrlicht.h>
#include "DecalSceneNode.h"

namespace irr {

    class DecalManager : public IEventReceiver {
    private:
        IrrlichtDevice* device;
        scene::ISceneManager* smgr;
        video::IVideoDriver* driver;
        scene::ITerrainSceneNode* terrain;
        scene::IMetaTriangleSelector* metaSelector;

        f32 size;
        f32 lifetime;
        f32 rotation;
        f32 distance;
        bool showWireFrame;
        bool useCameraNormal;
        bool useParent;
        u32 decalTextureId;
    public:
        /**
         * Constructor of DecalManager
         * @param smgr
         */
        DecalManager(IrrlichtDevice* device);

        /**
         * Destructor of DecalManager
         */
        virtual ~DecalManager();

        /**
         * Create new meta triangle selector
         */
        void createMetaTriangleSelector();

        /**
         * Get meta triangle selector
         * @return metaSelector
         */
        scene::IMetaTriangleSelector* getMetaTriangleSelector();

        /**
         * Get all decals or decal with the same texture
         * @param texturePath (optional)
         * @return decals
         */
        core::array<scene::DecalSceneNode*> getDecals(const io::path texturePath = "");

        /**
         * Get permanent decals (decals with lifetime 0)
         * @return decals
         */
        core::array<scene::DecalSceneNode*> getPermanentDecals();

        /**
         * Combine multiple decals into 1 decal (better draw performance) make sure they have the same material
         * @param decals
         * @return decals
         */
        core::array<scene::DecalSceneNode*> combineDecals(core::array<scene::DecalSceneNode*> decals);

        /**
         * Set terrain
         * @param terrain
         */
        void setTerrain(scene::ITerrainSceneNode* terrain);

        /**
         * Add mesh to where decals can be created on
         * @param node
         */
        void addMesh(scene::IAnimatedMeshSceneNode* node);

        /**
         * Add mesh to where decals can be created on
         * @param node
         */
        void addMesh(scene::IMeshSceneNode* node);

        /**
         * Add mesh to where decals can be created on
         * @param mesh
         * @param node
         */
        void addMesh(scene::IMesh* mesh, scene::ISceneNode* node);

        /**
         * Remove mesh
         * @param node
         */
        void removeMesh(scene::ISceneNode* node);

        /**
         * Remove all triangle selectors from the meta selector
         */
        void clear();

        /**
         * Get normal of triangle on position, which creates a line from top to bottom of the terrain node
         * @param position
         * @param useTerrainOnly
         *
         * @return normal
         */
        core::vector3df getNormal(core::vector3df position, const bool useTerrainOnly = false);

        /**
         * Get normal of triangle on position
         * @param startPosition
         * @param endPosition
         * @param useTerrainOnly
         *
         * @return normal
         */
        core::vector3df getNormal(core::vector3df startPosition, core::vector3df endPosition, const bool useTerrainOnly = false);

        /**
         * Get normal of triangle on position
         * @param line
         * @param useTerrainOnly
         *
         * @return normal
         */
        core::vector3df getNormal(core::line3df line, const bool useTerrainOnly = false);

        /**
         * Get collision point
         * @param position
         * @param useTerrainOnly
         *
         * @return collisionPoint
         */
        core::vector3df getCollisionPoint(core::vector3df position, const bool useTerrainOnly = false);

        /**
         * Add decal to world
         * @param filename
         * @param position center position of the decal
         * @param dimension
         * @param normal orientation of the decal
         * @param textureRotation in degrees
         * @param parent
         * @param lifetime
         * @param distance
         * @param useTerrainOnly
         *
         * @return decal
         */
        scene::DecalSceneNode* addDecal(io::path filename, core::vector3df position, core::vector3df dimension = core::vector3df(1, 1, 1), core::vector3df normal = core::vector3df(0, 1, 0), const f32 textureRotation = 0, scene::ISceneNode* parent = 0,
                const f32 lifeTime = 0, const f32 distance = 0, const bool useTerrainOnly = false);

        /**
         * Add decal to world
         * @param texture
         * @param position center position of the decal
         * @param dimension
         * @param normal orientation of the decal
         * @param textureRotation in degrees
         * @param parent
         * @param lifetime
         * @param distance
         * @param useTerrainOnly
         *
         * @return decal
         */
        scene::DecalSceneNode* addDecal(video::ITexture* texture, core::vector3df position, core::vector3df dimension = core::vector3df(1, 1, 1), core::vector3df normal = core::vector3df(0, 1, 0), const f32 textureRotation = 0, scene::ISceneNode* parent = 0,
                const f32 lifeTime = 0, const f32 distance = 0, const bool useTerrainOnly = false);

        bool OnEvent(const SEvent &event);
    private:
        /**
         * Create mesh
         * @param box
         * @param rotationMatrix
         * @param useTerrainOnly
         *
         * @return mesh
         */
        scene::IMesh* createMesh(core::aabbox3df box, core::matrix4 rotationMatrix, const bool useTerrainOnly = false);

        /**
         * Clip triangle
         * @param triangle to clip
         *
         * @return triangles
         */
        core::array<core::triangle3df> clipTriangle(core::triangle3df triangle);
    };

} // end of namespace irr

#endif /* DECALMANAGER_H_ */
