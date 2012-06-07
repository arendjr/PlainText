(function(window) {

    var isActive = true;
    var permissionRequested = false;

    function init() {

        if (window.webkitNotifications) {
            window.notifications = window.webkitNotifications;
        }

        attachListeners();
    }

    function attachListeners() {

        window.onfocus = function() {
            isActive = true;
        };
        window.onblur = function() {
            isActive = false;
        };

        var onKeyPress = controller.commandInput.onkeypress;
        controller.commandInput.onkeypress = function(event) {
            onKeyPress.apply(controller.commandInput, [event]);

            if (event.keyCode === keys.KEY_RETURN) {
                notifications.requestPermission(function() {});
                controller.commandInput.onkeypress = onKeyPress;
            }
        }

        var writeToScreen = controller.writeToScreen;
        controller.writeToScreen = function(message) {
            writeToScreen.apply(controller, [message]);

            if (!isActive &&
                notifications.checkPermission() === 0 &&
                /^\w+ (says|tells|shouts)/.test(message)) {

                var notification = notifications.createNotification(null, "MUD", message);
                notification.show()

                setTimeout(function(){
                    notification.cancel();
                }, 10000);
            }
        }
    }

    init();

})(window);
