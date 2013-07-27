// Copyright (C) 2010 Vadim Zabrodin
// Based on the work of Nikolaus Gebhardt

// This is a special type of camera intended to work in a "glide over terrain" mode

#include "../../headers/terrain.editor/CSceneNodeAnimatorCameraTerrain.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "Keycodes.h"
#include "ICursorControl.h"
#include "ICameraSceneNode.h"
#include "ISceneNodeAnimatorCollisionResponse.h"

#define CAMERA_DIFF 3.f
#define MIN_CAMERA 20.f
#define MAX_CAMERA 18000.f
#define CAMERA_STEP 50.f

namespace irr {
    namespace scene {

        ISceneNodeAnimator* CSceneNodeAnimatorCameraTerrain::create(IrrlichtDevice* device) {
            return new CSceneNodeAnimatorCameraTerrain(device->getCursorControl());
        }

        //! constructor

        CSceneNodeAnimatorCameraTerrain::CSceneNodeAnimatorCameraTerrain(gui::ICursorControl* cursorControl,
                ITerrainSceneNode* terrainNode,
                f32 rotateSpeed, f32 moveSpeed,
                SKeyMap* keyMapArray, u32 keyMapSize)
        : CursorControl(cursorControl), MaxVerticalAngle(88.0f),
        MoveSpeed(moveSpeed), RotateSpeed(rotateSpeed),
        LastAnimationTime(0), firstUpdate(true), p_terrainNode(terrainNode), MouseActive(true) {
#ifdef _DEBUG
            setDebugName("CCameraSceneNodeAnimatorTerrain");
#endif

            if (CursorControl)
                CursorControl->grab();

            allKeysUp();

            RMouseDown = false;
            RMouseChange = false;
            deltaWheel = 0;
            distanceTarget = 100;

            // create key map
            if (!keyMapArray || !keyMapSize) {
                // create default key map
                KeyMap.push_back(SCamKeyMap(EKAT_MOVE_UP, KEY_UP));
                KeyMap.push_back(SCamKeyMap(EKAT_MOVE_DOWN, KEY_DOWN));
                KeyMap.push_back(SCamKeyMap(EKAT_MOVE_LEFT, KEY_LEFT));
                KeyMap.push_back(SCamKeyMap(EKAT_MOVE_RIGHT, KEY_RIGHT));
            } else {
                // create custom key map
                setKeyMap(keyMapArray, keyMapSize);
            }
        }


        //! destructor

        CSceneNodeAnimatorCameraTerrain::~CSceneNodeAnimatorCameraTerrain() {
            if (CursorControl)
                CursorControl->drop();
        }


        //! It is possible to send mouse and key events to the camera. Most cameras
        //! may ignore this input, but camera scene nodes which are created for
        //! example with scene::ISceneManager::addMayaCameraSceneNode or
        //! scene::ISceneManager::addFPSCameraSceneNode, may want to get this input
        //! for changing their position, look at target or whatever.

        bool CSceneNodeAnimatorCameraTerrain::OnEvent(const SEvent& evt) {
            switch (evt.EventType) {
                case EET_KEY_INPUT_EVENT:
                    for (u32 i = 0; i < KeyMap.size(); ++i) {
                        if (KeyMap[i].keycode == evt.KeyInput.Key) {
                            CursorKeys[KeyMap[i].action] = evt.KeyInput.PressedDown;
                            return true;
                        }
                    }
                    break;

                case EET_MOUSE_INPUT_EVENT:
                    if (evt.MouseInput.Event == EMIE_MOUSE_MOVED) {
                        CursorPos = CursorControl->getRelativePosition();
                        return true;
                    }
                    if (MouseActive && evt.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN) {
                        RMouseDownCursor = CursorControl->getRelativePosition();
                        RMouseDown = true;
                        RMouseChange = true;
                        return true;
                    }
                    if (MouseActive && evt.MouseInput.Event == EMIE_RMOUSE_LEFT_UP) {
                        RMouseDown = false;
                        return true;
                    }
                    if (evt.MouseInput.Event == EMIE_MOUSE_WHEEL) {
                        deltaWheel -= evt.MouseInput.Wheel * CAMERA_STEP;
                    }
                    break;

                default:
                    break;
            }

            return false;
        }

        void CSceneNodeAnimatorCameraTerrain::animateNode(ISceneNode* node, u32 timeMs) {
            if (!node || node->getType() != ESNT_CAMERA)
                return;

            ICameraSceneNode* camera = static_cast<ICameraSceneNode*> (node);

            if (firstUpdate) {
                LastAnimationTime = timeMs;
                core::vector3df tar = camera->getTarget();
                tar.Y = p_terrainNode->getHeight(tar.X, tar.Z);
                camera->setTarget(tar);
                firstUpdate = false;
            }

            // If the camera isn't the active camera, and receiving input, then don't process it.
            if (!camera->isInputReceiverEnabled())
                return;

            scene::ISceneManager * smgr = camera->getSceneManager();
            if (smgr && smgr->getActiveCamera() != camera)
                return;

            // get time
            f32 timeDiff = (f32) (timeMs - LastAnimationTime);
            LastAnimationTime = timeMs;

            // if terrain is not assigned, the camera is binded to the (X,0,Z) plane
            camera->updateAbsolutePosition();

            // update target position
            core::vector3df target = camera->getTarget();

            // Update rotation
            core::vector3df pos = (target - camera->getAbsolutePosition());
            float targetdiff = pos.getLength();
            core::vector3df relativeRotation = pos.getHorizontalAngle();

            if (CursorControl) {
                if (RMouseChange) {
                    RMouseDownCursor = CursorControl->getRelativePosition();
                    RMouseChange = false;
                } else {
                    if (RMouseDown && (CursorPos != RMouseDownCursor)) {
                        relativeRotation.Y -= (RMouseDownCursor.X - CursorPos.X) * RotateSpeed;
                        relativeRotation.X -= (RMouseDownCursor.Y - CursorPos.Y) * RotateSpeed;

                        // X < MaxVerticalAngle or X > 360-MaxVerticalAngle

                        if (relativeRotation.X > MaxVerticalAngle * 2 &&
                                relativeRotation.X < 360.0f - MaxVerticalAngle) {
                            relativeRotation.X = 360.0f - MaxVerticalAngle;
                        } else
                            if (relativeRotation.X > MaxVerticalAngle &&
                                relativeRotation.X < 360.0f - MaxVerticalAngle) {
                            relativeRotation.X = MaxVerticalAngle;
                        }

                        RMouseDownCursor = CursorControl->getRelativePosition();
                    }
                }
            }

            // update target

            pos.set(sin(relativeRotation.Y * core::DEGTORAD), 0.f, cos(relativeRotation.Y * core::DEGTORAD));

            if (CursorKeys[EKAT_MOVE_UP] || CursorPos.Y < 0.01)
                target += pos * timeDiff * MoveSpeed;

            if (CursorKeys[EKAT_MOVE_DOWN] || CursorPos.Y > 0.99)
                target -= pos * timeDiff * MoveSpeed;

            pos.set(-cos(relativeRotation.Y * core::DEGTORAD), 0.f, sin(relativeRotation.Y * core::DEGTORAD));

            if (CursorKeys[EKAT_MOVE_LEFT] || CursorPos.X < 0.01)
                target += pos * timeDiff * MoveSpeed;

            if (CursorKeys[EKAT_MOVE_RIGHT] || CursorPos.X > 0.99)
                target -= pos * timeDiff * MoveSpeed;

            // update target position
            if (!p_terrainNode)
                target.Y = 0;
            else if (deltaWheel != 0) {
                if (p_terrainNode->getHeight(target.X, target.Z) >= 0)
                    target.Y = p_terrainNode->getHeight(target.X, target.Z);
                //else
                //	target.Y = 0;
            }

            //process wheel

            if (deltaWheel != 0) {
                if (((distanceTarget + deltaWheel) > MIN_CAMERA) && ((distanceTarget + deltaWheel) < MAX_CAMERA))
                    distanceTarget += deltaWheel;

                deltaWheel = 0;
            }

            targetdiff = targetdiff * 0.9f + distanceTarget * 0.1f;

            camera->setTarget(target);

            // update camera position
            core::vector3df movedir = core::vector3df(0, 1.f, 0);
            movedir *= targetdiff;

            movedir.rotateYZBy(relativeRotation.X - 90);
            movedir.rotateXZBy(-relativeRotation.Y);

            core::vector3df add = core::vector3df(100, 100, -100);
            target = target + movedir;

            float checkHeight = 0;
            if (p_terrainNode)
                checkHeight = p_terrainNode->getHeight(target.X, target.Z);

            if (target.Y < (checkHeight + CAMERA_DIFF)) target.Y = checkHeight + CAMERA_DIFF;

            camera->setPosition(target);
        }

        void CSceneNodeAnimatorCameraTerrain::allKeysUp() {
            for (u32 i = 0; i < 6; ++i)
                CursorKeys[i] = false;
        }


        //! Sets the rotation speed

        void CSceneNodeAnimatorCameraTerrain::setRotateSpeed(f32 speed) {
            RotateSpeed = speed;
        }


        //! Sets the movement speed

        void CSceneNodeAnimatorCameraTerrain::setMoveSpeed(f32 speed) {
            MoveSpeed = speed;
        }

        //! Sets mouse active state

        void CSceneNodeAnimatorCameraTerrain::setMouseActive(bool mouseActive) {
            MouseActive = mouseActive;
        }

        //! Gets the rotation speed

        f32 CSceneNodeAnimatorCameraTerrain::getRotateSpeed() const {
            return RotateSpeed;
        }


        // Gets the movement speed

        f32 CSceneNodeAnimatorCameraTerrain::getMoveSpeed() const {
            return MoveSpeed;
        }

        // Gets mouse active state

        bool CSceneNodeAnimatorCameraTerrain::getMouseActive() const {
            return MouseActive;
        }

        //! Sets the keyboard mapping for this animator

        void CSceneNodeAnimatorCameraTerrain::setKeyMap(SKeyMap *map, u32 count) {
            // clear the keymap
            KeyMap.clear();

            // add actions
            for (u32 i = 0; i < count; ++i) {
                switch (map[i].Action) {
                    case EKAT_MOVE_UP: KeyMap.push_back(SCamKeyMap(EKAT_MOVE_UP, map[i].KeyCode));
                        break;
                    case EKAT_MOVE_DOWN: KeyMap.push_back(SCamKeyMap(EKAT_MOVE_DOWN, map[i].KeyCode));
                        break;
                    case EKAT_MOVE_LEFT: KeyMap.push_back(SCamKeyMap(EKAT_MOVE_LEFT, map[i].KeyCode));
                        break;
                    case EKAT_MOVE_RIGHT: KeyMap.push_back(SCamKeyMap(EKAT_MOVE_RIGHT, map[i].KeyCode));
                        break;
                    default:
                        break;
                }
            }
        }

        ISceneNodeAnimator* CSceneNodeAnimatorCameraTerrain::createClone(ISceneNode* node, ISceneManager* newManager) {
            CSceneNodeAnimatorCameraTerrain * newAnimator =
                    new CSceneNodeAnimatorCameraTerrain(CursorControl, p_terrainNode, RotateSpeed, MoveSpeed,
                    0, 0);
            newAnimator->setKeyMap(KeyMap);
            return newAnimator;
        }

        void CSceneNodeAnimatorCameraTerrain::setKeyMap(const core::array<SCamKeyMap>& keymap) {
            KeyMap = keymap;
        }

        void CSceneNodeAnimatorCameraTerrain::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const {
            if (!out)
                return;

            out->addFloat("rotateSpeed", RotateSpeed);
            out->addFloat("moveSpeed", MoveSpeed);
        }

        void CSceneNodeAnimatorCameraTerrain::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) {
            if (!in)
                return;

            RotateSpeed = in->getAttributeAsFloat("rotateSpeed");
            MoveSpeed = in->getAttributeAsFloat("moveSpeed");
        }

    } // namespace scene
} // namespace irr

