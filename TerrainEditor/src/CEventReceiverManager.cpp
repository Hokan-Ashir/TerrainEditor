/* 
 * File:   CEventReceiverManager.cpp
 * Author: hokan
 * 
 * Created on 24 Июль 2013 г., 22:54
 */

#include "../headers/CEventReceiverManager.h"

namespace irr {

    void CEventReceiverManager::addEventReceiver(SEventReceiver sEventReceiver) {
        // use linear search to preserve order. messages are 
        // sent to receivers in the order they register.      
        u32 searchIndex;
        for (searchIndex = 0; searchIndex < eventReceivers.size(); ++searchIndex) {
            if (eventReceivers[searchIndex].allowFutherEventsHandle == sEventReceiver.allowFutherEventsHandle
                    && eventReceivers[searchIndex].eventReceiverName == sEventReceiver.eventReceiverName
                    && eventReceivers[searchIndex].pEventReceiver == sEventReceiver.pEventReceiver)
                break;
        }
        if (searchIndex != eventReceivers.size())
            return;

        if (sEventReceiver.allowFutherEventsHandle)
            eventReceivers.push_front(sEventReceiver);
        else
            eventReceivers.push_back(sEventReceiver);
    }

    void CEventReceiverManager::removeEventReceiver(IEventReceiver* pEventReceiver) {
        u32 searchIndex;
        for (searchIndex = 0; searchIndex < eventReceivers.size(); ++searchIndex) {
            if (eventReceivers[searchIndex].pEventReceiver == pEventReceiver)
                break;
        }
        if (searchIndex != eventReceivers.size())
            eventReceivers.erase(searchIndex);
    }

    void CEventReceiverManager::removeEventReceiverByName(core::stringw eventReceiverName) {
        u32 searchIndex;
        for (searchIndex = 0; searchIndex < eventReceivers.size(); ++searchIndex) {
            if (eventReceivers[searchIndex].eventReceiverName == eventReceiverName)
                break;
        }
        if (searchIndex != eventReceivers.size())
            eventReceivers.erase(searchIndex);
    }

    bool CEventReceiverManager::OnEvent(const SEvent& event) {
        for (u32 i = 0; i < eventReceivers.size(); ++i) {
            if (eventReceivers[i].pEventReceiver->OnEvent(event) && !eventReceivers[i].allowFutherEventsHandle)
                return true; // only let one receiver handle message 
        }

        return false;
    }
} // end of namespace irr