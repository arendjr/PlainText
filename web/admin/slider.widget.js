function SliderWidget(element, options) {

    this.element = element;
    this.options = options || {};

    this.init();
}

SliderWidget.prototype.init = function() {

    var width = this.options.width || 200;
    this.element.style.width = width + "px";

    this.handleMargin = this.options.handleMargin || 5;

    this.handle = document.createElement("div");
    this.handle.setAttribute("class", "handle");
    this.handle.style.left = (-this.handleMargin) + "px";
    this.element.appendChild(this.handle);

    this.attachListeners();
};

SliderWidget.prototype.attachListeners = function() {

    var self = this;

    function move(event) {
        var newX = bound(self.leftX, event.clientX + self.offsetX, self.rightX);

        var left = (newX - self.leftX);
        self.handle.style.left = (left - self.handleMargin) + "px";

        self.currentX = newX;

        self.element.dispatchEvent(new CustomEvent("change", {
            "detail": {
                "value": left / self.options.width
            }
        }));
    }

    function stop(event) {
        window.removeEventListener("mousemove", move, false);
        window.removeEventListener("mouseup", stop, false);
        return false;
    }

    this.handle.addEventListener("mousedown", function(event) {
        var sliderRect = self.element.getBoundingClientRect();
        var handleRect = self.handle.getBoundingClientRect();

        self.leftX = sliderRect.left;
        self.rightX = sliderRect.right;
        self.currentX = (handleRect.left + handleRect.right) >> 1;
        self.offsetX = self.currentX - event.clientX;

        window.addEventListener("mousemove", move, false);
        window.addEventListener("mouseup", stop, false);
    }, false);
};

scriptLoaded("slider.widget.js");
