import { createStore } from "../lib.js";

export const useRooms = createStore(set => ({
    rooms: new Map(),
    selectedRoomId: null,

    addRoom: room =>
        set(state => {
            const rooms = new Map(state.rooms);
            rooms.set(room.id, room);
            return { ...state, rooms };
        }),
    removeRoom: id =>
        set(state => {
            const rooms = new Map(state.rooms);
            rooms.delete(id);
            return {
                ...state,
                rooms,
                selectedRoomId:
                    state.selectedRoomId === id ? null : state.selectedRoomId,
            };
        }),
    setRooms: rooms =>
        set({ rooms: new Map(rooms.map(room => [room.id, room])) }),
    setSelectedRoomId: id => set({ selectedRoomId: id }),
    updateRoom: room =>
        set(state => {
            const rooms = new Map(state.rooms);
            rooms.set(room.id, room);
            return { ...state, rooms };
        }),
}));
