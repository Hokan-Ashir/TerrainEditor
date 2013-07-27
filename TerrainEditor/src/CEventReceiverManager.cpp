/* 
 * File:   CEventReceiverManager.cpp
 * Author: hokan
 * 
 * Created on 24 Июль 2013 г., 22:54
 */

#include "../headers/CEventReceiverManager.h"

namespace irr {

    /**
     * Add event receiver, its name and breaking-CoR-allow-flag to array of event receivers
     * 
     * @param eventReceiver struct of event receiver parameters
     */
    bool CEventReceiverManager::addEventReceiver(SEventReceiver sEventReceiver) {
        if (sEventReceiver.pEventReceiver == 0) {
            return false;
        }
        u32 searchIndex;
        for (searchIndex = 0; searchIndex < eventReceivers.size(); ++searchIndex) {
            if (eventReceivers[searchIndex].allowFutherEventsHandle == sEventReceiver.allowFutherEventsHandle
                    && eventReceivers[searchIndex].eventReceiverName == sEventReceiver.eventReceiverName
                    && eventReceivers[searchIndex].pEventReceiver == sEventReceiver.pEventReceiver)
                break;
        }
        if (searchIndex != eventReceivers.size())
            return false;

        // if receiver allow futher event handling instert it in front, otherwise push back
        if (sEventReceiver.allowFutherEventsHandle) {
            eventReceivers.push_front(sEventReceiver);
        } else {
            eventReceivers.push_back(sEventReceiver);
        }
        return true;
    }

    /**
     * Remove event receiver by pointer; uses linear search, so it's really slow, but really it doesn't need to launch often
     * 
     * @param pEventReceiver        pointer to IEventReceiver interface
     */
    void CEventReceiverManager::removeEventReceiver(IEventReceiver* pEventReceiver) {
        u32 searchIndex;
        for (searchIndex = 0; searchIndex < eventReceivers.size(); ++searchIndex) {
            if (eventReceivers[searchIndex].pEventReceiver == pEventReceiver) {
                break;
            }
        }
        if (searchIndex != eventReceivers.size()) {
            eventReceivers.erase(searchIndex);
        }
    }

    /**
     * Remove event receiver by name; uses linear search, so it's really slow, but really it doesn't need to launch often
     * 
     * @param eventReceiverName     name of event receiver to be removed
     */
    void CEventReceiverManager::removeEventReceiverByName(core::stringw eventReceiverName) {
        u32 searchIndex;
        for (searchIndex = 0; searchIndex < eventReceivers.size(); ++searchIndex) {
            if (eventReceivers[searchIndex].eventReceiverName == eventReceiverName) {
                break;
            }
        }
        if (searchIndex != eventReceivers.size()) {
            eventReceivers.erase(searchIndex);
        }
    }

    /**
     * Operates common OnEvent events from application
     * 
     * @param event incomming event
     * @return true if incomming event processed (this result will be used by animators, so don't neglect it)
     */
    bool CEventReceiverManager::OnEvent(const SEvent& event) {
        for (u32 i = 0; i < eventReceivers.size(); ++i) {
            // break CoR if flag equal true; no matter what result is
            if (eventReceivers[i].pEventReceiver->OnEvent(event) && !eventReceivers[i].allowFutherEventsHandle) {
                return true; // only let one receiver handle message 
            }
        }

        return false;
    }
} // end of namespace irr