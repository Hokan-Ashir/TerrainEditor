/* 
 * File:   CGUIManager.h
 * Author: hokan
 *
 * Created on 26 Июль 2013 г., 16:22
 */

#ifndef CGUIMANAGER_H
#define	CGUIMANAGER_H

#include <IEventReceiver.h>
#include <IGUIContextMenu.h>
#include <ITerrainSceneNode.h>

#include "CTerrainEditor.h"

namespace irr {

    /**
     * GUI Manager which creates GUI and manage its events calling appropriate functions
     * that's why it must have so "heavy" pointers (to IrrlichtDevice & TerrainEditor classes)
     * 
     * inspired by 08.MeshViewer tutorial
     */
    class CGUIManager : public IEventReceiver {
    public:
        /**
         * Constructor
         * 
         * @param pDevice pointer to IrrlichtDevice class instance
         * @param pTerrainEditor        pointer to CTerrainEditor class instance
         */
        CGUIManager(IrrlichtDevice* pDevice, CTerrainEditor* pTerrainEditor);

        /**
         * Copy constructor (not implemented)
         * 
         * @param orig reference to original class instance
         */
        CGUIManager(const CGUIManager& orig) {
        };

        /**
         * Virtual destructor (not implemented)
         */
        virtual ~CGUIManager() {
        };

        /**
         * Operates events affecting on GUI - pressing buttons, selecting elements
         * 
         * @param event incomming event
         * @return true if incomming event processed
         */
        virtual bool OnEvent(const SEvent& event);

    private:
        void OnMenuItemSelected(gui::IGUIContextMenu* pGUIContextMenu);
        
        void OnButtonPressed(s32 id);
        
        /**
         * Pointer to CTerrainEditor class instance
         */
        CTerrainEditor* pTerrainEditor;

        /**
         * Pointer to IrrlichtDevice class instance
         */
        IrrlichtDevice* pDevice;        
    };

} // end of namespace irr

#endif	/* CGUIMANAGER_H */

