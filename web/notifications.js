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

        window.addEventListener("focus", function() {
            isActive = true;
        }, false);

        window.addEventListener("blur", function() {
            isActive = false;
        }, false);

        function requestPermission() {
            notifications.requestPermission(function() {
                controller.removeCommandListener(requestPermission);
            });
        }

        controller.addCommandListener(requestPermission);

        controller.addIncomingMessageListener(function(message) {
            if (!isActive && notifications.checkPermission() === 0 &&
                /^\w+ (says|tells|shouts)/.test(message)) {

                var notification = notifications.createNotification(null, "MUD", message);
                notification.show()

                setTimeout(function() {
                    notification.cancel();
                }, 10000);
            }
        });
    }

    init();

})(window);
