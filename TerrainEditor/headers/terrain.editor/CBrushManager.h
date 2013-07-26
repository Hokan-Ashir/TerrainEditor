/* 
 * File:   CBrushManager.h
 * Author: hokan
 *
 * Created on 26 Июль 2013 г., 13:04
 */

#ifndef CBRUSHMANAGER_H
#define	CBRUSHMANAGER_H

#include <IEventReceiver.h>
#include <IVideoDriver.h>
#include <ITerrainSceneNode.h>

namespace irr {

    class CBrushManager : public IEventReceiver {
    public:
        CBrushManager(video::IVideoDriver* pVideoDriver, scene::ITerrainSceneNode* pTerrainSceneNode);
        CBrushManager(const CBrushManager& orig);
        virtual ~CBrushManager();
        void drawAll();
        void drawSquareBrushBorder(core::vector3df brushCenter,
                f32 brushRadius,
                f32 yAxisRotationAngle,
                video::SColor borderColour);
        void drawCircleBrushBorder(core::vector3df brushCenter,
                f32 brushRadius,
                video::SColor borderColour);
        virtual bool OnEvent(const SEvent& event);
        void setBrushYAngle(f32 brushYAngle);
        f32 getBrushYAngle() const;
        void setBrushSize(f32 brushSize);
        f32 getBrushSize() const;
        video:: IImage* getCurrentBrush() const;
    private:
        video::IImage* pBrush;
        video::IVideoDriver* pVideoDriver;
        scene::ITerrainSceneNode* pTerrainSceneNode;
        f32 brushYAngle;
        f32 brushSize;
    };
} // end of namespace irr 

#endif	/* CBRUSHMANAGER_H */

