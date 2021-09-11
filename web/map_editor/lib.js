import createImpl from "https://unpkg.com/zustand@3.5.9/vanilla.mjs?module";
import { h, render } from "https://unpkg.com/preact@latest?module";
import htm from "https://unpkg.com/htm@3.1.0/dist/htm.module.js?module";
import shallow from "https://unpkg.com/zustand@3.5.9/shallow.mjs?module";
import {
    useCallback,
    useEffect,
    useLayoutEffect,
    useMemo,
    useReducer,
    useRef,
    useState,
} from "https://unpkg.com/preact@10/hooks/dist/hooks.module.js?module";

export const html = htm.bind(h);

export {
    h,
    render,
    shallow as shallowEqual,
    useCallback,
    useEffect,
    useLayoutEffect,
    useMemo,
    useReducer,
    useRef,
    useState,
};

/**
 * See: https://github.com/pmndrs/zustand/blob/main/src/index.ts
 *
 * Copied inline to avoid React dependency.
 *
 * Licensed under the MIT license: https://github.com/pmndrs/zustand/blob/main/LICENSE
 */
export function createStore(createState) {
    const api =
        typeof createState === "function"
            ? createImpl(createState)
            : createState;
    const useStore = (selector = api.getState, equalityFn = Object.is) => {
        const [, forceUpdate] = useReducer(c => c + 1, 0);

        const state = api.getState();
        const stateRef = useRef(state);
        const selectorRef = useRef(selector);
        const equalityFnRef = useRef(equalityFn);
        const erroredRef = useRef(false);

        const currentSliceRef = useRef();
        if (currentSliceRef.current === undefined) {
            currentSliceRef.current = selector(state);
        }

        let newStateSlice;
        let hasNewStateSlice = false;

        // The selector or equalityFn need to be called during the render phase if
        // they change. We also want legitimate errors to be visible so we re-run
        // them if they errored in the subscriber.
        if (
            stateRef.current !== state ||
            selectorRef.current !== selector ||
            equalityFnRef.current !== equalityFn ||
            erroredRef.current
        ) {
            // Using local variables to avoid mutations in the render phase.
            newStateSlice = selector(state);
            hasNewStateSlice = !equalityFn(
                currentSliceRef.current,
                newStateSlice
            );
        }

        // Syncing changes in useEffect.
        useLayoutEffect(() => {
            if (hasNewStateSlice) {
                currentSliceRef.current = newStateSlice;
            }
            stateRef.current = state;
            selectorRef.current = selector;
            equalityFnRef.current = equalityFn;
            erroredRef.current = false;
        });

        const stateBeforeSubscriptionRef = useRef(state);
        useLayoutEffect(() => {
            const listener = () => {
                try {
                    const nextState = api.getState();
                    const nextStateSlice = selectorRef.current(nextState);
                    if (
                        !equalityFnRef.current(
                            currentSliceRef.current,
                            nextStateSlice
                        )
                    ) {
                        stateRef.current = nextState;
                        currentSliceRef.current = nextStateSlice;
                        forceUpdate();
                    }
                } catch (error) {
                    erroredRef.current = true;
                    forceUpdate();
                }
            };
            const unsubscribe = api.subscribe(listener);
            if (api.getState() !== stateBeforeSubscriptionRef.current) {
                listener(); // state has changed before subscription
            }
            return unsubscribe;
        }, []);

        return hasNewStateSlice ? newStateSlice : currentSliceRef.current;
    };

    Object.assign(useStore, api);

    return useStore;
}
