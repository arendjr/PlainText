import {
    html,
    useCallback,
    useLayoutEffect,
    useRef,
    useState,
} from "../lib.js";

export function Slider({
    className,
    handleMargin = 5,
    onChange,
    value,
    width = 200,
}) {
    const style = { width };
    const handleStyle = { left: value * width - handleMargin };

    const [isDragged, setDragged] = useState(false);
    const dragState = useRef({
        left: 0,
        right: 0,
        currentX: 0,
        offsetX: 0,
    });

    const move = useCallback(
        event => {
            dragState.current.currentX = bound(
                event.clientX + dragState.current.offsetX,
                dragState.current.left,
                dragState.current.right
            );

            const value =
                (dragState.current.currentX - dragState.current.left) / width;
            onChange(value);
        },
        [onChange]
    );

    const stop = useCallback(event => {
        event.preventDefault();
        event.stopPropagation();

        setDragged(false);
    }, []);

    useLayoutEffect(() => {
        if (!isDragged) {
            return;
        }

        window.addEventListener("mousemove", move, false);
        window.addEventListener("mouseup", stop, false);

        return () => {
            window.removeEventListener("mousemove", move, false);
            window.removeEventListener("mouseup", stop, false);
        };
    }, [isDragged, move, stop]);

    function onMouseDown(event) {
        const handle = event.target;
        const slider = handle.parentNode;
        const sliderRect = slider.getBoundingClientRect();
        const handleRect = handle.getBoundingClientRect();

        dragState.current.left = sliderRect.left;
        dragState.current.right = sliderRect.right;
        dragState.current.currentX = (handleRect.left + handleRect.right) >> 1;
        dragState.current.offsetX = dragState.current.currentX - event.clientX;

        setDragged(true);
    }

    return html`
        <div
            class=${["slider"].concat(className).filter(Boolean).join(" ")}
            style=${style}
        >
            <div
                class="handle"
                onMouseDown=${onMouseDown}
                style=${handleStyle}
            />
        </div>
    `;
}

function bound(value, min, max) {
    return value < min ? min : value > max ? max : value;
}
