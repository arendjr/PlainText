import { createStore } from "../lib.js";

export const useViewSettings = createStore(set => ({
    center: { x: 0, y: 0 },
    perspective: 0.0,
    showRoomNames: true,
    zRestriction: 0,
    zRestrictionEnabled: false,
    zoom: 0.2,

    move: (dx, dy) =>
        set(state => ({
            center: {
                x: state.center.x + (5.1 - 5 * state.zoom) * 20 * dx,
                y: state.center.y + (5.1 - 5 * state.zoom) * 20 * dy,
            },
        })),
    setPerspective: perspective => set({ perspective }),
    setShowRoomNames: showRoomNames => set({ showRoomNames }),
    setZRestriction: zRestriction => set({ zRestriction }),
    setZRestrictionEnabled: zRestrictionEnabled => set({ zRestrictionEnabled }),
    setZoom: zoom => set({ zoom }),
}));
