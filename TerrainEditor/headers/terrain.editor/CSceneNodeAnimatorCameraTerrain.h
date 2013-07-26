// Copyright (C) 2010 Vadim Zabrodin
// Based on the work of Nikolaus Gebhardt

// This is a special type of camera intended to work in a "glide over terrain" mode

#ifndef __C_SCENE_NODE_ANIMATOR_CAMERA_TERRAIN_H_INCLUDED__
#define __C_SCENE_NODE_ANIMATOR_CAMERA_TERRAIN_H_INCLUDED__

#include <irrlicht.h>
#include "vector2d.h"
#include "SKeyMap.h"
#include "irrArray.h"

namespace irr
{

namespace gui
{
	class ICursorControl;
}

namespace scene
{
	const ESCENE_NODE_ANIMATOR_TYPE ESNAT_GOT_CAM = (ESCENE_NODE_ANIMATOR_TYPE)
            MAKE_IRR_ID('a','G','O','T'); // GOT (Glide over Terrain) Animator

	enum EKEY_ACTION_TERRAIN
	{
		EKAT_MOVE_UP = 0,
		EKAT_MOVE_DOWN,
		EKAT_MOVE_LEFT,
		EKAT_MOVE_RIGHT,

		//! This value is not used. It only forces this enumeration to compile in 32 bit.
        EKAT_FORCE_32BIT = 0x7fffffff
	};

	//! Special scene node animator for RTS (or any other terrain-related) cameras
	class CSceneNodeAnimatorCameraTerrain : public ISceneNodeAnimator
	{
	public:

		// for extendible animator factory creation
		static ISceneNodeAnimator* create(IrrlichtDevice *device);

		//! Constructor
		CSceneNodeAnimatorCameraTerrain(gui::ICursorControl* cursorControl,
										ITerrainSceneNode* terrainNode = NULL,
										f32 rotateSpeed = 150.0f,
										f32 moveSpeed = 0.5f,
										SKeyMap* keyMapArray = 0,
										u32 keyMapSize = 0);

		//! Destructor
		virtual ~CSceneNodeAnimatorCameraTerrain();

		//! Animates the scene node
		virtual void animateNode(ISceneNode* node, u32 timeMs);

		//! Event receiver
		virtual bool OnEvent(const SEvent& event);

		//! Returns the speed of movement in units per second
		virtual f32 getMoveSpeed() const;

		//! Sets the speed of movement in units per second
		virtual void setMoveSpeed(f32 moveSpeed);

		//! Returns the rotation speed
		virtual f32 getRotateSpeed() const;

		//! Set the rotation speed
		virtual void setRotateSpeed(f32 rotateSpeed);

		//! Returns mouse active state
		virtual bool getMouseActive() const;

		//! Set mouse active state
		virtual void setMouseActive(bool mouseActive);

		//! Sets the keyboard mapping for this animator
		//! \param keymap: an array of keyboard mappings, see SKeyMap
		//! \param count: the size of the keyboard map array
		virtual void setKeyMap(SKeyMap *map, u32 count);

		//! This animator will receive events when attached to the active camera
		virtual bool isEventReceiverEnabled() const
		{
			return true;
		}

		//! Returns the type of this animator
		virtual ESCENE_NODE_ANIMATOR_TYPE getType() const
		{
			return ESNAT_GOT_CAM;
		}

		//! Creates a clone of this animator.
		/** Please note that you will have to drop
		(IReferenceCounted::drop()) the returned pointer once you're
		done with it. */
		virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager=0);

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0) const;

        virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0);

		struct SCamKeyMap
		{
			SCamKeyMap() {};
			SCamKeyMap(s32 a, EKEY_CODE k) : action(a), keycode(k) {}

			s32 action;
			EKEY_CODE keycode;
		};

		//! Sets the keyboard mapping for this animator
		/** Helper function for the clone method.
		\param keymap the new keymap array */
		void setKeyMap(const core::array<SCamKeyMap>& keymap);

	private:
		void allKeysUp();

		gui::ICursorControl *CursorControl;

		f32 MaxVerticalAngle;

		f32 MoveSpeed;
		f32 RotateSpeed;

		ITerrainSceneNode* p_terrainNode;

		s32 LastAnimationTime;

		core::array<SCamKeyMap> KeyMap;
		core::position2d<f32> RMouseDownCursor, CursorPos;

		bool MouseActive;
		bool RMouseDown, RMouseChange;
		float distanceTarget, deltaWheel;

		bool CursorKeys[6];

		bool firstUpdate;
		bool NoVerticalMovement;
	};

} // end namespace scene
} // end namespace irr

#endif // __C_SCENE_NODE_ANIMATOR_CAMERA_TERRAIN_H_INCLUDED__

