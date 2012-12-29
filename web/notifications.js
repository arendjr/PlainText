/*global define:false, require:false*/
define(["controller"], function(Controller) {

    "use strict";

    var isActive = true;

    var notifications;

    function init() {

        if (window.webkitNotifications) {
            notifications = window.webkitNotifications;
        } else {
            notifications = window.notifications;
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
                Controller.removeCommandListener(requestPermission);
            });
        }

        Controller.addCommandListener(requestPermission);

        Controller.addIncomingMessageListener(function(message) {
            if (!isActive && notifications.checkPermission() === 0 &&
                /^\w+ (asks|says|tells|shouts)/.test(message)) {

                var notification = notifications.createNotification(null, "MUD", message);
                notification.show();

                setTimeout(function() {
                    notification.cancel();
                }, 10000);
            }
        });
    }

    init();
});
