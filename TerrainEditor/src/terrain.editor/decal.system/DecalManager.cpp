/*
  Copyright (C) 2011 Thijs Ferket (RdR)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
 */

#include "../../../headers/terrain.editor/decal.system/DecalManager.h"
#include <map>
#include "../../../headers/terrain.editor/decal.system/CBatchingMesh.h"
#include "../../../headers/terrain.editor/decal.system/DecalSceneNode.h"

namespace irr {

    DecalManager::DecalManager(IrrlichtDevice* device) {
        this->device = device;
        this->smgr = device->getSceneManager();
        driver = smgr->getVideoDriver();
        terrain = 0;
        createMetaTriangleSelector();

        size = 40;
        lifetime = 0;
        distance = 0;
        rotation = 0;
        showWireFrame = false;
        useCameraNormal = false;
        useParent = false;
        decalTextureId = 1;
    }

    DecalManager::~DecalManager() {
        clear();
        metaSelector->drop();
    }

    void DecalManager::createMetaTriangleSelector() {
        metaSelector = smgr->createMetaTriangleSelector();
    }

    scene::IMetaTriangleSelector* DecalManager::getMetaTriangleSelector() {
        return metaSelector;
    }

    core::array<scene::DecalSceneNode*> DecalManager::getDecals(const io::path texturePath) {
        core::array<scene::ISceneNode*> nodes;
        core::array<scene::DecalSceneNode*> decals;

        smgr->getSceneNodesFromType((scene::ESCENE_NODE_TYPE) scene::DECAL_SCENE_NODE_ID, nodes, smgr->getRootSceneNode());
        for (u32 i = 0; i < nodes.size(); i++) {
            scene::DecalSceneNode* decal = (scene::DecalSceneNode*) nodes[i];
            if (decal->getTexture()->getName().getPath().equals_ignore_case(texturePath)) {
                decals.push_back(decal);
            }
        }

        nodes.clear();

        return decals;
    }

    core::array<scene::DecalSceneNode*> DecalManager::getPermanentDecals() {
        core::array<scene::ISceneNode*> nodes;
        core::array<scene::DecalSceneNode*> decals;

        smgr->getSceneNodesFromType((scene::ESCENE_NODE_TYPE) scene::DECAL_SCENE_NODE_ID, nodes, smgr->getRootSceneNode());
        for (u32 i = 0; i < nodes.size(); i++) {
            scene::DecalSceneNode* decal = (scene::DecalSceneNode*) nodes[i];
            if (decal->getLifetime() == 0) {
                decals.push_back(decal);
            }
        }

        nodes.clear();

        return decals;
    }

    core::array<scene::DecalSceneNode*> DecalManager::combineDecals(core::array<scene::DecalSceneNode*> decals) {
        core::array<scene::DecalSceneNode*> newDecals;

        if (decals.size() > 1) {
            std::map<io::path, scene::CBatchingMesh*> batchingMeshes = std::map<io::path, scene::CBatchingMesh*>();

            for (u32 i = 0; i < decals.size(); i++) {
                video::ITexture* texture = decals[i]->getTexture();
                scene::CBatchingMesh* batchingMesh = 0;

                // Check if already a batching mesh for this texture
                std::map<io::path, scene::CBatchingMesh*>::iterator iter = batchingMeshes.find(texture->getName().getPath());
                if (iter != batchingMeshes.end()) {
                    batchingMesh = iter->second;
                }// Create batching mesh
                else {
                    batchingMesh = new scene::CBatchingMesh();
                    batchingMeshes.insert(std::pair<io::path, scene::CBatchingMesh*>(texture->getName().getPath(), batchingMesh));
                }

                batchingMesh->addMesh(decals[i]->getMesh(), decals[i]->getAbsolutePosition());
                smgr->addToDeletionQueue(decals[i]);
            }

            // Finalize batching meshes and create new decal scene nodes
            for (std::map<io::path, scene::CBatchingMesh*>::iterator iter = batchingMeshes.begin(); iter != batchingMeshes.end(); iter++) {
                (iter->second)->finalize();

                // Create decal scene node
                video::ITexture* texture = smgr->getVideoDriver()->getTexture(iter->first);
                scene::DecalSceneNode* decal = new scene::DecalSceneNode(smgr->getRootSceneNode(), smgr, iter->second, texture);
                decal->drop();
                newDecals.push_back(decal);
            }

            decals.clear();
            batchingMeshes.clear();
        } else if (decals.size() == 1) {
            newDecals.push_back(decals[0]);
        }
        return newDecals;
    }

    void DecalManager::setTerrain(scene::ITerrainSceneNode* terrain) {
        this->terrain = terrain;
        metaSelector->addTriangleSelector(terrain->getTriangleSelector());
    }

    void DecalManager::addMesh(scene::IAnimatedMeshSceneNode* node) {
        addMesh(node->getMesh(), node);
    }

    void DecalManager::addMesh(scene::IMeshSceneNode* node) {
        addMesh(node->getMesh(), node);
    }

    void DecalManager::addMesh(scene::IMesh* mesh, scene::ISceneNode* node) {
        metaSelector->addTriangleSelector(smgr->createTriangleSelector(mesh, node));
    }

    void DecalManager::removeMesh(scene::ISceneNode* node) {
        metaSelector->removeTriangleSelector(node->getTriangleSelector());
    }

    void DecalManager::clear() {
        metaSelector->removeAllTriangleSelectors();
    }

    core::vector3df DecalManager::getNormal(core::vector3df position, const bool useTerrainOnly) {
        core::vector3df startPosition = position;
        if (terrain) {
            startPosition.Y = terrain->getBoundingBox().MaxEdge.Y;
        }

        core::vector3df endPosition = position;
        if (terrain) {
            endPosition.Y = terrain->getBoundingBox().MinEdge.Y;
        }

        return getNormal(core::line3df(startPosition, endPosition), useTerrainOnly);
    }

    core::vector3df DecalManager::getNormal(core::vector3df startPosition, core::vector3df endPosition, const bool useTerrainOnly) {
        core::vector3df direction = (endPosition - startPosition).normalize();
        return getNormal(core::line3df(startPosition, endPosition + direction), useTerrainOnly);
    }

    core::vector3df DecalManager::getNormal(core::line3df line, const bool useTerrainOnly) {
        core::vector3df normal = core::vector3df(0, 1, 0);

        core::triangle3df triangle;
        scene::ISceneNode* node = 0;
        core::vector3df collisionPoint;

        if (terrain && useTerrainOnly) {
            smgr->getSceneCollisionManager()->getCollisionPoint(line, terrain->getTriangleSelector(), collisionPoint, triangle, node);
        } else {
            smgr->getSceneCollisionManager()->getCollisionPoint(line, (scene::ITriangleSelector*) metaSelector, collisionPoint, triangle, node);
        }

        normal = triangle.getNormal();
        normal.normalize();

        return normal;
    }

    core::vector3df DecalManager::getCollisionPoint(core::vector3df position, const bool useTerrainOnly) {
        core::vector3df startPosition = position;
        if (terrain) {
            startPosition.Y = terrain->getBoundingBox().MaxEdge.Y;
        }

        core::vector3df endPosition = position;
        if (terrain) {
            endPosition.Y = terrain->getBoundingBox().MinEdge.Y;
        }

        core::line3df line = core::line3df(startPosition, endPosition);
        core::triangle3df triangle;
        scene::ISceneNode* node = 0;
        core::vector3df collisionPoint;

        if (terrain && useTerrainOnly) {
            smgr->getSceneCollisionManager()->getCollisionPoint(line, terrain->getTriangleSelector(), collisionPoint, triangle, node);
        } else {
            smgr->getSceneCollisionManager()->getCollisionPoint(line, (scene::ITriangleSelector*) metaSelector, collisionPoint, triangle, node);
        }
        return collisionPoint;
    }

    scene::DecalSceneNode* DecalManager::addDecal(io::path filename, core::vector3df position, core::vector3df dimension, core::vector3df normal, const f32 textureRotation,
            scene::ISceneNode* parent, const f32 lifeTime, const f32 distance, const bool useTerrainOnly) {
        return addDecal(driver->getTexture(filename), position, dimension, normal, textureRotation, parent, lifeTime, distance, useTerrainOnly);
    }

    scene::DecalSceneNode* DecalManager::addDecal(video::ITexture* texture, core::vector3df position, core::vector3df dimension, core::vector3df normal, const f32 textureRotation,
            scene::ISceneNode* parent, const f32 lifeTime, const f32 distance, const bool useTerrainOnly) {
        // Check if meta selector has any selectors added
        if (metaSelector->getTriangleCount() == 0) {
            return 0;
        }

        dimension *= 0.5f;

        // Create boxes
        core::aabbox3df box = core::aabbox3df(-dimension, dimension);

        box.MinEdge += position;
        box.MaxEdge += position;

        // Calculate rotation
        normal.normalize();
        core::quaternion quatDirection;
        quatDirection.rotationFromTo(core::vector3df(0, 1, 0), normal);
        core::vector3df rotation = quatDirection.getMatrix().getRotationDegrees();
        rotation.Y += textureRotation;

        // Create rotation matrix
        core::matrix4 rotationMatrix;
        rotationMatrix.setRotationDegrees(rotation);
        rotationMatrix.setRotationCenter(core::vector3df(0.5f, 0.5f, 0.5f), core::vector3df(0, 0, 0));

        // Create mesh
        scene::IMesh* mesh = createMesh(box, rotationMatrix, useTerrainOnly);

        // Create decal scene node
        if (parent == 0) {
            parent = smgr->getRootSceneNode();
        }

        position -= parent->getAbsolutePosition();
        position /= parent->getScale();

        scene::DecalSceneNode* decal = new scene::DecalSceneNode(parent, smgr, mesh, texture, position);
        decal->setLifetime(lifeTime);
        decal->setDistance(distance);

        decal->drop();

        return decal;
    }

    bool DecalManager::OnEvent(const SEvent &event) {
        if (event.EventType == EET_MOUSE_INPUT_EVENT) {
            // Create decal
            if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) {
                scene::ISceneNode* outNode = 0; // Change to 'const scene::ISceneNode* outNode;' for Irrlicht 1.7.x
                core::line3d<f32> line;
                core::vector3df collisionPoint;
                core::triangle3df triangle;
                line.start = smgr->getActiveCamera()->getAbsolutePosition();
                line.end = smgr->getActiveCamera()->getTarget();

                const core::line3df rayFromScreenCoordinates = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(device->getCursorControl()->getPosition(), smgr->getActiveCamera());
                scene::ITriangleSelector* selector = getMetaTriangleSelector();

                if (smgr->getSceneCollisionManager()->getCollisionPoint(rayFromScreenCoordinates, selector, collisionPoint, triangle, outNode)) {
                    printf("Collision detected\nX: %f Y: %f Z: %f\n", collisionPoint.X, collisionPoint.Y, collisionPoint.Z);
                    
                    core::vector3df normal = triangle.getNormal();
                    if (useCameraNormal) {
                        normal = line.start - line.end;
                    }
                    scene::ISceneNode* parent = 0;
                    if (useParent) {
                        //parent = sphere;
                    }

                    io::path filename = "media/decal.png";
                    if (decalTextureId == 2) {
                        filename = "media/decal2.png";
                    }
                    addDecal(filename, collisionPoint, core::vector3df(size, size, size), normal, rotation, parent, lifetime, distance);
                }
                return true;
            }
            // Rotate
            if (event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN) {
                rotation = (rand() % 3600) / 10.0f;
                return true;
            }

        } else if (event.EventType == EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown) {
            switch (event.KeyInput.Key) {

                    // switch terrain wireframe
                case KEY_KEY_T:
                {
                    if (decalTextureId == 1) {
                        decalTextureId = 2;
                    } else {
                        decalTextureId = 1;
                    }
                    return true;
                }
                    // Toggle decal wireframe
                case KEY_KEY_W:
                {
                    core::array<scene::ISceneNode*> nodes;
                    smgr->getSceneNodesFromType((scene::ESCENE_NODE_TYPE) scene::DECAL_SCENE_NODE_ID, nodes, smgr->getRootSceneNode());

                    showWireFrame = !showWireFrame;
                    for (u32 i = 0; i < nodes.size(); i++) {
                        nodes[i]->getMaterial(0).Wireframe = showWireFrame;
                    }

                    return true;
                }

                    // Random rotation
                case KEY_KEY_R:
                {
                    rotation = (rand() % 3600) / 10.0f;
                    return true;
                }

                    // Combine permanent decals into 1 decal
                case KEY_KEY_C:
                {
                    combineDecals(getPermanentDecals());
                    return true;
                }

                    // Increase size of decal
                case KEY_KEY_S:
                {
                    size++;
                    if (size > 20) {
                        size = 20;
                    }
                    return true;
                }
                    // Decrease size of decal
                case KEY_KEY_D:
                {
                    size--;
                    if (size < 1) {
                        size = 1;
                    }
                    return true;
                }

                    // Increase lifetime of decal
                case KEY_KEY_O:
                {
                    lifetime += 5;
                    return true;
                }
                    // Decrease lifetime of decal
                case KEY_KEY_L:
                {
                    lifetime -= 5;
                    if (lifetime < 0) {
                        lifetime = 0;
                    }
                    return true;
                }

                    // Increase distance of decal
                case KEY_KEY_I:
                {
                    distance += 50;
                    return true;
                }
                    // Decrease distance of decal
                case KEY_KEY_K:
                {
                    distance -= 50;
                    if (distance < 0) {
                        distance = 0;
                    }
                    return true;
                }
                    // Toggle use camera direction as normal
                case KEY_KEY_N:
                {
                    useCameraNormal = !useCameraNormal;
                    return true;
                }

                    // Toggle use spehre as parent
                case KEY_KEY_P:
                {
                    useParent = !useParent;
                    return true;
                }

                default:
                    break;
            }
        }

        return false;
    }
    
    f32 DecalManager::getRotation() const {
       return rotation; 
    }

    scene::IMesh* DecalManager::createMesh(core::aabbox3df box, core::matrix4 rotationMatrix, const bool useTerrainOnly) {
        // Get triangles
        core::array<core::triangle3df> triangles;
        s32 triangleCount = 0;
        if (terrain && useTerrainOnly) {
            triangles.set_used(terrain->getTriangleSelector()->getTriangleCount());
            terrain->getTriangleSelector()->getTriangles(&triangles[0], triangles.size(), triangleCount, box);
        } else {
            triangles.set_used(metaSelector->getTriangleCount());
            metaSelector->getTriangles(&triangles[0], triangles.size(), triangleCount, box);
        }

        // Create vertices and indices arrays
        core::array<video::S3DVertex> vertices = core::array<video::S3DVertex>();
        core::array<u16> indices = core::array<u16>();

        // Create translation matrix (Scale/Translate to 0,0 - 1,1 box)
        core::vector3df scale = core::vector3df(1, 1, 1) / box.getExtent();

        core::matrix4 m;
        m.setTranslation(-(box.MinEdge * scale));

        // Create scale matrix
        core::matrix4 scaleMatrix;
        scaleMatrix.setScale(scale);
        m *= scaleMatrix;

        // Clip all triangles and fill vertex and indices
        u32 vertexIndex = 0;
        //const f32 offsetScale = (scale.X + scale.Z) * 0.5f;
        std::map<core::vector3df, u32> positions;
        for (u32 i = 0; i < (u32) triangleCount; i++) {
            u32 index = 0;

            core::vector3df triangleNormal = triangles[i].getNormal().normalize();

            // Scale & Translate positions
            m.transformVect(triangles[i].pointA);
            m.transformVect(triangles[i].pointB);
            m.transformVect(triangles[i].pointC);

            // Rotate positions
            rotationMatrix.transformVect(triangles[i].pointA);
            rotationMatrix.transformVect(triangles[i].pointB);
            rotationMatrix.transformVect(triangles[i].pointC);

            // Clip triangle
            core::array<core::triangle3df> triangles2 = clipTriangle(triangles[i]);

            // Fill vertices and indices
            for (u32 t = 0; t < triangles2.size(); t++) {

                for (u32 p = 0; p < 3; p++) {
                    core::vector3df pos = triangles2[t].pointA;
                    if (p == 1) {
                        pos = triangles2[t].pointB;
                    } else if (p == 2) {
                        pos = triangles2[t].pointC;
                    }

                    // Search if vertex already exists in the vertices list
                    std::map<core::vector3df, u32>::iterator iter = positions.find(pos);
                    if (iter != positions.end()) {
                        index = iter->second;
                    }// Add vertex to list
                    else {
                        index = vertexIndex;
                        positions.insert(std::pair<core::vector3df, u32>(pos, vertexIndex));

                        // Set translation +0.01f to avoid flickering in draw
                        //pos += triangleNormal * (0.02f * offsetScale);
                        pos += triangleNormal * 0.01f;
                        vertices.push_back(video::S3DVertex(pos, triangleNormal, video::SColor(255, 255, 255, 255), core::vector2df(pos.X, 1 - pos.Z))); // 1 - pos.Z to flip the texture
                        vertexIndex++;
                    }
                    indices.push_back(index);
                }
            }
        }

        // Create meshBuffer
        scene::SMeshBuffer* meshBuffer = new scene::SMeshBuffer();
        meshBuffer->append(vertices.pointer(), vertices.size(), indices.pointer(), indices.size());

        // Clear arrays
        positions.clear();
        vertices.clear();
        indices.clear();

        // Inverse rotation matrix to restore original vertex positions
        rotationMatrix.makeInverse();
        smgr->getMeshManipulator()->transform(meshBuffer, rotationMatrix);

        // Inverse scaling matrix to restore original vertex positions
        m.makeInverse();
        m.setTranslation(-(box.getExtent() * 0.5f)); // Translate center to 0,0

        // Create batching mesh
        scene::CBatchingMesh* batchingMesh = new scene::CBatchingMesh();
        batchingMesh->addMeshBuffer(meshBuffer, m);

        meshBuffer->drop();
        batchingMesh->finalize();

        return batchingMesh;
    }

    core::array<core::triangle3df> DecalManager::clipTriangle(core::triangle3df triangle) {
        core::aabbox3df box = core::aabbox3df(0, 0, 0, 1, 1, 1);

        // Create list of triangles
        core::array<core::triangle3df> triangles;

        // Total inside box (no need for further checks)
        if (triangle.isTotalInsideBox(box)) {
            triangles.push_back(triangle);
            return triangles;
        }// Triangle total outside box
        else if (triangle.isTotalOutsideBox(box)) {
            return core::array<core::triangle3df>();
        }// Partial inside box
        else {
            triangles.push_back(triangle);
            return triangles;
        }
    }

} // end of namespace irr