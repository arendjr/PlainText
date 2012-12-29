/*global define:false, require:false, CustomEvent:false*/
define(["controller"], function(Controller) {

    "use strict";

    function bound(min, value, max) {

        return (value < min) ? min : ((value > max) ? max : value);
    }


    function SliderWidget(element, options) {

        this.element = element;
        this.options = options || {};
        this.options.width = this.options.width || 200;

        this.init();
    }

    SliderWidget.prototype.init = function() {

        Controller.addStyle("sliderwidget/slider");

        this.element.style.width = this.options.width + "px";

        this.handleMargin = this.options.handleMargin || 5;

        this.handle = document.createElement("div");
        this.handle.setAttribute("class", "handle");
        this.handle.style.left = (-this.handleMargin) + "px";
        this.element.appendChild(this.handle);

        if (this.options.initialValue) {
            var left = this.options.initialValue * this.options.width;
            this.handle.style.left = (left - this.handleMargin) + "px";
        }

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

    return SliderWidget;
});
