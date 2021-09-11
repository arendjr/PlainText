import { createStore } from "../lib.js";

export const usePortals = createStore(set => ({
    portals: new Map(),

    addPortal: portal =>
        set(state => {
            const portals = new Map(state.portals);
            portals.set(portal.id, portal);
            return { portals };
        }),
    removePortal: id =>
        set(state => {
            const portals = new Map(state.portals);
            portals.delete(id);
            return { portals };
        }),
    setPortals: portals =>
        set({ portals: new Map(portals.map(portal => [portal.id, portal])) }),
    updatePortal: portal =>
        set(state => {
            const portals = new Map(state.portals);
            portals.set(portal.id, portal);
            return { portals };
        }),
}));
